#include "MerivusLinkDiagnostics.h"

#include "LinkConfiguration.h"
#include "LinkInterface.h"
#include "LinkManager.h"
#include "QGCApplication.h"
#include "QGCToolbox.h"
#include "QmlObjectListModel.h"
#include "TCPLink.h"

#include <QLoggingCategory>
#include <QVariantMap>

Q_LOGGING_CATEGORY(MerivusLinkDiagnosticsLog, "merivus.link.diagnostics")

MerivusLinkDiagnostics::MerivusLinkDiagnostics(QObject* parent)
    : QObject(parent)
{
    _summaryText = tr("未知");
    _detailText = tr("Link 诊断尚未初始化");
    _lastStateChange = QDateTime::currentDateTime();

    _refreshTimer.setSingleShot(true);
    _refreshTimer.setInterval(0);
    connect(&_refreshTimer, &QTimer::timeout, this, &MerivusLinkDiagnostics::_refresh);

    _pollTimer.setInterval(1500);
    connect(&_pollTimer, &QTimer::timeout, this, &MerivusLinkDiagnostics::_scheduleRefresh);
    _pollTimer.start();

    _scheduleRefresh();
}

void MerivusLinkDiagnostics::_scheduleRefresh()
{
    if (!_refreshTimer.isActive()) {
        _refreshTimer.start();
    }
}

QmlObjectListModel* MerivusLinkDiagnostics::_linkConfigurations() const
{
    if (!qgcApp() || !qgcApp()->toolbox() || !qgcApp()->toolbox()->linkManager()) {
        return nullptr;
    }

    QVariant value = qgcApp()->toolbox()->linkManager()->property("linkConfigurations");
    if (value.canConvert<QObject*>()) {
        return qobject_cast<QmlObjectListModel*>(value.value<QObject*>());
    }
    return value.value<QmlObjectListModel*>();
}

QString MerivusLinkDiagnostics::_linkTypeName(LinkConfiguration* config) const
{
    if (!config) {
        return tr("未知");
    }

    switch (config->type()) {
    case LinkConfiguration::TypeTcp:
        return QStringLiteral("TCP");
    case LinkConfiguration::TypeUdp:
        return QStringLiteral("UDP");
    case LinkConfiguration::TypeLogReplay:
        return QStringLiteral("LogReplay");
#ifndef NO_SERIAL_LINK
    case LinkConfiguration::TypeSerial:
        return QStringLiteral("Serial");
#endif
#ifdef QGC_ENABLE_BLUETOOTH
    case LinkConfiguration::TypeBluetooth:
        return QStringLiteral("Bluetooth");
#endif
#ifdef QT_DEBUG
    case LinkConfiguration::TypeMock:
        return QStringLiteral("Mock");
#endif
    case LinkConfiguration::TypeLast:
        break;
    }
    return tr("未知");
}

QString MerivusLinkDiagnostics::_safeErrorSummary(const QString& title, const QString& error) const
{
    QString summary = error.trimmed();
    if (summary.isEmpty()) {
        summary = title.trimmed();
    } else if (!title.trimmed().isEmpty()) {
        summary = title.trimmed() + QStringLiteral(": ") + summary;
    }
    return summary.left(240);
}

void MerivusLinkDiagnostics::_observeConfiguration(LinkConfiguration* config)
{
    if (!config || _observedObjects.contains(config)) {
        return;
    }

    _observedObjects.insert(config);
    connect(config, &LinkConfiguration::linkChanged, this, &MerivusLinkDiagnostics::_scheduleRefresh, Qt::UniqueConnection);
    connect(config, &LinkConfiguration::nameChanged, this, &MerivusLinkDiagnostics::_scheduleRefresh, Qt::UniqueConnection);
    connect(config, &QObject::destroyed, this, [this](QObject* object) {
        _observedObjects.remove(object);
        _scheduleRefresh();
    });

    if (config->type() == LinkConfiguration::TypeTcp) {
        if (TCPConfiguration* tcpConfig = qobject_cast<TCPConfiguration*>(config)) {
            connect(tcpConfig, &TCPConfiguration::hostChanged, this, &MerivusLinkDiagnostics::_scheduleRefresh, Qt::UniqueConnection);
            connect(tcpConfig, &TCPConfiguration::portChanged, this, &MerivusLinkDiagnostics::_scheduleRefresh, Qt::UniqueConnection);
            qCInfo(MerivusLinkDiagnosticsLog) << "TCP link configuration discovered" << tcpConfig->name();
        }
    }
}

void MerivusLinkDiagnostics::_observeLink(LinkInterface* link)
{
    if (!link || _observedObjects.contains(link)) {
        return;
    }

    _observedObjects.insert(link);
    connect(link, &LinkInterface::connected, this, &MerivusLinkDiagnostics::_linkConnected, Qt::UniqueConnection);
    connect(link, &LinkInterface::disconnected, this, &MerivusLinkDiagnostics::_linkDisconnected, Qt::UniqueConnection);
    connect(link, &LinkInterface::communicationError, this, &MerivusLinkDiagnostics::_linkCommunicationError, Qt::UniqueConnection);
    connect(link, &QObject::destroyed, this, [this](QObject* object) {
        _observedObjects.remove(object);
        _scheduleRefresh();
    });

    if (link->linkConfiguration() && link->linkConfiguration()->type() == LinkConfiguration::TypeTcp) {
        qCInfo(MerivusLinkDiagnosticsLog) << "TCP link connect attempt observed" << link->linkConfiguration()->name();
    }
}

void MerivusLinkDiagnostics::_linkConnected()
{
    LinkInterface* link = qobject_cast<LinkInterface*>(sender());
    if (link && link->linkConfiguration() && link->linkConfiguration()->type() == LinkConfiguration::TypeTcp) {
        qCInfo(MerivusLinkDiagnosticsLog) << "TCP link connected" << link->linkConfiguration()->name();
    }
    _scheduleRefresh();
}

void MerivusLinkDiagnostics::_linkDisconnected()
{
    LinkInterface* link = qobject_cast<LinkInterface*>(sender());
    if (link && link->linkConfiguration() && link->linkConfiguration()->type() == LinkConfiguration::TypeTcp) {
        qCInfo(MerivusLinkDiagnosticsLog) << "TCP link disconnected" << link->linkConfiguration()->name();
    }
    _scheduleRefresh();
}

void MerivusLinkDiagnostics::_linkCommunicationError(const QString& title, const QString& error)
{
    LinkInterface* link = qobject_cast<LinkInterface*>(sender());
    const QString errorSummary = _safeErrorSummary(title, error);
    if (link && link->linkConfiguration()) {
        _lastErrorsByName[link->linkConfiguration()->name()] = errorSummary;
        if (link->linkConfiguration()->type() == LinkConfiguration::TypeTcp) {
            qCWarning(MerivusLinkDiagnosticsLog) << "TCP link error" << link->linkConfiguration()->name() << errorSummary;
        }
    }
    _scheduleRefresh();
}

void MerivusLinkDiagnostics::_refresh()
{
    QmlObjectListModel* configs = _linkConfigurations();
    if (!configs) {
        _configuredLinkCount = 0;
        _tcpLinkCount = 0;
        _connectedTcpLinkCount = 0;
        _updateState(QStringLiteral("unknown"), tr("未知"), tr("LinkManager 尚不可用"), {});
        return;
    }

    connect(configs, &QmlObjectListModel::countChanged, this, &MerivusLinkDiagnostics::_scheduleRefresh, Qt::UniqueConnection);

    QVariantList details;
    QStringList detailLines;
    int configuredCount = 0;
    int tcpCount = 0;
    int connectedTcpCount = 0;
    int connectingTcpCount = 0;
    bool hasTcpError = false;
    QString firstTcpError;

    for (int i = 0; i < configs->count(); ++i) {
        LinkConfiguration* config = qobject_cast<LinkConfiguration*>(configs->get(i));
        if (!config || config->isDynamic()) {
            continue;
        }

        ++configuredCount;
        _observeConfiguration(config);

        LinkInterface* link = config->link();
        _observeLink(link);

        const bool connected = link && link->isConnected();
        const bool connecting = link && !connected;
        QVariantMap detail;
        detail[QStringLiteral("displayName")] = config->name();
        detail[QStringLiteral("linkType")] = _linkTypeName(config);
        detail[QStringLiteral("connected")] = connected;
        detail[QStringLiteral("connecting")] = connecting ? tr("未知") : tr("否");
        detail[QStringLiteral("associatedVehicleIds")] = tr("未绑定");
        detail[QStringLiteral("lastStateChange")] = _lastStateChange.toString(Qt::ISODate);

        if (config->type() == LinkConfiguration::TypeTcp) {
            ++tcpCount;
            TCPConfiguration* tcpConfig = qobject_cast<TCPConfiguration*>(config);
            const QString host = tcpConfig ? tcpConfig->host() : QString();
            const int port = tcpConfig ? static_cast<int>(tcpConfig->port()) : 0;
            const QString lastError = _lastErrorsByName.value(config->name());
            if (connected) {
                ++connectedTcpCount;
            } else if (connecting) {
                ++connectingTcpCount;
            }
            if (!lastError.isEmpty()) {
                hasTcpError = true;
                if (firstTcpError.isEmpty()) {
                    firstTcpError = lastError;
                }
            }

            detail[QStringLiteral("host")] = host;
            detail[QStringLiteral("port")] = port;
            detail[QStringLiteral("lastError")] = lastError.isEmpty() ? tr("无") : lastError;
            detailLines << tr("%1 TCP %2:%3 - %4%5")
                               .arg(config->name().isEmpty() ? tr("未命名") : config->name())
                               .arg(host.isEmpty() ? tr("未知主机") : host)
                               .arg(port > 0 ? QString::number(port) : tr("未知端口"))
                               .arg(connected ? tr("已连接") : (connecting ? tr("连接中/未知") : tr("未连接")))
                               .arg(lastError.isEmpty() ? QString() : tr("；错误：%1").arg(lastError));
            details << detail;
        }
    }

    _configuredLinkCount = configuredCount;
    _tcpLinkCount = tcpCount;
    _connectedTcpLinkCount = connectedTcpCount;
    if (connectedTcpCount > 0) {
        _everHadConnectedTcpLink = true;
    }

    QString state;
    QString text;
    if (tcpCount == 0) {
        state = QStringLiteral("unconfigured");
        text = tr("未配置");
    } else if (connectedTcpCount > 0) {
        state = QStringLiteral("connected");
        text = tcpCount > 1 ? tr("%1/%2 已连接").arg(connectedTcpCount).arg(tcpCount) : tr("已连接");
    } else if (connectingTcpCount > 0) {
        state = QStringLiteral("connecting");
        text = tr("连接中");
    } else if (hasTcpError) {
        state = QStringLiteral("error");
        text = tr("连接错误");
    } else if (_everHadConnectedTcpLink) {
        state = QStringLiteral("disconnected");
        text = tr("已断开");
    } else {
        state = QStringLiteral("configured_disconnected");
        text = tr("已配置/未连接");
    }

    QString detailText;
    if (detailLines.isEmpty()) {
        detailText = tr("未发现 TCP Link 配置。\n请在原生 QGC 通信设置中添加或连接 TCP Link。");
    } else {
        detailText = detailLines.join(QStringLiteral("\n"));
        if (hasTcpError && !firstTcpError.isEmpty()) {
            detailText += tr("\n最近错误：%1").arg(firstTcpError);
        }
    }

    _updateState(state, text, detailText, details);
}

void MerivusLinkDiagnostics::_updateState(const QString& state, const QString& text, const QString& detailText, const QVariantList& details)
{
    const bool changed = state != _summaryState || text != _summaryText || detailText != _detailText || details != _linkDetails;
    if (!changed) {
        return;
    }

    _summaryState = state;
    _summaryText = text;
    _detailText = detailText;
    _linkDetails = details;
    _lastStateChange = QDateTime::currentDateTime();

    qCInfo(MerivusLinkDiagnosticsLog) << "TCP summary changed" << _summaryState << _summaryText << _lastStateChange.toString(Qt::ISODate);
    emit diagnosticsChanged();
}
