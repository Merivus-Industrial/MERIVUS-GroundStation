#include "AiServiceSupervisor.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QNetworkRequest>
#include <QProcessEnvironment>
#include <QStandardPaths>
#include <QtGlobal>
#include <QUuid>

namespace {
const char* kTimedOutProperty = "merivusSupervisorTimedOut";
const char* kServiceName = "merivus-agent";

QString findAncestorPathContaining(const QString& startPath, const QString& relativePath)
{
    QDir dir(startPath);
    if (QFileInfo(startPath).isFile()) {
        dir = QFileInfo(startPath).absoluteDir();
    }

    while (dir.exists()) {
        const QString candidate = dir.filePath(relativePath);
        if (QFileInfo(candidate).exists()) {
            return dir.absolutePath();
        }

        if (!dir.cdUp()) {
            break;
        }
    }

    return QString();
}

QString findAgentRepoRoot()
{
    const QString marker = QStringLiteral("agent/app/__main__.py");
    const QStringList roots = QStringList()
        << QCoreApplication::applicationDirPath()
        << QDir::currentPath();

    for (const QString& root : roots) {
        const QString repoRoot = findAncestorPathContaining(root, marker);
        if (!repoRoot.isEmpty()) {
            return repoRoot;
        }
    }

    return QString();
}

QString findPythonExecutable()
{
    const QString envPython = qEnvironmentVariable("MERIVUS_AGENT_DEV_PYTHON").trimmed();
    if (!envPython.isEmpty()) {
        const QFileInfo pythonInfo(envPython);
        if (pythonInfo.exists() && pythonInfo.isFile()) {
            return pythonInfo.absoluteFilePath();
        }
    }

    const QStringList candidates = QStringList()
        << QStringLiteral("python.exe")
        << QStringLiteral("python")
        << QStringLiteral("py.exe")
        << QStringLiteral("py");

    for (const QString& candidate : candidates) {
        const QString executable = QStandardPaths::findExecutable(candidate);
        if (!executable.isEmpty()) {
            return executable;
        }
    }

    return QString();
}

QStringList pythonModuleArguments(const QString& pythonExecutable)
{
    const QString executableName = QFileInfo(pythonExecutable).fileName().toLower();
    if (executableName == QStringLiteral("py.exe") || executableName == QStringLiteral("py")) {
        return QStringList() << QStringLiteral("-3") << QStringLiteral("-m") << QStringLiteral("app");
    }

    return QStringList() << QStringLiteral("-m") << QStringLiteral("app");
}
}

AiServiceSupervisor::AiServiceSupervisor(QObject* parent)
    : QObject(parent)
{
    connect(&_process, &QProcess::started, this, [this]() {
        emit processRunningChanged();
        emit agentStarted();
        _setState(Starting);
        _startStartupPolling();
    });

    connect(&_process,
            static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this,
            [this](int exitCode, QProcess::ExitStatus exitStatus) {
                emit processRunningChanged();
                _stopStartupPolling();
                _stopRuntimeHealth();
                _setHealthReady(false);

                if (_stopping) {
                    _stopping = false;
                    _terminateTimer.stop();
                    _setOwnsProcess(false);
                    _clearLocalToken();
                    _setState(_enabled ? Stopped : Disabled);
                    emit agentStopped();
                    return;
                }

                if (_restartPending) {
                    _terminateTimer.stop();
                    _setOwnsProcess(false);
                    _clearLocalToken();
                    _queueRestartAfterExit();
                    return;
                }

                _setState(Crashed);
                _setLastError(QStringLiteral("Agent进程已退出。"));
                emit agentCrashed(exitCode, static_cast<int>(exitStatus));

                if (_ownsProcess && _autoStart && _enabled && _restartCount < kMaxRestartCount) {
                    _scheduleRestart();
                } else {
                    _setOwnsProcess(false);
                    _clearLocalToken();
                }
            });

    connect(&_process, &QProcess::errorOccurred, this, [this](QProcess::ProcessError error) {
        if (error == QProcess::FailedToStart) {
            const QString message = _process.errorString().isEmpty()
                ? QStringLiteral("Agent启动失败。")
                : _process.errorString();
            _stopStartupPolling();
            _setHealthReady(false);
            _setOwnsProcess(false);
            _clearLocalToken();
            _setLastError(message);
            _setState(Error);
            emit processRunningChanged();
            emit startFailed(message);
            return;
        }

        qWarning() << "MERIVUS Agent process error" << error << _process.errorString();
    });

    connect(&_process, &QProcess::stateChanged, this, [this]() {
        emit processRunningChanged();
    });

    connect(&_process, &QProcess::readyReadStandardOutput, this, [this]() {
        const QString output = QString::fromLocal8Bit(_process.readAllStandardOutput()).trimmed().left(800);
        if (!output.isEmpty()) {
            qDebug() << "MERIVUS Agent stdout:" << output;
        }
    });

    connect(&_process, &QProcess::readyReadStandardError, this, [this]() {
        const QString output = QString::fromLocal8Bit(_process.readAllStandardError()).trimmed().left(800);
        if (!output.isEmpty()) {
            qWarning() << "MERIVUS Agent stderr:" << output;
        }
    });

    connect(&_startupHealthTimer, &QTimer::timeout, this, [this]() {
        _requestHealth(HealthPurpose::StartupPoll);
    });
    _startupHealthTimer.setInterval(kStartupPollMs);

    connect(&_startupTimeoutTimer, &QTimer::timeout, this, [this]() {
        _stopStartupPolling();
        _setHealthReady(false);
        _setLastError(QStringLiteral("Agent启动超时。"));
        _setState(Error);
        if (_ownsProcess && _process.state() != QProcess::NotRunning) {
            _shutdownOwnedProcess();
        }
    });
    _startupTimeoutTimer.setSingleShot(true);

    connect(&_runtimeHealthTimer, &QTimer::timeout, this, [this]() {
        _requestHealth(HealthPurpose::Runtime);
    });
    _runtimeHealthTimer.setInterval(kRuntimeHealthMs);

    connect(&_terminateTimer, &QTimer::timeout, this, [this]() {
        _forceKillIfStillRunning();
    });
    _terminateTimer.setSingleShot(true);

    if (QCoreApplication::instance()) {
        connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, [this]() {
            _shutdownOwnedProcess();
        });
    }
}

AiServiceSupervisor::~AiServiceSupervisor()
{
    _shutdownOwnedProcess();
}

bool AiServiceSupervisor::processRunning() const
{
    return _process.state() != QProcess::NotRunning;
}

void AiServiceSupervisor::setEnabled(bool enabled)
{
    if (_enabled == enabled) {
        return;
    }

    _enabled = enabled;
    emit enabledChanged();

    if (!enabled) {
        stopAgent();
        _setHealthReady(false);
        _setState(Disabled);
        return;
    }

    _setState(Stopped);
}

void AiServiceSupervisor::setAutoStart(bool autoStart)
{
    if (_autoStart == autoStart) {
        return;
    }

    _autoStart = autoStart;
    emit autoStartChanged();
}

void AiServiceSupervisor::setProvider(const QString& provider)
{
    const QString normalized = _normalizedProvider(provider);
    if (_provider == normalized) {
        return;
    }

    _provider = normalized;
    emit providerSettingsChanged();
}

void AiServiceSupervisor::setOllamaBaseUrl(const QString& baseUrl)
{
    const QString normalized = _normalizedOllamaBaseUrl(baseUrl);
    if (_ollamaBaseUrl == normalized) {
        return;
    }

    _ollamaBaseUrl = normalized;
    emit providerSettingsChanged();
}

void AiServiceSupervisor::setOllamaModel(const QString& model)
{
    const QString normalized = _normalizedOllamaModel(model);
    if (_ollamaModel == normalized) {
        return;
    }

    _ollamaModel = normalized;
    emit providerSettingsChanged();
}

void AiServiceSupervisor::setOllamaTimeoutSeconds(int seconds)
{
    const int normalized = qBound(1, seconds, 300);
    if (_ollamaTimeoutSeconds == normalized) {
        return;
    }

    _ollamaTimeoutSeconds = normalized;
    emit providerSettingsChanged();
}

void AiServiceSupervisor::setAllowMockFallback(bool allow)
{
    if (_allowMockFallback == allow) {
        return;
    }

    _allowMockFallback = allow;
    emit providerSettingsChanged();
}

void AiServiceSupervisor::ensureRunning()
{
    if (!_enabled) {
        _setState(Disabled);
        return;
    }

    if (_healthReady) {
        return;
    }

    clearError();
    _setState(Checking);
    _requestHealth(HealthPurpose::PreStart);
}

void AiServiceSupervisor::startAgent()
{
    ensureRunning();
}

void AiServiceSupervisor::stopAgent()
{
    _restartPending = false;
    _stopStartupPolling();
    _stopRuntimeHealth();
    _setHealthReady(false);

    if (!_ownsProcess) {
        _clearLocalToken();
        _setState(_enabled ? Stopped : Disabled);
        return;
    }

    if (_process.state() == QProcess::NotRunning) {
        _setOwnsProcess(false);
        _clearLocalToken();
        _setState(_enabled ? Stopped : Disabled);
        emit agentStopped();
        return;
    }

    _setState(Stopping);
    _stopping = true;
    _process.terminate();
    _terminateTimer.start(kTerminateTimeoutMs);
}

void AiServiceSupervisor::restartAgent()
{
    clearError();
    _setRestartCount(0);

    if (!_ownsProcess && _healthReady) {
        _setLastError(QStringLiteral("当前Agent不是由MERIVUS启动，无法自动切换Provider；请手动停止外部Agent后重试。"));
        _requestHealth(HealthPurpose::Manual);
        return;
    }

    if (_ownsProcess && _process.state() != QProcess::NotRunning) {
        stopAgent();
        QTimer::singleShot(kTerminateTimeoutMs + 250, this, [this]() {
            if (_enabled) {
                ensureRunning();
            }
        });
        return;
    }

    _setHealthReady(false);
    ensureRunning();
}

void AiServiceSupervisor::checkHealth()
{
    if (!_enabled) {
        _setState(Disabled);
        return;
    }

    _setState(Checking);
    _requestHealth(HealthPurpose::Manual);
}

void AiServiceSupervisor::clearError()
{
    _setLastError(QString());
}

void AiServiceSupervisor::_requestHealth(HealthPurpose purpose)
{
    if (_healthReply) {
        return;
    }

    QNetworkRequest request(_healthUrl());
    request.setRawHeader("Accept", "application/json");
    QNetworkReply* reply = _networkManager.get(request);
    _healthReply = reply;

    QTimer* timeout = new QTimer(reply);
    timeout->setSingleShot(true);
    connect(timeout, &QTimer::timeout, this, [reply]() {
        if (reply && reply->isRunning()) {
            reply->setProperty(kTimedOutProperty, true);
            reply->abort();
        }
    });
    timeout->start(kHealthTimeoutMs);

    connect(reply, &QNetworkReply::finished, this, [this, reply, purpose]() {
        _handleHealthFinished(reply, purpose);
    });
}

void AiServiceSupervisor::_handleHealthFinished(QNetworkReply* reply, HealthPurpose purpose)
{
    if (!reply) {
        return;
    }

    const bool timedOut = reply->property(kTimedOutProperty).toBool();
    const QNetworkReply::NetworkError networkError = reply->error();
    const QByteArray body = reply->readAll();
    const QVariant statusAttribute = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    const int httpStatus = statusAttribute.isValid() ? statusAttribute.toInt() : 0;
    _healthReply.clear();

    if (timedOut) {
        _handleHealthFailure(purpose, QStringLiteral("Agent health检查超时。"), false);
        reply->deleteLater();
        return;
    }

    if (networkError != QNetworkReply::NoError) {
        _handleHealthFailure(purpose, reply->errorString(), false);
        reply->deleteLater();
        return;
    }

    if (httpStatus < 200 || httpStatus >= 300) {
        _handleHealthFailure(purpose, QStringLiteral("端口返回HTTP %1。").arg(httpStatus), true);
        reply->deleteLater();
        return;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(body, &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        _handleHealthFailure(purpose, QStringLiteral("端口响应不是兼容的MERIVUS Agent JSON。"), true);
        reply->deleteLater();
        return;
    }

    const QJsonObject object = document.object();
    const bool compatible = object.value(QStringLiteral("status")).toString() == QStringLiteral("ok") &&
        object.value(QStringLiteral("service")).toString() == QString::fromLatin1(kServiceName);
    if (!compatible) {
        _handleHealthFailure(purpose, QStringLiteral("端口上运行的不是MERIVUS Agent。"), true);
        reply->deleteLater();
        return;
    }

    _handleHealthSuccess(purpose);
    reply->deleteLater();
}

void AiServiceSupervisor::_handleHealthSuccess(HealthPurpose purpose)
{
    _runtimeFailureCount = 0;
    _stopStartupPolling();
    _setHealthReady(true);
    _setLastError(QString());

    if (purpose == HealthPurpose::PreStart && _process.state() == QProcess::NotRunning) {
        _setOwnsProcess(false);
        _clearLocalToken();
    }

    _setState(Healthy);
    _startRuntimeHealth();
    emit agentHealthy();
}

void AiServiceSupervisor::_handleHealthFailure(HealthPurpose purpose, const QString& message, bool portConflict)
{
    if (purpose == HealthPurpose::PreStart) {
        if (portConflict) {
            _setHealthReady(false);
            _setOwnsProcess(false);
            _clearLocalToken();
            _setLastError(message);
            _setState(PortConflict);
            return;
        }

        if (_autoStart) {
            _startResolvedAgent();
        } else {
            _setLastError(message);
            _setState(Stopped);
        }
        return;
    }

    if (purpose == HealthPurpose::StartupPoll) {
        return;
    }

    if (purpose == HealthPurpose::Runtime) {
        _runtimeFailureCount++;
        if (_runtimeFailureCount < kRuntimeFailureLimit) {
            return;
        }

        _setHealthReady(false);
        _setLastError(message);
        if (_ownsProcess && _autoStart && _enabled && _restartCount < kMaxRestartCount) {
            _scheduleRestart();
        } else {
            _setState(Error);
        }
        return;
    }

    _setHealthReady(false);
    _setLastError(message);
    _setState(Error);
}

void AiServiceSupervisor::_startResolvedAgent()
{
    if (_process.state() != QProcess::NotRunning) {
        return;
    }

    const LaunchSpec spec = _resolveLaunchSpec();
    if (!spec.valid) {
        _setHealthReady(false);
        _setOwnsProcess(false);
        _clearLocalToken();
        _setLastError(QStringLiteral("未找到本机Agent程序。"));
        _setState(NotInstalled);
        emit startFailed(_lastError);
        return;
    }

    _localToken = _generateLocalToken();
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    environment.insert(QStringLiteral("MERIVUS_AGENT_HOST"), QStringLiteral("127.0.0.1"));
    environment.insert(QStringLiteral("MERIVUS_AGENT_PORT"), QStringLiteral("8765"));
    environment.insert(QStringLiteral("MERIVUS_LOCAL_TOKEN"), _localToken);
    environment.insert(QStringLiteral("MERIVUS_AGENT_PROVIDER"), _provider);
    environment.insert(QStringLiteral("MERIVUS_OLLAMA_BASE_URL"), _ollamaBaseUrl);
    environment.insert(QStringLiteral("MERIVUS_OLLAMA_MODEL"), _ollamaModel);
    environment.insert(QStringLiteral("MERIVUS_OLLAMA_TIMEOUT_SECONDS"), QString::number(_ollamaTimeoutSeconds));
    environment.insert(QStringLiteral("MERIVUS_AGENT_ALLOW_MOCK_FALLBACK"), _allowMockFallback ? QStringLiteral("true") : QStringLiteral("false"));

    _process.setProgram(spec.program);
    _process.setArguments(spec.arguments);
    _process.setWorkingDirectory(spec.workingDirectory);
    _process.setProcessEnvironment(environment);
    _setOwnsProcess(true);
    _stopping = false;
    _runtimeFailureCount = 0;
    _setState(Starting);
    emit localTokenChanged(_localToken);
    _process.start();
}

AiServiceSupervisor::LaunchSpec AiServiceSupervisor::_resolveLaunchSpec()
{
    LaunchSpec spec;

    const QString releaseProgram = QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("agent/merivus-agent.exe"));
    QFileInfo releaseInfo(releaseProgram);
    if (releaseInfo.exists() && releaseInfo.isFile()) {
        spec.program = releaseInfo.absoluteFilePath();
        spec.workingDirectory = releaseInfo.absolutePath();
        spec.valid = true;
        _setProgramPath(spec.program);
        _setWorkingDirectory(spec.workingDirectory);
        return spec;
    }

    const QString repoRoot = findAgentRepoRoot();
    if (!repoRoot.isEmpty()) {
        const QString packagedProgram = QDir(repoRoot).filePath(QStringLiteral("agent/dist/merivus-agent/merivus-agent.exe"));
        QFileInfo packagedInfo(packagedProgram);
        if (packagedInfo.exists() && packagedInfo.isFile()) {
            spec.program = packagedInfo.absoluteFilePath();
            spec.workingDirectory = packagedInfo.absolutePath();
            spec.valid = true;
            _setProgramPath(spec.program);
            _setWorkingDirectory(spec.workingDirectory);
            return spec;
        }
    }

    const QString devPython = qEnvironmentVariable("MERIVUS_AGENT_DEV_PYTHON");
    const QString devRoot = qEnvironmentVariable("MERIVUS_AGENT_DEV_ROOT");
    QFileInfo pythonInfo(devPython);
    QDir rootDir(devRoot);
    if (!devPython.trimmed().isEmpty() && !devRoot.trimmed().isEmpty() && pythonInfo.exists() && pythonInfo.isFile() && rootDir.exists()) {
        spec.program = pythonInfo.absoluteFilePath();
        spec.arguments = pythonModuleArguments(spec.program);
        spec.workingDirectory = rootDir.absolutePath();
        spec.valid = true;
        _setProgramPath(spec.program);
        _setWorkingDirectory(spec.workingDirectory);
        return spec;
    }

    if (!repoRoot.isEmpty()) {
        const QString python = findPythonExecutable();
        if (!python.isEmpty()) {
            spec.program = python;
            spec.arguments = pythonModuleArguments(spec.program);
            spec.workingDirectory = QDir(repoRoot).filePath(QStringLiteral("agent"));
            spec.valid = true;
            _setProgramPath(spec.program);
            _setWorkingDirectory(spec.workingDirectory);
            return spec;
        }
    }

    _setProgramPath(QString());
    _setWorkingDirectory(QString());
    return spec;
}

QString AiServiceSupervisor::_generateLocalToken() const
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces) +
        QUuid::createUuid().toString(QUuid::WithoutBraces);
}

QUrl AiServiceSupervisor::_healthUrl() const
{
    QUrl url = _endpoint;
    url.setPath(QStringLiteral("/health"));
    return url;
}

void AiServiceSupervisor::_startStartupPolling()
{
    _setHealthReady(false);
    _startupHealthTimer.start();
    _startupTimeoutTimer.start(kStartupTimeoutMs);
    _requestHealth(HealthPurpose::StartupPoll);
}

void AiServiceSupervisor::_stopStartupPolling()
{
    _startupHealthTimer.stop();
    _startupTimeoutTimer.stop();
}

void AiServiceSupervisor::_startRuntimeHealth()
{
    if (!_runtimeHealthTimer.isActive()) {
        _runtimeHealthTimer.start();
    }
}

void AiServiceSupervisor::_stopRuntimeHealth()
{
    _runtimeHealthTimer.stop();
    _runtimeFailureCount = 0;
}

void AiServiceSupervisor::_scheduleRestart()
{
    _stopRuntimeHealth();
    _setHealthReady(false);
    _setRestartCount(_restartCount + 1);
    _setState(Crashed);
    _restartPending = true;

    if (_process.state() != QProcess::NotRunning) {
        _process.terminate();
        _terminateTimer.start(kTerminateTimeoutMs);
        return;
    }

    _queueRestartAfterExit();
}

void AiServiceSupervisor::_queueRestartAfterExit()
{
    const int delayMs = qMin(5000, 1000 * _restartCount);
    QTimer::singleShot(delayMs, this, [this]() {
        if (_enabled && _autoStart && _restartCount <= kMaxRestartCount) {
            _restartPending = false;
            _startResolvedAgent();
        } else {
            _restartPending = false;
        }
    });
}

void AiServiceSupervisor::_shutdownOwnedProcess()
{
    if (!_ownsProcess || _process.state() == QProcess::NotRunning) {
        return;
    }

    _setState(Stopping);
    _stopping = true;
    _process.terminate();
    _terminateTimer.start(kTerminateTimeoutMs);
}

void AiServiceSupervisor::_forceKillIfStillRunning()
{
    if (_ownsProcess && _process.state() != QProcess::NotRunning) {
        _process.kill();
    }
}

void AiServiceSupervisor::_clearLocalToken()
{
    if (_localToken.isEmpty()) {
        emit localTokenChanged(QString());
        return;
    }

    _localToken.clear();
    emit localTokenChanged(QString());
}

void AiServiceSupervisor::_setState(State state)
{
    QString text;
    switch (state) {
    case Disabled:
        text = QStringLiteral("未启用");
        break;
    case Stopped:
        text = QStringLiteral("Agent未启动");
        break;
    case Checking:
        text = QStringLiteral("正在检查");
        break;
    case Starting:
        text = QStringLiteral("正在启动");
        break;
    case Healthy:
        text = QStringLiteral("已连接");
        break;
    case Stopping:
        text = QStringLiteral("正在停止");
        break;
    case Crashed:
        text = QStringLiteral("Agent已崩溃");
        break;
    case NotInstalled:
        text = QStringLiteral("未找到本机Agent程序");
        break;
    case PortConflict:
        text = QStringLiteral("端口被占用");
        break;
    case Error:
        text = QStringLiteral("启动失败");
        break;
    }

    if (_state == state && _stateText == text) {
        return;
    }

    _state = state;
    _stateText = text;
    emit stateChanged();
}

void AiServiceSupervisor::_setHealthReady(bool ready)
{
    if (_healthReady == ready) {
        return;
    }

    _healthReady = ready;
    emit healthReadyChanged();
}

void AiServiceSupervisor::_setOwnsProcess(bool ownsProcess)
{
    if (_ownsProcess == ownsProcess) {
        return;
    }

    _ownsProcess = ownsProcess;
    emit ownsProcessChanged();
}

void AiServiceSupervisor::_setRestartCount(int count)
{
    if (_restartCount == count) {
        return;
    }

    _restartCount = count;
    emit restartCountChanged();
}

void AiServiceSupervisor::_setLastError(const QString& message)
{
    if (_lastError == message) {
        return;
    }

    _lastError = message;
    emit lastErrorChanged();
}

void AiServiceSupervisor::_setProgramPath(const QString& path)
{
    if (_programPath == path) {
        return;
    }

    _programPath = path;
    emit programPathChanged();
}

void AiServiceSupervisor::_setWorkingDirectory(const QString& path)
{
    if (_workingDirectory == path) {
        return;
    }

    _workingDirectory = path;
    emit workingDirectoryChanged();
}

QString AiServiceSupervisor::_normalizedProvider(const QString& provider) const
{
    const QString normalized = provider.trimmed().toLower();
    if (normalized == QStringLiteral("ollama")) {
        return QStringLiteral("ollama");
    }
    return QStringLiteral("mock");
}

QString AiServiceSupervisor::_normalizedOllamaBaseUrl(const QString& baseUrl) const
{
    QUrl url(baseUrl.trimmed());
    if (!url.isValid() || url.scheme() != QStringLiteral("http")) {
        return QStringLiteral("http://127.0.0.1:11434");
    }

    const QString host = url.host().toLower();
    if (host != QStringLiteral("127.0.0.1") && host != QStringLiteral("localhost")) {
        return QStringLiteral("http://127.0.0.1:11434");
    }

    QUrl normalized;
    normalized.setScheme(QStringLiteral("http"));
    normalized.setHost(QStringLiteral("127.0.0.1"));
    normalized.setPort(url.port(11434));
    return normalized.toString(QUrl::RemovePath | QUrl::RemoveQuery | QUrl::RemoveFragment).trimmed();
}

QString AiServiceSupervisor::_normalizedOllamaModel(const QString& model) const
{
    const QString normalized = model.trimmed();
    return normalized.isEmpty() ? QStringLiteral("qwen3:8b") : normalized;
}
