#pragma once

#include <QObject>
#include <QGeoCoordinate>
#include <QVariantList>
#include <QVariantMap>

#include "MAVLinkProtocol.h"

class Vehicle;
class MissionItem;
class LinkInterface;

class SwarmController : public QObject
{
    Q_OBJECT

public:
    explicit SwarmController(QObject* parent = nullptr);

    Q_INVOKABLE QVariantMap executeGoto(const QVariantList& selectedVehicleIds, const QVariant& targetCoordinate);
    Q_INVOKABLE QVariantMap executeQueuedGoto(const QVariantList& selectedVehicleIds, const QVariantList& queuedCoordinates);
    Q_INVOKABLE QVariantMap sendStartCommand();

private slots:
    void _receiveMessage(LinkInterface* link, mavlink_message_t message);

private:
    QList<Vehicle*> _selectedVehicles(const QVariantList& selectedVehicleIds) const;
    QVariantMap _executeGotoInternal(const QVariantList& selectedVehicleIds, const QList<QGeoCoordinate>& coordinates, bool queued);
    QVariantMap _result(bool ok, const QString& message, const QList<int>& dispatchedIds = QList<int>(), const QList<int>& skippedIds = QList<int>()) const;
    QGeoCoordinate _coordinateFromVariant(const QVariant& value) const;
    bool _vehicleReady(Vehicle* vehicle) const;
    void _dispatchGoto(Vehicle* vehicle, const QGeoCoordinate& coordinate, int delayMs);
    void _dispatchTemporaryMission(Vehicle* vehicle, const QList<QGeoCoordinate>& coordinates, int delayMs);
    QList<MissionItem*> _buildTemporaryMissionItems(Vehicle* vehicle, const QList<QGeoCoordinate>& coordinates) const;
    double _relativeAltitudeMeters(Vehicle* vehicle, double fallbackMeters) const;
    void _ensureLegacyForwardingConnected();
    bool _sendLegacyStartPacket();

    bool _legacyForwardingConnected = false;
    bool _legacyForwardingEnabled = false;
    static const double kMinimumGuidedAltitudeMeters;
    static const double kDefaultMissionAltitudeMeters;
};
