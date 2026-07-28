#pragma once

#include <QElapsedTimer>
#include <QObject>
#include <QGeoCoordinate>
#include <QHash>
#include <QSet>
#include <QTimer>
#include <QVariantList>
#include <QVariantMap>

#include "MAVLinkProtocol.h"

class Vehicle;
class MissionItem;
class LinkInterface;

class SwarmController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool sitlSwarmModeEnabled READ sitlSwarmModeEnabled CONSTANT)
    Q_PROPERTY(bool temporaryMissionExecutionEnabled READ temporaryMissionExecutionEnabled CONSTANT)
    Q_PROPERTY(bool formationActive READ formationActive NOTIFY formationActiveChanged)

public:
    explicit SwarmController(QObject* parent = nullptr);

    Q_INVOKABLE QVariantMap executeGoto(const QVariantList& selectedVehicleIds, const QVariant& targetCoordinate);
    Q_INVOKABLE QVariantMap executeQueuedGoto(const QVariantList& selectedVehicleIds,
                                              const QVariantList& queuedCoordinates,
                                              const QVariantList& referenceCoordinates,
                                              bool replaceExisting);
    Q_INVOKABLE QVariantMap executeTakeoff(const QVariantList& selectedVehicleIds, double altitudeMeters);
    Q_INVOKABLE bool hasActiveTemporaryMission(const QVariantList& selectedVehicleIds) const;
    Q_INVOKABLE QVariantMap sendStartCommand(const QVariantList& selectedVehicleIds);
    Q_INVOKABLE QVariantMap endFormationSession();

    bool sitlSwarmModeEnabled() const;
    bool temporaryMissionExecutionEnabled() const;
    bool formationActive() const { return _formationActive; }

signals:
    void formationActiveChanged();
    void formationFault(const QString& message);
    void temporaryMissionCompleted(int vehicleId, bool clearError);

private slots:
    void _receiveMessage(LinkInterface* link, mavlink_message_t message);
    void _checkFormationHealth();
    void _checkTemporaryMissionProgress();

private:
    enum class TemporaryMissionStage {
        Uploading,
        Executing,
        Clearing,
    };

    struct TemporaryMissionState {
        Vehicle* vehicle = nullptr;
        QGeoCoordinate finalCoordinate;
        int expectedMissionItemCount = 0;
        int finalMissionIndex = -1;
        TemporaryMissionStage stage = TemporaryMissionStage::Uploading;
    };

    QList<Vehicle*> _selectedVehicles(const QVariantList& selectedVehicleIds) const;
    QVariantMap _executeGotoInternal(const QVariantList& selectedVehicleIds,
                                     const QList<QGeoCoordinate>& coordinates,
                                     bool queued,
                                     const QList<QGeoCoordinate>& referenceCoordinates = QList<QGeoCoordinate>(),
                                     bool replaceExisting = false);
    QVariantMap _result(bool ok, const QString& message, const QList<int>& dispatchedIds = QList<int>(), const QList<int>& skippedIds = QList<int>()) const;
    QGeoCoordinate _coordinateFromVariant(const QVariant& value) const;
    bool _vehicleReady(Vehicle* vehicle) const;
    bool _vehicleTakeoffReady(Vehicle* vehicle, double altitudeMeters) const;
    bool _vehicleFormationReady(Vehicle* vehicle) const;
    void _dispatchGoto(Vehicle* vehicle, const QGeoCoordinate& coordinate, int delayMs);
    void _dispatchTakeoff(Vehicle* vehicle, double altitudeMeters, int delayMs);
    void _dispatchTemporaryMission(Vehicle* vehicle, const QList<QGeoCoordinate>& coordinates, int delayMs, bool replaceExisting);
    void _ensureTemporaryMissionConnections(Vehicle* vehicle);
    void _handleTemporaryMissionSendComplete(Vehicle* vehicle, bool error);
    void _handleTemporaryMissionRemoveAllComplete(Vehicle* vehicle, bool error);
    void _cancelTemporaryMissionForGoto(Vehicle* vehicle);
    void _requestTemporaryMissionClear(Vehicle* vehicle);
    QList<MissionItem*> _buildTemporaryMissionItems(Vehicle* vehicle, const QList<QGeoCoordinate>& coordinates) const;
    double _relativeAltitudeMeters(Vehicle* vehicle, double fallbackMeters) const;
    bool _legacyForwardingFeatureEnabled() const;
    bool _autoStartMissionFeatureEnabled() const;
    void _ensureLegacyForwardingConnected();
    bool _sendLegacyStartPacket();
    void _setFormationActive(bool active);
    void _pauseFormationFollowers();

    bool _legacyForwardingConnected = false;
    bool _legacyForwardingEnabled = false;
    bool _formationActive = false;
    QList<int> _formationVehicleIds;
    QSet<int> _reportedLostFollowerIds;
    QElapsedTimer _leaderGpsTimer;
    QTimer _formationWatchdogTimer;
    QHash<int, TemporaryMissionState> _temporaryMissions;
    QSet<int> _temporaryMissionConnections;
    QSet<int> _staleTemporaryMissionIds;
    QTimer _temporaryMissionTimer;
    static const double kMinimumGuidedAltitudeMeters;
    static const double kDefaultMissionAltitudeMeters;
};
