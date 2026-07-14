#pragma once

#include <QObject>
#include <QDateTime>
#include <QLoggingCategory>
#include <QMap>
#include <QSet>
#include <QTimer>
#include <QVariantList>

class LinkConfiguration;
class LinkInterface;
class QmlObjectListModel;

Q_DECLARE_LOGGING_CATEGORY(MerivusLinkDiagnosticsLog)

class MerivusLinkDiagnostics : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int configuredLinkCount READ configuredLinkCount NOTIFY diagnosticsChanged)
    Q_PROPERTY(int tcpLinkCount READ tcpLinkCount NOTIFY diagnosticsChanged)
    Q_PROPERTY(int connectedTcpLinkCount READ connectedTcpLinkCount NOTIFY diagnosticsChanged)
    Q_PROPERTY(bool hasConfiguredTcpLink READ hasConfiguredTcpLink NOTIFY diagnosticsChanged)
    Q_PROPERTY(bool hasConnectedTcpLink READ hasConnectedTcpLink NOTIFY diagnosticsChanged)
    Q_PROPERTY(QString summaryState READ summaryState NOTIFY diagnosticsChanged)
    Q_PROPERTY(QString summaryText READ summaryText NOTIFY diagnosticsChanged)
    Q_PROPERTY(QString lastStateChange READ lastStateChange NOTIFY diagnosticsChanged)
    Q_PROPERTY(QString detailText READ detailText NOTIFY diagnosticsChanged)
    Q_PROPERTY(QVariantList linkDetails READ linkDetails NOTIFY diagnosticsChanged)

public:
    explicit MerivusLinkDiagnostics(QObject* parent = nullptr);

    int configuredLinkCount() const { return _configuredLinkCount; }
    int tcpLinkCount() const { return _tcpLinkCount; }
    int connectedTcpLinkCount() const { return _connectedTcpLinkCount; }
    bool hasConfiguredTcpLink() const { return _tcpLinkCount > 0; }
    bool hasConnectedTcpLink() const { return _connectedTcpLinkCount > 0; }
    QString summaryState() const { return _summaryState; }
    QString summaryText() const { return _summaryText; }
    QString lastStateChange() const { return _lastStateChange.toString(Qt::ISODate); }
    QString detailText() const { return _detailText; }
    QVariantList linkDetails() const { return _linkDetails; }

signals:
    void diagnosticsChanged();

private slots:
    void _scheduleRefresh();
    void _refresh();
    void _linkConnected();
    void _linkDisconnected();
    void _linkCommunicationError(const QString& title, const QString& error);

private:
    QString _linkTypeName(LinkConfiguration* config) const;
    QString _safeErrorSummary(const QString& title, const QString& error) const;
    void _observeConfiguration(LinkConfiguration* config);
    void _observeLink(LinkInterface* link);
    void _updateState(const QString& state, const QString& text, const QString& detailText, const QVariantList& details);
    QmlObjectListModel* _linkConfigurations() const;

    int _configuredLinkCount = 0;
    int _tcpLinkCount = 0;
    int _connectedTcpLinkCount = 0;
    bool _everHadConnectedTcpLink = false;
    QString _summaryState = QStringLiteral("unknown");
    QString _summaryText;
    QString _detailText;
    QDateTime _lastStateChange;
    QVariantList _linkDetails;
    QMap<QString, QString> _lastErrorsByName;
    QSet<QObject*> _observedObjects;
    QTimer _refreshTimer;
    QTimer _pollTimer;
};
