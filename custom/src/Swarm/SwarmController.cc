#include "SwarmController.h"

#include <QPointer>
#include <QSet>
#include <QTimer>
#include <QtAlgorithms>

#include <algorithm>
#include <cmath>

#include "Fact.h"
#include "HealthAndArmingCheckReport.h"
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
    _formationWatchdogTimer.setInterval(500);
    connect(&_formationWatchdogTimer, &QTimer::timeout, this, &SwarmController::_checkFormationHealth);

    _temporaryMissionTimer.setInterval(500);
    connect(&_temporaryMissionTimer, &QTimer::timeout, this, &SwarmController::_checkTemporaryMissionProgress);
}

QVariantMap SwarmController::executeGoto(const QVariantList& selectedVehicleIds, const QVariant& targetCoordinate)
{
    const QGeoCoordinate coordinate = _coordinateFromVariant(targetCoordinate);
    if (!coordinate.isValid()) {
        return _result(false, tr("Invalid target coordinate."));
    }
    return _executeGotoInternal(selectedVehicleIds, QList<QGeoCoordinate>() << coordinate, false);
}

QVariantMap SwarmController::executeQueuedGoto(const QVariantList& selectedVehicleIds,
                                               const QVariantList& queuedCoordinates,
                                               const QVariantList& referenceCoordinates,
                                               bool replaceExisting)
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

    QList<QGeoCoordinate> references;
    for (const QVariant& value : referenceCoordinates) {
        const QGeoCoordinate coordinate = _coordinateFromVariant(value);
        if (!coordinate.isValid()) {
            return _result(false, tr("The frozen vehicle geometry contains an invalid coordinate."));
        }
        references << coordinate;
    }

    return _executeGotoInternal(selectedVehicleIds, coordinates, true, references, replaceExisting);
}

QVariantMap SwarmController::executeTakeoff(const QVariantList& selectedVehicleIds, double altitudeMeters)
{
    if (selectedVehicleIds.isEmpty()) {
        return _result(false, tr("Select at least one vehicle before batch takeoff."));
    }

    if (!std::isfinite(altitudeMeters) || altitudeMeters <= 0.0) {
        return _result(false, tr("Takeoff altitude must be greater than zero."));
    }

    QSet<int> requestedIds;
    for (const QVariant& value : selectedVehicleIds) {
        const int id = value.toInt();
        if (id <= 0 || requestedIds.contains(id)) {
            return _result(false, tr("Vehicle selection contains an invalid or duplicate ID."));
        }
        requestedIds.insert(id);
    }

    const QList<Vehicle*> vehicles = _selectedVehicles(selectedVehicleIds);
    QList<int> dispatchedIds;
    QList<int> skippedIds;
    QSet<int> matchedIds;
    int delayMs = 0;

    for (Vehicle* vehicle : vehicles) {
        if (!vehicle) {
            continue;
        }

        matchedIds.insert(vehicle->id());
        if (!_vehicleTakeoffReady(vehicle, altitudeMeters)) {
            skippedIds << vehicle->id();
            continue;
        }

        _dispatchTakeoff(vehicle, altitudeMeters, delayMs);
        dispatchedIds << vehicle->id();
        delayMs += 200;
    }

    for (int id : requestedIds) {
        if (!matchedIds.contains(id)) {
            skippedIds << id;
        }
    }

    const bool ok = !dispatchedIds.isEmpty();
    return _result(ok,
                   ok ? tr("Batch takeoff commands scheduled for the selected vehicles.")
                      : tr("No selected vehicle met the takeoff requirements."),
                   dispatchedIds,
                   skippedIds);
}

bool SwarmController::hasActiveTemporaryMission(const QVariantList& selectedVehicleIds) const
{
    for (const QVariant& value : selectedVehicleIds) {
        const int id = value.toInt();
        if (_temporaryMissions.contains(id) || _staleTemporaryMissionIds.contains(id)) {
            return true;
        }
    }
    return false;
}

bool SwarmController::sitlSwarmModeEnabled() const
{
    return _legacyForwardingFeatureEnabled();
}

bool SwarmController::temporaryMissionExecutionEnabled() const
{
    return _autoStartMissionFeatureEnabled();
}

QVariantMap SwarmController::sendStartCommand(const QVariantList& selectedVehicleIds)
{
    if (!_legacyForwardingFeatureEnabled()) {
        return _result(false, tr("SITL formation mode is disabled. Set MERIVUS_DEV_ENABLE_SWARM_LEGACY_FORWARDING=1 before starting QGC."));
    }

    if (_formationActive) {
        return _result(false, tr("A formation session is already active. End it before starting another one."));
    }

    if (selectedVehicleIds.count() != 6) {
        return _result(false, tr("The SITL formation requires exactly UAV-1 through UAV-6."));
    }

    QSet<int> requestedIds;
    for (const QVariant& value : selectedVehicleIds) {
        const int id = value.toInt();
        if (id < 1 || id > 6 || requestedIds.contains(id)) {
            return _result(false, tr("The SITL formation target must contain each ID from 1 through 6 exactly once."));
        }
        requestedIds.insert(id);
    }

    QList<int> skippedIds;
    const QList<Vehicle*> vehicles = _selectedVehicles(selectedVehicleIds);
    QSet<int> matchedIds;
    for (Vehicle* vehicle : vehicles) {
        if (!vehicle) {
            continue;
        }

        matchedIds.insert(vehicle->id());
        if (!_vehicleFormationReady(vehicle)) {
            skippedIds << vehicle->id();
        }
    }
    for (int id = 1; id <= 6; ++id) {
        if (!matchedIds.contains(id)) {
            skippedIds << id;
        }
    }
    if (!skippedIds.isEmpty()) {
        std::sort(skippedIds.begin(), skippedIds.end());
        return _result(false, tr("All six SITL vehicles must be connected, on the ground, and ready before formation start."), QList<int>(), skippedIds);
    }

    _legacyForwardingEnabled = true;
    _ensureLegacyForwardingConnected();

    if (!_sendLegacyStartPacket()) {
        _legacyForwardingEnabled = false;
        return _result(false, tr("UAV-1 does not have an active primary link."));
    }

    _formationVehicleIds.clear();
    for (int id = 1; id <= 6; ++id) {
        _formationVehicleIds << id;
    }
    _reportedLostFollowerIds.clear();
    _leaderGpsTimer.restart();
    _setFormationActive(true);
    _formationWatchdogTimer.start();

    return _result(true, tr("SITL formation start signal sent to UAV-1."), _formationVehicleIds);
}

QVariantMap SwarmController::endFormationSession()
{
    if (!_formationActive) {
        return _result(false, tr("No formation session is active."));
    }

    QList<int> pausedIds;
    MultiVehicleManager* manager = qgcApp()->toolbox()->multiVehicleManager();
    if (manager && manager->vehicles()) {
        QmlObjectListModel* model = manager->vehicles();
        const int count = model->objectList()->count();
        for (int i = 0; i < count; ++i) {
            Vehicle* vehicle = qobject_cast<Vehicle*>(model->get(i));
            if (vehicle && _formationVehicleIds.contains(vehicle->id())
                && vehicle->vehicleLinkManager()
                && !vehicle->vehicleLinkManager()->communicationLost()) {
                vehicle->pauseVehicle();
                pausedIds << vehicle->id();
            }
        }
    }

    _legacyForwardingEnabled = false;
    _formationWatchdogTimer.stop();
    _formationVehicleIds.clear();
    _reportedLostFollowerIds.clear();
    _setFormationActive(false);
    return _result(true, tr("Formation forwarding stopped; connected members were told to hold position."), pausedIds);
}

QVariantMap SwarmController::_executeGotoInternal(const QVariantList& selectedVehicleIds,
                                                   const QList<QGeoCoordinate>& coordinates,
                                                   bool queued,
                                                   const QList<QGeoCoordinate>& referenceCoordinates,
                                                   bool replaceExisting)
{
    if (coordinates.isEmpty()) {
        return _result(false, tr("No target coordinate."));
    }
    if (queued && !_autoStartMissionFeatureEnabled()) {
        return _result(false, tr("SITL temporary mission execution is disabled. Set MERIVUS_DEV_ENABLE_SWARM_AUTO_START_MISSION=1 before starting QGC."));
    }

    const QGeoCoordinate finalTarget = coordinates.last();
    QList<Vehicle*> vehicles = _selectedVehicles(selectedVehicleIds);
    if (vehicles.isEmpty()) {
        return _result(false, tr("No vehicle available."));
    }

    QList<int> dispatchedIds;
    QList<int> skippedIds;
    QHash<int, QGeoCoordinate> frozenCoordinates;

    if (!selectedVehicleIds.isEmpty()) {
        QSet<int> uniqueIds;
        for (int i = 0; i < selectedVehicleIds.count(); ++i) {
            const QVariant& value = selectedVehicleIds[i];
            const int id = value.toInt();
            if (id <= 0 || uniqueIds.contains(id)) {
                return _result(false, tr("Invalid or duplicate vehicle IDs in selection."));
            }
            uniqueIds.insert(id);
            if (queued) {
                if (referenceCoordinates.count() != selectedVehicleIds.count()) {
                    return _result(false, tr("The frozen target geometry does not match the selected vehicle set."));
                }
                frozenCoordinates.insert(id, referenceCoordinates[i]);
            }
        }
    }

    if (queued && hasActiveTemporaryMission(selectedVehicleIds) && !replaceExisting) {
        QVariantMap result = _result(false, tr("One or more target vehicles already have an active or uncleared temporary mission."));
        result.insert(QStringLiteral("replacementRequired"), true);
        return result;
    }

    if (!selectedVehicleIds.isEmpty()) {
        QSet<int> matchedIds;
        for (Vehicle* vehicle : vehicles) {
            if (vehicle) {
                matchedIds.insert(vehicle->id());
            }
        }
        for (const QVariant& value : selectedVehicleIds) {
            const int id = value.toInt();
            if (!matchedIds.contains(id)) {
                skippedIds << id;
            }
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
            _dispatchTemporaryMission(vehicle, coordinates, 0, replaceExisting);
            dispatchedIds << vehicle->id();
            return _result(true, tr("Temporary mission route upload started."), dispatchedIds, skippedIds);
        }

        _cancelTemporaryMissionForGoto(vehicle);
        _dispatchGoto(vehicle, finalTarget, 0);
        dispatchedIds << vehicle->id();
        return _result(true, tr("Goto command dispatched."), dispatchedIds, skippedIds);
    }

    double centerLat = 0.0;
    double centerLon = 0.0;
    int validCount = 0;
    for (Vehicle* vehicle : vehicles) {
        if (!vehicle) {
            continue;
        }
        const QGeoCoordinate reference = queued ? frozenCoordinates.value(vehicle->id()) : vehicle->coordinate();
        if (reference.isValid()) {
            centerLat += reference.latitude();
            centerLon += reference.longitude();
            ++validCount;
        }
    }

    if (validCount != vehicles.count()) {
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
            const QGeoCoordinate reference = frozenCoordinates.value(vehicle->id());
            QList<QGeoCoordinate> vehicleRoute;
            for (const QGeoCoordinate& coordinate : coordinates) {
                const double moveDistance = centroid.distanceTo(coordinate);
                const double moveAzimuth = centroid.azimuthTo(coordinate);
                QGeoCoordinate vehicleCoordinate = reference.atDistanceAndAzimuth(moveDistance, moveAzimuth);
                vehicleCoordinate.setAltitude(coordinate.altitude());
                vehicleRoute << vehicleCoordinate;
            }
            _dispatchTemporaryMission(vehicle, vehicleRoute, delayMs, replaceExisting);
        } else {
            const double moveDistance = centroid.distanceTo(finalTarget);
            const double moveAzimuth = centroid.azimuthTo(finalTarget);
            const QGeoCoordinate vehicleTarget = vehicle->coordinate().atDistanceAndAzimuth(moveDistance, moveAzimuth);
            _cancelTemporaryMissionForGoto(vehicle);
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

bool SwarmController::_vehicleTakeoffReady(Vehicle* vehicle, double altitudeMeters) const
{
    if (!vehicle
        || !vehicle->isInitialConnectComplete()
        || !vehicle->vehicleLinkManager()
        || vehicle->vehicleLinkManager()->communicationLost()
        || !vehicle->guidedModeSupported()
        || !vehicle->takeoffVehicleSupported()
        || vehicle->flying()
        || altitudeMeters < vehicle->minimumTakeoffAltitude()) {
        return false;
    }

    HealthAndArmingCheckReport* report = vehicle->healthAndArmingCheckReport();
    return !report || !report->supported() || report->canTakeoff();
}

bool SwarmController::_vehicleFormationReady(Vehicle* vehicle) const
{
    if (!vehicle
        || !vehicle->isInitialConnectComplete()
        || !vehicle->vehicleLinkManager()
        || vehicle->vehicleLinkManager()->communicationLost()
        || !vehicle->px4Firmware()
        || vehicle->flying()) {
        return false;
    }

    HealthAndArmingCheckReport* report = vehicle->healthAndArmingCheckReport();
    return !report || !report->supported() || report->canTakeoff();
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

void SwarmController::_dispatchTakeoff(Vehicle* vehicle, double altitudeMeters, int delayMs)
{
    QPointer<Vehicle> guardedVehicle(vehicle);
    QTimer::singleShot(delayMs, this, [this, guardedVehicle, altitudeMeters]() {
        if (guardedVehicle && _vehicleTakeoffReady(guardedVehicle, altitudeMeters)) {
            guardedVehicle->guidedModeTakeoff(altitudeMeters);
        }
    });
}

void SwarmController::_dispatchTemporaryMission(Vehicle* vehicle,
                                                const QList<QGeoCoordinate>& coordinates,
                                                int delayMs,
                                                bool replaceExisting)
{
    QPointer<Vehicle> guardedVehicle(vehicle);
    QTimer::singleShot(delayMs, this, [this, guardedVehicle, coordinates, replaceExisting]() {
        if (!guardedVehicle || !guardedVehicle->missionManager()) {
            return;
        }

        MissionManager* missionManager = guardedVehicle->missionManager();
        if (missionManager->inProgress()) {
            return;
        }

        if (replaceExisting && (_temporaryMissions.contains(guardedVehicle->id()) || _staleTemporaryMissionIds.contains(guardedVehicle->id()))) {
            guardedVehicle->pauseVehicle();
        }

        QList<MissionItem*> missionItems = _buildTemporaryMissionItems(guardedVehicle, coordinates);
        if (missionItems.count() < 2) {
            qDeleteAll(missionItems);
            return;
        }

        _ensureTemporaryMissionConnections(guardedVehicle);

        TemporaryMissionState state;
        state.vehicle = guardedVehicle;
        state.finalCoordinate = coordinates.last();
        state.expectedMissionItemCount = coordinates.count();
        state.stage = TemporaryMissionStage::Uploading;
        _temporaryMissions.insert(guardedVehicle->id(), state);
        _staleTemporaryMissionIds.remove(guardedVehicle->id());
        _temporaryMissionTimer.start();

        missionManager->writeMissionItems(missionItems);
    });
}

void SwarmController::_ensureTemporaryMissionConnections(Vehicle* vehicle)
{
    if (!vehicle || !vehicle->missionManager() || _temporaryMissionConnections.contains(vehicle->id())) {
        return;
    }

    _temporaryMissionConnections.insert(vehicle->id());
    const int vehicleId = vehicle->id();
    QPointer<Vehicle> guardedVehicle(vehicle);
    MissionManager* missionManager = vehicle->missionManager();
    connect(missionManager, &MissionManager::sendComplete, this, [this, guardedVehicle](bool error) {
        if (guardedVehicle) {
            _handleTemporaryMissionSendComplete(guardedVehicle, error);
        }
    });
    connect(missionManager, &MissionManager::removeAllComplete, this, [this, guardedVehicle](bool error) {
        if (guardedVehicle) {
            _handleTemporaryMissionRemoveAllComplete(guardedVehicle, error);
        }
    });
    connect(vehicle, &QObject::destroyed, this, [this, vehicleId]() {
        _temporaryMissions.remove(vehicleId);
        _temporaryMissionConnections.remove(vehicleId);
        _staleTemporaryMissionIds.remove(vehicleId);
        if (_temporaryMissions.isEmpty()) {
            _temporaryMissionTimer.stop();
        }
    });
}

void SwarmController::_handleTemporaryMissionSendComplete(Vehicle* vehicle, bool error)
{
    if (!vehicle || !_temporaryMissions.contains(vehicle->id())) {
        return;
    }

    TemporaryMissionState& state = _temporaryMissions[vehicle->id()];
    if (state.stage == TemporaryMissionStage::Clearing) {
        if (!vehicle->missionManager()->inProgress()) {
            vehicle->missionManager()->removeAll();
        }
        return;
    }
    if (state.stage != TemporaryMissionStage::Uploading) {
        return;
    }

    if (error || !_autoStartMissionFeatureEnabled()) {
        _staleTemporaryMissionIds.insert(vehicle->id());
        _temporaryMissions.remove(vehicle->id());
        if (_temporaryMissions.isEmpty()) {
            _temporaryMissionTimer.stop();
        }
        emit temporaryMissionCompleted(vehicle->id(), true);
        return;
    }

    const int onboardCount = vehicle->missionManager()->missionItems().count();
    state.finalMissionIndex = qMax(0, (onboardCount > 0 ? onboardCount : state.expectedMissionItemCount) - 1);
    state.stage = TemporaryMissionStage::Executing;
    vehicle->startMission();
}

void SwarmController::_handleTemporaryMissionRemoveAllComplete(Vehicle* vehicle, bool error)
{
    if (!vehicle) {
        return;
    }

    const int id = vehicle->id();
    if (!_temporaryMissions.contains(id) && !_staleTemporaryMissionIds.contains(id)) {
        return;
    }
    _temporaryMissions.remove(id);
    if (error) {
        _staleTemporaryMissionIds.insert(id);
    } else {
        _staleTemporaryMissionIds.remove(id);
    }
    if (_temporaryMissions.isEmpty()) {
        _temporaryMissionTimer.stop();
    }
    emit temporaryMissionCompleted(id, error);
}

void SwarmController::_cancelTemporaryMissionForGoto(Vehicle* vehicle)
{
    if (!vehicle || !vehicle->missionManager()
        || (!_temporaryMissions.contains(vehicle->id()) && !_staleTemporaryMissionIds.contains(vehicle->id()))) {
        return;
    }

    _ensureTemporaryMissionConnections(vehicle);
    if (!_temporaryMissions.contains(vehicle->id())) {
        TemporaryMissionState state;
        state.vehicle = vehicle;
        state.stage = TemporaryMissionStage::Clearing;
        _temporaryMissions.insert(vehicle->id(), state);
    } else {
        _temporaryMissions[vehicle->id()].stage = TemporaryMissionStage::Clearing;
    }
    _staleTemporaryMissionIds.insert(vehicle->id());
    vehicle->pauseVehicle();
    _requestTemporaryMissionClear(vehicle);
}

void SwarmController::_requestTemporaryMissionClear(Vehicle* vehicle)
{
    if (!vehicle || !vehicle->missionManager()) {
        return;
    }
    if (!vehicle->missionManager()->inProgress()) {
        vehicle->missionManager()->removeAll();
    }
}

void SwarmController::_checkTemporaryMissionProgress()
{
    const QList<int> ids = _temporaryMissions.keys();
    for (int id : ids) {
        if (!_temporaryMissions.contains(id)) {
            continue;
        }

        TemporaryMissionState& state = _temporaryMissions[id];
        Vehicle* vehicle = state.vehicle;
        if (!vehicle || !vehicle->missionManager()) {
            _temporaryMissions.remove(id);
            continue;
        }
        if (state.stage == TemporaryMissionStage::Clearing) {
            _requestTemporaryMissionClear(vehicle);
            continue;
        }
        if (state.stage != TemporaryMissionStage::Executing
            || vehicle->missionManager()->currentIndex() < state.finalMissionIndex
            || !vehicle->coordinate().isValid()
            || !state.finalCoordinate.isValid()
            || vehicle->coordinate().distanceTo(state.finalCoordinate) > 2.0) {
            continue;
        }

        bool speedOk = false;
        const double groundSpeed = vehicle->groundSpeed()->rawValue().toDouble(&speedOk);
        if (!speedOk || !std::isfinite(groundSpeed) || groundSpeed > 0.8) {
            continue;
        }

        state.stage = TemporaryMissionStage::Clearing;
        vehicle->pauseVehicle();
        QPointer<Vehicle> guardedVehicle(vehicle);
        QTimer::singleShot(500, this, [this, guardedVehicle]() {
            if (guardedVehicle) {
                _requestTemporaryMissionClear(guardedVehicle);
            }
        });
    }
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

void SwarmController::_setFormationActive(bool active)
{
    if (_formationActive == active) {
        return;
    }
    _formationActive = active;
    emit formationActiveChanged();
}

void SwarmController::_pauseFormationFollowers()
{
    MultiVehicleManager* manager = qgcApp()->toolbox()->multiVehicleManager();
    if (!manager || !manager->vehicles()) {
        return;
    }

    QmlObjectListModel* model = manager->vehicles();
    const int count = model->objectList()->count();
    for (int i = 0; i < count; ++i) {
        Vehicle* vehicle = qobject_cast<Vehicle*>(model->get(i));
        if (!vehicle || vehicle->id() == 1 || !_formationVehicleIds.contains(vehicle->id())
            || !vehicle->vehicleLinkManager() || vehicle->vehicleLinkManager()->communicationLost()) {
            continue;
        }
        vehicle->pauseVehicle();
    }
}

void SwarmController::_checkFormationHealth()
{
    if (!_formationActive) {
        _formationWatchdogTimer.stop();
        return;
    }

    MultiVehicleManager* manager = qgcApp()->toolbox()->multiVehicleManager();
    Vehicle* leader = manager ? manager->getVehicleById(1) : nullptr;
    const bool leaderLost = !leader
        || !leader->vehicleLinkManager()
        || leader->vehicleLinkManager()->communicationLost()
        || (_leaderGpsTimer.isValid() && _leaderGpsTimer.elapsed() > 3000);
    if (leaderLost) {
        _pauseFormationFollowers();
        _legacyForwardingEnabled = false;
        _formationWatchdogTimer.stop();
        _setFormationActive(false);
        emit formationFault(tr("UAV-1 telemetry timed out. Connected followers were told to hold position."));
        return;
    }

    for (int id = 2; id <= 6; ++id) {
        Vehicle* follower = manager ? manager->getVehicleById(id) : nullptr;
        const bool lost = !follower
            || !follower->vehicleLinkManager()
            || follower->vehicleLinkManager()->communicationLost();
        if (lost && !_reportedLostFollowerIds.contains(id)) {
            _reportedLostFollowerIds.insert(id);
            emit formationFault(tr("UAV-%1 is disconnected. The remaining formation continues.").arg(id));
        } else if (!lost) {
            _reportedLostFollowerIds.remove(id);
        }
    }
}

void SwarmController::_receiveMessage(LinkInterface*, mavlink_message_t message)
{
    if (!_legacyForwardingFeatureEnabled() || !_legacyForwardingEnabled || message.msgid != MAVLINK_MSG_ID_GPS_RAW_INT || message.sysid != 1) {
        return;
    }

    mavlink_gps_raw_int_t gpsRaw;
    mavlink_msg_gps_raw_int_decode(&message, &gpsRaw);
    _leaderGpsTimer.restart();

    MultiVehicleManager* manager = qgcApp()->toolbox()->multiVehicleManager();
    if (!manager || !manager->vehicles()) {
        return;
    }

    QmlObjectListModel* model = manager->vehicles();
    const int count = model->objectList()->count();
    for (int i = 0; i < count; ++i) {
        Vehicle* vehicle = qobject_cast<Vehicle*>(model->get(i));
        if (!vehicle || vehicle->id() == 1 || !_formationVehicleIds.contains(vehicle->id())) {
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
