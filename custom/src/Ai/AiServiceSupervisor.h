#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPointer>
#include <QProcess>
#include <QTimer>
#include <QUrl>

class AiServiceSupervisor : public QObject
{
    Q_OBJECT

public:
    explicit AiServiceSupervisor(QObject* parent = nullptr);
    ~AiServiceSupervisor() override;

    enum State {
        Disabled,
        Stopped,
        Checking,
        Starting,
        Healthy,
        Stopping,
        Crashed,
        NotInstalled,
        PortConflict,
        Error,
    };
    Q_ENUM(State)

    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool autoStart READ autoStart WRITE setAutoStart NOTIFY autoStartChanged)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(QString stateText READ stateText NOTIFY stateChanged)
    Q_PROPERTY(bool processRunning READ processRunning NOTIFY processRunningChanged)
    Q_PROPERTY(bool healthReady READ healthReady NOTIFY healthReadyChanged)
    Q_PROPERTY(bool ownsProcess READ ownsProcess NOTIFY ownsProcessChanged)
    Q_PROPERTY(int restartCount READ restartCount NOTIFY restartCountChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)
    Q_PROPERTY(QString programPath READ programPath NOTIFY programPathChanged)
    Q_PROPERTY(QString workingDirectory READ workingDirectory NOTIFY workingDirectoryChanged)
    Q_PROPERTY(QString provider READ provider WRITE setProvider NOTIFY providerSettingsChanged)
    Q_PROPERTY(QString ollamaBaseUrl READ ollamaBaseUrl WRITE setOllamaBaseUrl NOTIFY providerSettingsChanged)
    Q_PROPERTY(QString ollamaModel READ ollamaModel WRITE setOllamaModel NOTIFY providerSettingsChanged)
    Q_PROPERTY(int ollamaTimeoutSeconds READ ollamaTimeoutSeconds WRITE setOllamaTimeoutSeconds NOTIFY providerSettingsChanged)
    Q_PROPERTY(bool allowMockFallback READ allowMockFallback WRITE setAllowMockFallback NOTIFY providerSettingsChanged)

    bool enabled() const { return _enabled; }
    bool autoStart() const { return _autoStart; }
    State state() const { return _state; }
    QString stateText() const { return _stateText; }
    bool processRunning() const;
    bool healthReady() const { return _healthReady; }
    bool ownsProcess() const { return _ownsProcess; }
    int restartCount() const { return _restartCount; }
    QString lastError() const { return _lastError; }
    QString programPath() const { return _programPath; }
    QString workingDirectory() const { return _workingDirectory; }
    QString provider() const { return _provider; }
    QString ollamaBaseUrl() const { return _ollamaBaseUrl; }
    QString ollamaModel() const { return _ollamaModel; }
    int ollamaTimeoutSeconds() const { return _ollamaTimeoutSeconds; }
    bool allowMockFallback() const { return _allowMockFallback; }

    void setEnabled(bool enabled);
    void setAutoStart(bool autoStart);
    void setProvider(const QString& provider);
    void setOllamaBaseUrl(const QString& baseUrl);
    void setOllamaModel(const QString& model);
    void setOllamaTimeoutSeconds(int seconds);
    void setAllowMockFallback(bool allow);

    Q_INVOKABLE void ensureRunning();
    Q_INVOKABLE void startAgent();
    Q_INVOKABLE void stopAgent();
    Q_INVOKABLE void restartAgent();
    Q_INVOKABLE void checkHealth();
    Q_INVOKABLE void clearError();

signals:
    void enabledChanged();
    void autoStartChanged();
    void stateChanged();
    void processRunningChanged();
    void healthReadyChanged();
    void ownsProcessChanged();
    void restartCountChanged();
    void lastErrorChanged();
    void programPathChanged();
    void workingDirectoryChanged();
    void providerSettingsChanged();
    void localTokenChanged(const QString& token);

    void agentStarted();
    void agentHealthy();
    void agentStopped();
    void agentCrashed(int exitCode, int exitStatus);
    void startFailed(const QString& message);

private:
    enum class HealthPurpose {
        Manual,
        PreStart,
        StartupPoll,
        Runtime,
    };

    struct LaunchSpec {
        QString program;
        QStringList arguments;
        QString workingDirectory;
        bool valid = false;
    };

    void _requestHealth(HealthPurpose purpose);
    void _handleHealthFinished(QNetworkReply* reply, HealthPurpose purpose);
    void _handleHealthSuccess(HealthPurpose purpose);
    void _handleHealthFailure(HealthPurpose purpose, const QString& message, bool portConflict);
    void _startResolvedAgent();
    LaunchSpec _resolveLaunchSpec();
    QString _generateLocalToken() const;
    QUrl _healthUrl() const;
    void _startStartupPolling();
    void _stopStartupPolling();
    void _startRuntimeHealth();
    void _stopRuntimeHealth();
    void _scheduleRestart();
    void _queueRestartAfterExit();
    void _shutdownOwnedProcess();
    void _forceKillIfStillRunning();
    void _clearLocalToken();
    void _setState(State state);
    void _setHealthReady(bool ready);
    void _setOwnsProcess(bool ownsProcess);
    void _setRestartCount(int count);
    void _setLastError(const QString& message);
    void _setProgramPath(const QString& path);
    void _setWorkingDirectory(const QString& path);
    QString _normalizedProvider(const QString& provider) const;
    QString _normalizedOllamaBaseUrl(const QString& baseUrl) const;
    QString _normalizedOllamaModel(const QString& model) const;

    QNetworkAccessManager _networkManager;
    QProcess _process;
    QPointer<QNetworkReply> _healthReply;
    QTimer _startupHealthTimer;
    QTimer _startupTimeoutTimer;
    QTimer _runtimeHealthTimer;
    QTimer _terminateTimer;

    QUrl _endpoint = QUrl(QStringLiteral("http://127.0.0.1:8765"));
    bool _enabled = true;
    bool _autoStart = true;
    State _state = Stopped;
    QString _stateText = QStringLiteral("Agent未启动");
    bool _healthReady = false;
    bool _ownsProcess = false;
    bool _stopping = false;
    bool _restartPending = false;
    int _restartCount = 0;
    int _runtimeFailureCount = 0;
    QString _lastError;
    QString _programPath;
    QString _workingDirectory;
    QString _localToken;
    QString _provider = QStringLiteral("mock");
    QString _ollamaBaseUrl = QStringLiteral("http://127.0.0.1:11434");
    QString _ollamaModel = QStringLiteral("qwen3:8b");
    int _ollamaTimeoutSeconds = 60;
    bool _allowMockFallback = false;

    static constexpr int kHealthTimeoutMs = 2000;
    static constexpr int kStartupPollMs = 500;
    static constexpr int kStartupTimeoutMs = 10000;
    static constexpr int kRuntimeHealthMs = 5000;
    static constexpr int kRuntimeFailureLimit = 3;
    static constexpr int kMaxRestartCount = 2;
    static constexpr int kTerminateTimeoutMs = 3000;
};
