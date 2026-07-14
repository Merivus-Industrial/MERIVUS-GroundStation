#include "SwarmController.h"

#include <QPointer>
#include <QSet>
#include <QTimer>
#include <QtAlgorithms>

#include <cmath>

#include "Fact.h"
#include "LinkInterface.h"
#include "MissionItem.h"
#include "MissionManager.h"
#include "MultiVehicleManager.h"
#include "QGCApplication.h"
#include "QGCToolbox.h"
#include "QmlObjectListModel.h"
#include "Vehicle.h"
#include "VehicleLinkManager.h"

const double SwarmController::kMinimumGuidedAltitudeMeters = 5.0;
const double SwarmController::kDefaultMissionAltitudeMeters = 20.0;

SwarmController::SwarmController(QObject* parent)
    : QObject(parent)
{
}

QVariantMap SwarmController::executeGoto(const QVariantList& selectedVehicleIds, const QVariant& targetCoordinate)
{
    const QGeoCoordinate coordinate = _coordinateFromVariant(targetCoordinate);
    if (!coordinate.isValid()) {
        return _result(false, tr("Invalid target coordinate."));
    }
    return _executeGotoInternal(selectedVehicleIds, QList<QGeoCoordinate>() << coordinate, false);
}

QVariantMap SwarmController::executeQueuedGoto(const QVariantList& selectedVehicleIds, const QVariantList& queuedCoordinates)
{
    QList<QGeoCoordinate> coordinates;
    for (const QVariant& value : queuedCoordinates) {
        const QGeoCoordinate coordinate = _coordinateFromVariant(value);
        if (coordinate.isValid()) {
            coordinates << coordinate;
        }
    }

    if (coordinates.isEmpty()) {
        return _result(false, tr("No valid queued coordinates."));
    }

    return _executeGotoInternal(selectedVehicleIds, coordinates, true);
}

QVariantMap SwarmController::sendStartCommand()
{
    if (!_legacyForwardingFeatureEnabled()) {
        return _result(false, tr("Legacy swarm forwarding is disabled by safety containment."));
    }

    _legacyForwardingEnabled = true;
    _ensureLegacyForwardingConnected();

    if (!_sendLegacyStartPacket()) {
        return _result(false, tr("No vehicle with id 1 has an active primary link."));
    }

    return _result(true, tr("Legacy swarm start command sent."));
}

QVariantMap SwarmController::_executeGotoInternal(const QVariantList& selectedVehicleIds, const QList<QGeoCoordinate>& coordinates, bool queued)
{
    if (coordinates.isEmpty()) {
        return _result(false, tr("No target coordinate."));
    }

    const QGeoCoordinate finalTarget = coordinates.last();
    QList<Vehicle*> vehicles = _selectedVehicles(selectedVehicleIds);
    if (vehicles.isEmpty()) {
        return _result(false, tr("No vehicle available."));
    }

    QList<int> dispatchedIds;
    QList<int> skippedIds;

    if (!selectedVehicleIds.isEmpty()) {
        QSet<int> uniqueIds;
        for (const QVariant& value : selectedVehicleIds) {
            const int id = value.toInt();
            if (uniqueIds.contains(id)) {
                return _result(false, tr("Duplicate vehicle IDs in selection."));
            }
            uniqueIds.insert(id);
        }
    }

    if (selectedVehicleIds.isEmpty()) {
        Vehicle* vehicle = vehicles.first();
        if (!_vehicleReady(vehicle)) {
            skippedIds << vehicle->id();
            return _result(false, tr("Vehicle is not armed or is below the minimum safe altitude."), dispatchedIds, skippedIds);
        }

        if (queued) {
            if (vehicle->missionManager() && vehicle->missionManager()->inProgress()) {
                skippedIds << vehicle->id();
                return _result(false, tr("Vehicle mission upload is already in progress."), dispatchedIds, skippedIds);
            }
            _dispatchTemporaryMission(vehicle, coordinates, 0);
            dispatchedIds << vehicle->id();
            return _result(true, tr("Temporary mission route upload started."), dispatchedIds, skippedIds);
        }

        _dispatchGoto(vehicle, finalTarget, 0);
        dispatchedIds << vehicle->id();
        return _result(true, tr("Goto command dispatched."), dispatchedIds, skippedIds);
    }

    double centerLat = 0.0;
    double centerLon = 0.0;
    int validCount = 0;
    for (Vehicle* vehicle : vehicles) {
        if (vehicle && vehicle->coordinate().isValid()) {
            centerLat += vehicle->coordinate().latitude();
            centerLon += vehicle->coordinate().longitude();
            ++validCount;
        }
    }

    if (validCount == 0) {
        return _result(false, tr("Selected vehicles do not have valid positions."));
    }

    const QGeoCoordinate centroid(centerLat / validCount, centerLon / validCount);

    int delayMs = 50;
    for (Vehicle* vehicle : vehicles) {
        if (!_vehicleReady(vehicle)) {
            if (vehicle) {
                skippedIds << vehicle->id();
            }
            continue;
        }

        if (queued) {
            if (vehicle->missionManager() && vehicle->missionManager()->inProgress()) {
                skippedIds << vehicle->id();
                continue;
            }
            QList<QGeoCoordinate> vehicleRoute;
            for (const QGeoCoordinate& coordinate : coordinates) {
                const double moveDistance = centroid.distanceTo(coordinate);
                const double moveAzimuth = centroid.azimuthTo(coordinate);
                QGeoCoordinate vehicleCoordinate = vehicle->coordinate().atDistanceAndAzimuth(moveDistance, moveAzimuth);
                vehicleCoordinate.setAltitude(coordinate.altitude());
                vehicleRoute << vehicleCoordinate;
            }
            _dispatchTemporaryMission(vehicle, vehicleRoute, delayMs);
        } else {
            const double moveDistance = centroid.distanceTo(finalTarget);
            const double moveAzimuth = centroid.azimuthTo(finalTarget);
            const QGeoCoordinate vehicleTarget = vehicle->coordinate().atDistanceAndAzimuth(moveDistance, moveAzimuth);
            _dispatchGoto(vehicle, vehicleTarget, delayMs);
        }

        dispatchedIds << vehicle->id();
        delayMs += 200;
    }

    const bool ok = !dispatchedIds.isEmpty();
    return _result(ok, ok ? (queued ? tr("Temporary swarm mission upload started.") : tr("Swarm goto command dispatched.")) : tr("No selected vehicle met the safety requirements."), dispatchedIds, skippedIds);
}

QList<Vehicle*> SwarmController::_selectedVehicles(const QVariantList& selectedVehicleIds) const
{
    QList<Vehicle*> result;
    MultiVehicleManager* manager = qgcApp()->toolbox()->multiVehicleManager();
    if (!manager) {
        return result;
    }

    if (selectedVehicleIds.isEmpty()) {
        if (manager->activeVehicle()) {
            result << manager->activeVehicle();
        }
        return result;
    }

    QSet<int> selectedIds;
    for (const QVariant& value : selectedVehicleIds) {
        selectedIds.insert(value.toInt());
    }

    QmlObjectListModel* model = manager->vehicles();
    if (!model) {
        return result;
    }

    const int count = model->objectList()->count();
    for (int i = 0; i < count; ++i) {
        Vehicle* vehicle = qobject_cast<Vehicle*>(model->get(i));
        if (vehicle && selectedIds.contains(vehicle->id())) {
            result << vehicle;
        }
    }

    return result;
}

QVariantMap SwarmController::_result(bool ok, const QString& message, const QList<int>& dispatchedIds, const QList<int>& skippedIds) const
{
    QVariantList dispatched;
    QVariantList skipped;
    for (int id : dispatchedIds) {
        dispatched << id;
    }
    for (int id : skippedIds) {
        skipped << id;
    }

    QVariantMap map;
    map.insert(QStringLiteral("ok"), ok);
    map.insert(QStringLiteral("message"), message);
    map.insert(QStringLiteral("dispatchedIds"), dispatched);
    map.insert(QStringLiteral("skippedIds"), skipped);
    return map;
}

QGeoCoordinate SwarmController::_coordinateFromVariant(const QVariant& value) const
{
    if (value.canConvert<QGeoCoordinate>()) {
        return value.value<QGeoCoordinate>();
    }
    return QGeoCoordinate();
}

bool SwarmController::_vehicleReady(Vehicle* vehicle) const
{
    if (!vehicle || !vehicle->armed() || !vehicle->altitudeRelative()) {
        return false;
    }

    bool ok = false;
    const double altitude = vehicle->altitudeRelative()->rawValue().toDouble(&ok);
    return ok && altitude >= kMinimumGuidedAltitudeMeters;
}

void SwarmController::_dispatchGoto(Vehicle* vehicle, const QGeoCoordinate& coordinate, int delayMs)
{
    QPointer<Vehicle> guardedVehicle(vehicle);
    QTimer::singleShot(delayMs, this, [guardedVehicle, coordinate]() {
        if (!guardedVehicle) {
            return;
        }

        if (guardedVehicle->flightMode() != guardedVehicle->gotoFlightMode()) {
            guardedVehicle->setFlightMode(guardedVehicle->gotoFlightMode());
            QTimer::singleShot(300, guardedVehicle, [guardedVehicle, coordinate]() {
                if (guardedVehicle) {
                    guardedVehicle->guidedModeGotoLocation(coordinate);
                }
            });
        } else {
            guardedVehicle->guidedModeGotoLocation(coordinate);
        }
    });
}



void SwarmController::_dispatchTemporaryMission(Vehicle* vehicle, const QList<QGeoCoordinate>& coordinates, int delayMs)
{
    QPointer<Vehicle> guardedVehicle(vehicle);
    QTimer::singleShot(delayMs, this, [this, guardedVehicle, coordinates]() {
        if (!guardedVehicle || !guardedVehicle->missionManager() || guardedVehicle->missionManager()->inProgress()) {
            return;
        }

        QList<MissionItem*> missionItems = _buildTemporaryMissionItems(guardedVehicle, coordinates);
        if (missionItems.count() < 2) {
            qDeleteAll(missionItems);
            return;
        }

        MissionManager* missionManager = guardedVehicle->missionManager();
        QPointer<MissionManager> guardedMissionManager(missionManager);
        connect(missionManager, &MissionManager::sendComplete, this, [this, guardedVehicle, guardedMissionManager](bool error) {
            if (guardedMissionManager) {
                disconnect(guardedMissionManager.data(), nullptr, this, nullptr);
            }
            if (!error && guardedVehicle && guardedMissionManager && _autoStartMissionFeatureEnabled()) {
                guardedVehicle->startMission();
            }
        });

        missionManager->writeMissionItems(missionItems);
    });
}

QList<MissionItem*> SwarmController::_buildTemporaryMissionItems(Vehicle* vehicle, const QList<QGeoCoordinate>& coordinates) const
{
    QList<MissionItem*> missionItems;
    if (!vehicle || coordinates.isEmpty()) {
        return missionItems;
    }

    QGeoCoordinate homeCoordinate = vehicle->homePosition();
    if (!homeCoordinate.isValid()) {
        homeCoordinate = vehicle->coordinate();
    }
    if (!homeCoordinate.isValid()) {
        return missionItems;
    }

    const double homeAltitude = std::isfinite(homeCoordinate.altitude()) ? homeCoordinate.altitude() : 0.0;
    int sequenceNumber = 0;
    missionItems << new MissionItem(sequenceNumber++,
                                    MAV_CMD_NAV_WAYPOINT,
                                    MAV_FRAME_GLOBAL,
                                    0, 0, 0, 0,
                                    homeCoordinate.latitude(),
                                    homeCoordinate.longitude(),
                                    homeAltitude,
                                    true,
                                    false);

    const double fallbackAltitude = _relativeAltitudeMeters(vehicle, kDefaultMissionAltitudeMeters);
    for (const QGeoCoordinate& coordinate : coordinates) {
        if (!coordinate.isValid()) {
            continue;
        }

        const double altitude = std::isfinite(coordinate.altitude()) ? coordinate.altitude() : fallbackAltitude;
        missionItems << new MissionItem(sequenceNumber++,
                                        MAV_CMD_NAV_WAYPOINT,
                                        MAV_FRAME_GLOBAL_RELATIVE_ALT,
                                        0, 0, 0, 0,
                                        coordinate.latitude(),
                                        coordinate.longitude(),
                                        altitude,
                                        true,
                                        false);
    }

    return missionItems;
}

double SwarmController::_relativeAltitudeMeters(Vehicle* vehicle, double fallbackMeters) const
{
    if (!vehicle || !vehicle->altitudeRelative()) {
        return fallbackMeters;
    }

    bool ok = false;
    const double altitude = vehicle->altitudeRelative()->rawValue().toDouble(&ok);
    return ok && std::isfinite(altitude) ? altitude : fallbackMeters;
}

bool SwarmController::_legacyForwardingFeatureEnabled() const
{
    return qEnvironmentVariableIntValue("MERIVUS_DEV_ENABLE_SWARM_LEGACY_FORWARDING") == 1;
}

bool SwarmController::_autoStartMissionFeatureEnabled() const
{
    return qEnvironmentVariableIntValue("MERIVUS_DEV_ENABLE_SWARM_AUTO_START_MISSION") == 1;
}

void SwarmController::_ensureLegacyForwardingConnected()
{
    if (!_legacyForwardingFeatureEnabled()) {
        return;
    }

    if (_legacyForwardingConnected) {
        return;
    }

    MAVLinkProtocol* mavlinkProtocol = qgcApp()->toolbox()->mavlinkProtocol();
    if (mavlinkProtocol) {
        connect(mavlinkProtocol, &MAVLinkProtocol::messageReceived, this, &SwarmController::_receiveMessage);
        _legacyForwardingConnected = true;
    }
}

bool SwarmController::_sendLegacyStartPacket()
{
    if (!_legacyForwardingFeatureEnabled()) {
        return false;
    }

    MultiVehicleManager* manager = qgcApp()->toolbox()->multiVehicleManager();
    if (!manager || !manager->vehicles()) {
        return false;
    }

    QmlObjectListModel* model = manager->vehicles();
    const int count = model->objectList()->count();
    for (int i = 0; i < count; ++i) {
        Vehicle* vehicle = qobject_cast<Vehicle*>(model->get(i));
        if (!vehicle || vehicle->id() != 1) {
            continue;
        }

        WeakLinkInterfacePtr weakLink = vehicle->vehicleLinkManager()->primaryLink();
        if (weakLink.expired()) {
            return false;
        }

        SharedLinkInterfacePtr sharedLink = weakLink.lock();
        if (!sharedLink) {
            return false;
        }

        mavlink_message_t msg;
        mavlink_msg_gps_raw_int_pack_chan(55, 55, sharedLink->mavlinkChannel(), &msg,
                                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        vehicle->sendMessageOnLinkThreadSafe(sharedLink.get(), msg);
        return true;
    }

    return false;
}

void SwarmController::_receiveMessage(LinkInterface*, mavlink_message_t message)
{
    if (!_legacyForwardingFeatureEnabled() || !_legacyForwardingEnabled || message.msgid != MAVLINK_MSG_ID_GPS_RAW_INT || message.sysid != 1) {
        return;
    }

    mavlink_gps_raw_int_t gpsRaw;
    mavlink_msg_gps_raw_int_decode(&message, &gpsRaw);

    MultiVehicleManager* manager = qgcApp()->toolbox()->multiVehicleManager();
    if (!manager || !manager->vehicles()) {
        return;
    }

    QmlObjectListModel* model = manager->vehicles();
    const int count = model->objectList()->count();
    for (int i = 0; i < count; ++i) {
        Vehicle* vehicle = qobject_cast<Vehicle*>(model->get(i));
        if (!vehicle) {
            continue;
        }

        WeakLinkInterfacePtr weakLink = vehicle->vehicleLinkManager()->primaryLink();
        if (weakLink.expired()) {
            continue;
        }

        SharedLinkInterfacePtr sharedLink = weakLink.lock();
        if (!sharedLink) {
            continue;
        }

        mavlink_message_t msg;
        mavlink_msg_gps_raw_int_pack_chan(1, 1, sharedLink->mavlinkChannel(), &msg,
                                          gpsRaw.time_usec, gpsRaw.fix_type, gpsRaw.lat, gpsRaw.lon, gpsRaw.alt,
                                          gpsRaw.eph, gpsRaw.epv, gpsRaw.vel, 0, 0, 0, 0, 0, 0,
                                          gpsRaw.hdg_acc, gpsRaw.yaw);
        vehicle->sendMessageOnLinkThreadSafe(sharedLink.get(), msg);
    }
}
