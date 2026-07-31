#pragma once

#include <QElapsedTimer>
#include <QObject>
#include <QGeoCoordinate>
#include <QHash>
#include <QSet>
#include <QString>
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
    Q_PROPERTY(bool swarmModeEnabled READ swarmModeEnabled CONSTANT)
    Q_PROPERTY(bool temporaryMissionExecutionEnabled READ temporaryMissionExecutionEnabled CONSTANT)
    Q_PROPERTY(bool formationActive READ formationActive NOTIFY formationActiveChanged)
    Q_PROPERTY(bool formationBusy READ formationBusy NOTIFY formationBusyChanged)
    Q_PROPERTY(QString formationStatus READ formationStatus NOTIFY formationStatusChanged)

public:
    explicit SwarmController(QObject* parent = nullptr);

    Q_INVOKABLE QVariantMap executeGoto(const QVariantList& selectedVehicleIds, const QVariant& targetCoordinate);
    Q_INVOKABLE QVariantMap executeQueuedGoto(const QVariantList& selectedVehicleIds,
                                              const QVariantList& queuedCoordinates,
                                              const QVariantList& referenceCoordinates,
                                              bool replaceExisting);
    Q_INVOKABLE QVariantMap executeTakeoff(const QVariantList& selectedVehicleIds, double altitudeMeters);
    Q_INVOKABLE QVariantMap executeLand(const QVariantList& selectedVehicleIds);
    Q_INVOKABLE QVariantMap executeRTL(const QVariantList& selectedVehicleIds);
    Q_INVOKABLE bool hasActiveTemporaryMission(const QVariantList& selectedVehicleIds) const;
    Q_INVOKABLE QVariantMap sendStartCommand(const QVariantList& selectedVehicleIds);
    Q_INVOKABLE QVariantMap endFormationSession();

    bool swarmModeEnabled() const;
    bool temporaryMissionExecutionEnabled() const;
    bool formationActive() const { return _formationActive; }
    bool formationBusy() const;
    QString formationStatus() const { return _formationStatus; }

signals:
    void formationActiveChanged();
    void formationBusyChanged();
    void formationStatusChanged();
    void formationFault(const QString& message);
    void temporaryMissionCompleted(int vehicleId, bool clearError);

private slots:
    void _receiveMessage(LinkInterface* link, mavlink_message_t message);
    void _checkFormationHealth();
    void _handleFormationCommandResult(int vehicleId, int targetComponent, int command, int ackResult, int failureCode);
    void _handleFormationCommandTimeout();
    void _checkTemporaryMissionProgress();

private:
    enum class TemporaryMissionStage {
        Uploading,
        Executing,
        Clearing,
    };

    enum class FormationPhase {
        Idle,
        Preparing,
        Committing,
        Releasing,
        Active,
        Aborting,
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
    bool _vehicleLandReady(Vehicle* vehicle) const;
    bool _vehicleRTLReady(Vehicle* vehicle) const;
    bool _vehicleFormationReady(Vehicle* vehicle) const;
    void _dispatchGoto(Vehicle* vehicle, const QGeoCoordinate& coordinate, int delayMs);
    void _dispatchTakeoff(Vehicle* vehicle, double altitudeMeters, int delayMs);
    void _dispatchLand(Vehicle* vehicle, int delayMs);
    void _dispatchRTL(Vehicle* vehicle, int delayMs);
    void _dispatchTemporaryMission(Vehicle* vehicle, const QList<QGeoCoordinate>& coordinates, int delayMs, bool replaceExisting);
    void _ensureTemporaryMissionConnections(Vehicle* vehicle);
    void _handleTemporaryMissionSendComplete(Vehicle* vehicle, bool error);
    void _handleTemporaryMissionRemoveAllComplete(Vehicle* vehicle, bool error);
    void _cancelTemporaryMissionForGoto(Vehicle* vehicle);
    void _requestTemporaryMissionClear(Vehicle* vehicle);
    QList<MissionItem*> _buildTemporaryMissionItems(Vehicle* vehicle, const QList<QGeoCoordinate>& coordinates) const;
    double _relativeAltitudeMeters(Vehicle* vehicle, double fallbackMeters) const;
    void _ensureFormationForwardingConnected();
    void _ensureFormationCommandConnection(Vehicle* vehicle);
    bool _sendFormationCommand(MAV_CMD command,
                               const QList<int>& vehicleIds,
                               QList<int>* dispatchedIds = nullptr,
                               QList<int>* skippedIds = nullptr);
    void _beginFormationCommit();
    void _beginFormationRelease();
    void _beginFormationAbort(const QString& reason);
    void _finishFormationIdle(const QString& status);
    void _setFormationPhase(FormationPhase phase);
    void _setFormationStatus(const QString& status);
    quint8 _memberMask(const QList<int>& vehicleIds) const;
    void _setFormationActive(bool active);
    void _pauseFormationFollowers();

    bool _formationForwardingConnected = false;
    bool _formationForwardingEnabled = false;
    bool _formationActive = false;
    FormationPhase _formationPhase = FormationPhase::Idle;
    QString _formationStatus;
    QList<int> _formationVehicleIds;
    QSet<int> _pendingFormationCommandIds;
    QSet<int> _successfulFormationCommandIds;
    QSet<int> _reportedLostFollowerIds;
    quint8 _formationMemberMask = 0;
    quint32 _formationSessionId = 0;
    QElapsedTimer _leaderGpsTimer;
    QTimer _formationWatchdogTimer;
    QTimer _formationCommandTimer;
    QHash<int, TemporaryMissionState> _temporaryMissions;
    QSet<int> _temporaryMissionConnections;
    QSet<int> _staleTemporaryMissionIds;
    QTimer _temporaryMissionTimer;
    // Product defaults: formation control and Shift temporary mission execution
    // are available in both SITL and supported PX4 hardware builds.
    static constexpr bool kFormationFeatureEnabled = true;
    static constexpr bool kAutoStartMissionFeatureEnabled = true;
    static constexpr uint8_t kSwarmProtocolVersion = 2;
    static constexpr uint8_t kSwarmVehicleCount = 6;
    static constexpr uint8_t kSwarmLeaderSystemId = 1;
    static constexpr quint32 kMaximumSessionId = 0x00ffffffU;
    static constexpr quint64 kFollowTargetMagicPrefix = 0x4d45524900000000ULL; // "MERI" + session
    static const double kMinimumGuidedAltitudeMeters;
    static const double kDefaultMissionAltitudeMeters;
};
