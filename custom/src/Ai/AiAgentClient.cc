#include "AiAgentClient.h"

#include "AiAuditEvent.h"
#include "AiCommandPolicy.h"
#include "AiSchemaValidator.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QTimer>
#include <QUuid>

namespace {
const char* kTimedOutProperty = "merivusTimedOut";
const char* kCanceledProperty = "merivusCanceled";
}

AiAgentClient::AiAgentClient(QObject* parent)
    : QObject(parent)
    , _endpoint(QStringLiteral("http://127.0.0.1:8765"))
    , _sessionId(QUuid::createUuid().toString(QUuid::WithoutBraces))
{
}

void AiAgentClient::setAgentEnabled(bool enabled)
{
    if (_agentEnabled == enabled) {
        return;
    }

    _agentEnabled = enabled;
    emit agentEnabledChanged();

    if (!enabled) {
        cancelCurrentRequest();
        _setAgentOnline(false);
        _setStatusText(QStringLiteral("Agent已禁用"));
    } else {
        _setStatusText(QStringLiteral("Agent未启动"));
        checkHealth();
    }
}

void AiAgentClient::setEndpoint(const QString& endpoint)
{
    QUrl requested(endpoint.trimmed());
    if (!requested.isValid() || requested.scheme() != QStringLiteral("http") || requested.host() != QStringLiteral("127.0.0.1")) {
        _setLastError(QStringLiteral("Agent地址被拒绝：仅允许 http://127.0.0.1:8765"));
        return;
    }

    QUrl normalized;
    normalized.setScheme(QStringLiteral("http"));
    normalized.setHost(QStringLiteral("127.0.0.1"));
    normalized.setPort(requested.port(8765));

    if (_endpoint == normalized) {
        return;
    }

    _endpoint = normalized;
    emit endpointChanged();
}

void AiAgentClient::setLocalToken(const QString& token)
{
    _localToken = token.trimmed();
}

void AiAgentClient::clearLocalToken()
{
    _localToken.clear();
}

void AiAgentClient::checkHealth()
{
    if (!_agentEnabled) {
        _setAgentOnline(false);
        _setStatusText(QStringLiteral("Agent已禁用"));
        return;
    }

    _setStatusText(QStringLiteral("正在连接"));
    QNetworkReply* reply = _networkManager.get(_jsonRequest(QStringLiteral("/health")));
    _attachTimeout(reply, kHealthTimeoutMs);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        _handleReplyFinished(reply, RequestKind::Health, QString());
    });
}

void AiAgentClient::loadInfo()
{
    if (!_agentEnabled) {
        return;
    }

    QNetworkReply* reply = _networkManager.get(_jsonRequest(QStringLiteral("/merivus/info")));
    _attachTimeout(reply, kInfoTimeoutMs);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        _handleReplyFinished(reply, RequestKind::Info, QString());
    });
}

QString AiAgentClient::sendMessage(const QString& message, const QVariantMap& context, const QVariantList& allowedCapabilities)
{
    const QString requestId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    const QString trimmedMessage = message.trimmed();

    if (!_agentEnabled) {
        _failRequest(requestId, QStringLiteral("agent_disabled"), QStringLiteral("Agent已禁用。"), false);
        return requestId;
    }

    if (requestInProgress()) {
        _failRequest(requestId, QStringLiteral("request_in_progress"), QStringLiteral("已有Agent请求正在处理。"), false);
        return requestId;
    }

    if (trimmedMessage.isEmpty()) {
        _failRequest(requestId, QStringLiteral("empty_message"), QStringLiteral("消息不能为空。"), false);
        return requestId;
    }

    QJsonObject payload;
    payload.insert(QStringLiteral("request_id"), requestId);
    payload.insert(QStringLiteral("session_id"), _sessionId);
    payload.insert(QStringLiteral("message"), trimmedMessage);
    payload.insert(QStringLiteral("context"), _safeContext(context));
    payload.insert(QStringLiteral("allowed_capabilities"), _capabilitiesArray(allowedCapabilities));

    QNetworkReply* reply = _networkManager.post(
        _jsonRequest(QStringLiteral("/merivus/agent"), true),
        QJsonDocument(payload).toJson(QJsonDocument::Compact));

    _currentChatReply = reply;
    _setRequestInProgress(true);
    _setStatusText(QStringLiteral("请求中"));
    _attachTimeout(reply, kChatTimeoutMs);

    connect(reply, &QNetworkReply::finished, this, [this, reply, requestId]() {
        _handleReplyFinished(reply, RequestKind::Chat, requestId);
    });

    return requestId;
}

void AiAgentClient::cancelCurrentRequest()
{
    if (_currentChatReply) {
        _currentChatReply->setProperty(kCanceledProperty, true);
        _currentChatReply->abort();
    }
}

QNetworkRequest AiAgentClient::_jsonRequest(const QString& path, bool includeLocalToken) const
{
    QNetworkRequest request(_urlForPath(path));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json; charset=utf-8"));
    request.setRawHeader("Accept", "application/json");
    if (includeLocalToken && !_localToken.isEmpty()) {
        request.setRawHeader("X-Merivus-Token", _localToken.toUtf8());
    }
    return request;
}

QUrl AiAgentClient::_urlForPath(const QString& path) const
{
    QUrl url = _endpoint;
    url.setPath(path);
    return url;
}

void AiAgentClient::_attachTimeout(QNetworkReply* reply, int timeoutMs)
{
    QTimer* timer = new QTimer(reply);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, [reply]() {
        if (reply && reply->isRunning()) {
            reply->setProperty(kTimedOutProperty, true);
            reply->abort();
        }
    });
    timer->start(timeoutMs);
}

void AiAgentClient::_handleReplyFinished(QNetworkReply* reply, RequestKind kind, const QString& requestId)
{
    if (!reply) {
        return;
    }

    const QByteArray body = reply->readAll();
    const QVariant statusAttribute = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    const int httpStatus = statusAttribute.isValid() ? statusAttribute.toInt() : 0;

    switch (kind) {
    case RequestKind::Health:
        _handleHealthReply(reply, body, httpStatus);
        break;
    case RequestKind::Info:
        _handleInfoReply(reply, body, httpStatus);
        break;
    case RequestKind::Chat:
        _handleChatReply(reply, body, httpStatus, requestId);
        _currentChatReply.clear();
        _setRequestInProgress(false);
        break;
    }

    reply->deleteLater();
}

void AiAgentClient::_handleHealthReply(QNetworkReply* reply, const QByteArray& body, int httpStatus)
{
    if (!_checkHttpSuccess(reply, body, httpStatus, QString(), false)) {
        _setAgentOnline(false);
        _setStatusText(QStringLiteral("Agent未启动"));
        return;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(body, &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        _setAgentOnline(false);
        _setLastError(QStringLiteral("Agent health响应不是有效JSON对象。"));
        _setStatusText(QStringLiteral("请求失败"));
        return;
    }

    const QJsonObject object = document.object();
    if (object.value(QStringLiteral("status")).toString() != QStringLiteral("ok")) {
        _setAgentOnline(false);
        _setLastError(QStringLiteral("Agent health状态异常。"));
        _setStatusText(QStringLiteral("请求失败"));
        return;
    }

    _setAgentOnline(true);
    _setLastError(QString());
    _setStatusText(QStringLiteral("已连接"));
    loadInfo();
}

void AiAgentClient::_handleInfoReply(QNetworkReply* reply, const QByteArray& body, int httpStatus)
{
    if (!_checkHttpSuccess(reply, body, httpStatus, QString(), false)) {
        return;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(body, &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        _setLastError(QStringLiteral("Agent info响应不是有效JSON对象。"));
        return;
    }

    const QJsonObject object = document.object();
    const QString provider = object.value(QStringLiteral("provider")).toString();
    const QString model = object.value(QStringLiteral("model")).toString();
    const QString version = object.value(QStringLiteral("version")).toString();
    const bool providerReady = object.value(QStringLiteral("provider_ready")).toBool(false);
    const QString providerError = object.value(QStringLiteral("provider_error")).toString();
    QStringList availableModels;
    const QJsonArray models = object.value(QStringLiteral("available_models")).toArray();
    for (const QJsonValue& value : models) {
        const QString modelName = value.toString().trimmed();
        if (!modelName.isEmpty()) {
            availableModels.append(modelName);
        }
    }

    if (!provider.isEmpty() && !model.isEmpty()) {
        _setInfo(provider, model, version, providerReady, providerError, availableModels);
        if (!providerReady && !providerError.isEmpty()) {
            _setLastError(providerError);
        }
    }
}

void AiAgentClient::_handleChatReply(QNetworkReply* reply, const QByteArray& body, int httpStatus, const QString& requestId)
{
    if (!_checkHttpSuccess(reply, body, httpStatus, requestId, true)) {
        return;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(body, &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        _failRequest(requestId, QStringLiteral("invalid_json"), QStringLiteral("Agent响应不是有效JSON对象。"), false);
        return;
    }

    const QJsonObject object = document.object();
    const QString responseRequestId = object.value(QStringLiteral("request_id")).toString();
    if (responseRequestId != requestId) {
        _failRequest(requestId, QStringLiteral("request_id_mismatch"), QStringLiteral("Agent响应request_id不匹配，已安全拒绝。"), false);
        return;
    }

    const QJsonValue replyValue = object.value(QStringLiteral("reply"));
    if (!replyValue.isString()) {
        _failRequest(requestId, QStringLiteral("invalid_reply"), QStringLiteral("Agent响应缺少字符串reply字段。"), false);
        return;
    }

    if (object.value(QStringLiteral("status")).toString() != QStringLiteral("ok")) {
        _failRequest(requestId, QStringLiteral("invalid_status"), QStringLiteral("Agent响应状态不是ok。"), false);
        return;
    }

    const QString responseProvider = object.value(QStringLiteral("provider")).toString();
    const QString responseModel = object.value(QStringLiteral("model")).toString();
    if (!responseProvider.isEmpty() && !responseModel.isEmpty()) {
        _setInfo(responseProvider, responseModel, _serviceVersion, _providerReady, _providerError, _availableModels);
    }

    const QJsonValue proposalValue = object.value(QStringLiteral("proposal"));
    QVariant proposal;
    if (proposalValue.isObject()) {
        ActionProposal evaluatedProposal = AiSchemaValidator::validate(
            requestId,
            proposalValue,
            responseProvider.isEmpty() ? _provider : responseProvider,
            responseModel.isEmpty() ? _model : responseModel);
        evaluatedProposal = AiCommandPolicy::evaluate(evaluatedProposal);
        AiAuditEvent::recordProposal(evaluatedProposal, _sessionId);
        proposal = evaluatedProposal.toVariantMap();
    } else if (!proposalValue.isNull() && !proposalValue.isUndefined()) {
        _failRequest(requestId, QStringLiteral("invalid_proposal"), QStringLiteral("Agent proposal必须为对象或null。"), false);
        return;
    }

    _setAgentOnline(true);
    _setLastError(QString());
    _setStatusText(QStringLiteral("已连接"));
    emit responseReceived(requestId, replyValue.toString(), proposal);
}

bool AiAgentClient::_checkHttpSuccess(QNetworkReply* reply,
                                      const QByteArray& body,
                                      int httpStatus,
                                      const QString& requestId,
                                      bool emitFailure)
{
    if (reply->property(kTimedOutProperty).toBool()) {
        if (emitFailure) {
            _failRequest(requestId, QStringLiteral("timeout"), QStringLiteral("Agent请求超时。"), true);
        } else {
            _setLastError(QStringLiteral("Agent请求超时。"));
        }
        return false;
    }

    if (reply->property(kCanceledProperty).toBool()) {
        if (emitFailure) {
            _failRequest(requestId, QStringLiteral("canceled"), QStringLiteral("Agent请求已取消。"), false);
        }
        return false;
    }

    if (reply->error() != QNetworkReply::NoError) {
        const QString message = reply->errorString().isEmpty() ? QStringLiteral("无法连接Agent。") : reply->errorString();
        if (emitFailure) {
            _failRequest(requestId, QStringLiteral("network_error"), message, true);
        } else {
            _setLastError(message);
        }
        return false;
    }

    if (httpStatus < 200 || httpStatus >= 300) {
        QString message = QStringLiteral("Agent返回HTTP %1。").arg(httpStatus);
        QJsonParseError parseError;
        const QJsonDocument document = QJsonDocument::fromJson(body, &parseError);
        if (parseError.error == QJsonParseError::NoError && document.isObject()) {
            const QString serverMessage = document.object().value(QStringLiteral("message")).toString();
            if (!serverMessage.isEmpty()) {
                message = serverMessage;
            }
        }

        if (emitFailure) {
            _failRequest(requestId, QStringLiteral("http_error"), message, httpStatus == 0);
        } else {
            _setLastError(message);
        }
        return false;
    }

    return true;
}

void AiAgentClient::_failRequest(const QString& requestId, const QString& errorCode, const QString& message, bool setOffline)
{
    if (setOffline) {
        _setAgentOnline(false);
    }
    _setLastError(message);
    _setStatusText(errorCode == QStringLiteral("timeout") ? QStringLiteral("请求超时") : QStringLiteral("请求失败"));
    emit requestFailed(requestId, errorCode, message);
}

void AiAgentClient::_setAgentOnline(bool online)
{
    if (_agentOnline == online) {
        emit healthChanged();
        return;
    }

    _agentOnline = online;
    emit agentOnlineChanged();
    emit healthChanged();
}

void AiAgentClient::_setRequestInProgress(bool inProgress)
{
    Q_UNUSED(inProgress)
    emit requestInProgressChanged();
}

void AiAgentClient::_setStatusText(const QString& text)
{
    if (_statusText == text) {
        return;
    }

    _statusText = text;
    emit statusTextChanged();
}

void AiAgentClient::_setLastError(const QString& text)
{
    if (_lastError == text) {
        return;
    }

    _lastError = text;
    emit lastErrorChanged();
}

void AiAgentClient::_setInfo(const QString& provider,
                             const QString& model,
                             const QString& serviceVersion,
                             bool providerReady,
                             const QString& providerError,
                             const QStringList& availableModels)
{
    if (_provider == provider &&
        _model == model &&
        _serviceVersion == serviceVersion &&
        _providerReady == providerReady &&
        _providerError == providerError &&
        _availableModels == availableModels) {
        return;
    }

    _provider = provider;
    _model = model;
    _serviceVersion = serviceVersion;
    _providerReady = providerReady;
    _providerError = providerError;
    _availableModels = availableModels;
    emit infoChanged();
}

QJsonObject AiAgentClient::_safeContext(const QVariantMap& context) const
{
    QJsonObject object = QJsonObject::fromVariantMap(context);

    if (!object.contains(QStringLiteral("vehicle_count"))) {
        object.insert(QStringLiteral("vehicle_count"), 0);
    }
    if (!object.contains(QStringLiteral("active_vehicle_id"))) {
        object.insert(QStringLiteral("active_vehicle_id"), QJsonValue::Null);
    }
    if (!object.contains(QStringLiteral("connected"))) {
        object.insert(QStringLiteral("connected"), false);
    }
    if (!object.contains(QStringLiteral("armed"))) {
        object.insert(QStringLiteral("armed"), false);
    }

    return object;
}

QJsonArray AiAgentClient::_capabilitiesArray(const QVariantList& allowedCapabilities) const
{
    QJsonArray array;
    for (const QVariant& capability : allowedCapabilities) {
        const QString value = capability.toString().trimmed();
        if (!value.isEmpty()) {
            array.append(value);
        }
    }
    return array;
}
