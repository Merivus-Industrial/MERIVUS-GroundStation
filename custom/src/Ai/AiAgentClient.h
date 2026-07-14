#pragma once

#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPointer>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QVariant>

class AiAgentClient : public QObject
{
    Q_OBJECT

public:
    explicit AiAgentClient(QObject* parent = nullptr);

    Q_PROPERTY(bool agentEnabled READ agentEnabled WRITE setAgentEnabled NOTIFY agentEnabledChanged)
    Q_PROPERTY(bool agentOnline READ agentOnline NOTIFY agentOnlineChanged)
    Q_PROPERTY(bool requestInProgress READ requestInProgress NOTIFY requestInProgressChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)
    Q_PROPERTY(QString provider READ provider NOTIFY infoChanged)
    Q_PROPERTY(QString model READ model NOTIFY infoChanged)
    Q_PROPERTY(bool providerReady READ providerReady NOTIFY infoChanged)
    Q_PROPERTY(QString providerError READ providerError NOTIFY infoChanged)
    Q_PROPERTY(QString availableModelsText READ availableModelsText NOTIFY infoChanged)
    Q_PROPERTY(QString serviceVersion READ serviceVersion NOTIFY infoChanged)
    Q_PROPERTY(QString endpoint READ endpoint WRITE setEndpoint NOTIFY endpointChanged)

    bool agentEnabled() const { return _agentEnabled; }
    bool agentOnline() const { return _agentOnline; }
    bool requestInProgress() const { return !_currentChatReply.isNull(); }
    QString statusText() const { return _statusText; }
    QString lastError() const { return _lastError; }
    QString provider() const { return _provider; }
    QString model() const { return _model; }
    bool providerReady() const { return _providerReady; }
    QString providerError() const { return _providerError; }
    QString availableModelsText() const { return _availableModels.join(QStringLiteral(", ")); }
    QString serviceVersion() const { return _serviceVersion; }
    QString endpoint() const { return _endpoint.toString(QUrl::RemovePath | QUrl::RemoveQuery | QUrl::RemoveFragment); }

    void setAgentEnabled(bool enabled);
    void setEndpoint(const QString& endpoint);

    Q_INVOKABLE void setLocalToken(const QString& token);
    Q_INVOKABLE void clearLocalToken();
    Q_INVOKABLE void checkHealth();
    Q_INVOKABLE void loadInfo();
    Q_INVOKABLE QString sendMessage(const QString& message,
                                    const QVariantMap& context = QVariantMap(),
                                    const QVariantList& allowedCapabilities = QVariantList());
    Q_INVOKABLE void cancelCurrentRequest();

signals:
    void agentEnabledChanged();
    void agentOnlineChanged();
    void requestInProgressChanged();
    void statusTextChanged();
    void lastErrorChanged();
    void endpointChanged();

    void responseReceived(const QString& requestId, const QString& reply, const QVariant& proposal);
    void requestFailed(const QString& requestId, const QString& errorCode, const QString& message);
    void healthChanged();
    void infoChanged();

private:
    enum class RequestKind {
        Health,
        Info,
        Chat,
    };

    QNetworkRequest _jsonRequest(const QString& path, bool includeLocalToken = false) const;
    QUrl _urlForPath(const QString& path) const;
    void _attachTimeout(QNetworkReply* reply, int timeoutMs);
    void _handleReplyFinished(QNetworkReply* reply, RequestKind kind, const QString& requestId);
    void _handleHealthReply(QNetworkReply* reply, const QByteArray& body, int httpStatus);
    void _handleInfoReply(QNetworkReply* reply, const QByteArray& body, int httpStatus);
    void _handleChatReply(QNetworkReply* reply, const QByteArray& body, int httpStatus, const QString& requestId);
    bool _checkHttpSuccess(QNetworkReply* reply,
                           const QByteArray& body,
                           int httpStatus,
                           const QString& requestId,
                           bool emitFailure);
    void _failRequest(const QString& requestId, const QString& errorCode, const QString& message, bool setOffline);
    void _setAgentOnline(bool online);
    void _setRequestInProgress(bool inProgress);
    void _setStatusText(const QString& text);
    void _setLastError(const QString& text);
    void _setInfo(const QString& provider,
                  const QString& model,
                  const QString& serviceVersion,
                  bool providerReady,
                  const QString& providerError,
                  const QStringList& availableModels);
    QJsonObject _safeContext(const QVariantMap& context) const;
    QJsonArray _capabilitiesArray(const QVariantList& allowedCapabilities) const;

    QNetworkAccessManager _networkManager;
    QUrl _endpoint;
    QString _sessionId;
    QPointer<QNetworkReply> _currentChatReply;

    bool _agentEnabled = true;
    bool _agentOnline = false;
    QString _localToken;
    QString _statusText = QStringLiteral("Agent未启动");
    QString _lastError;
    QString _provider = QStringLiteral("mock");
    QString _model = QStringLiteral("mock-v1");
    bool _providerReady = false;
    QString _providerError;
    QStringList _availableModels;
    QString _serviceVersion;

    static constexpr int kHealthTimeoutMs = 2000;
    static constexpr int kInfoTimeoutMs = 3000;
    static constexpr int kChatTimeoutMs = 60000;
};
