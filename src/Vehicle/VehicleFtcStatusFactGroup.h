/****************************************************************************
 * Copyright (c) 2026 MERIVUS. All rights reserved.
 ****************************************************************************/

#pragma once

#include "FactGroup.h"
#include "QGCMAVLink.h"

#include <QAbstractListModel>
#include <QDateTime>
#include <QTimer>

#include <array>

class FtcMotorStatusModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        MotorIndexRole = Qt::UserRole + 1,
        HealthRole,
        EffectivenessRole,
        FaultProbabilityRole,
        ConfidenceRole,
        FaultTypeRole,
        FaultTypeTextRole,
        DegradedRole,
        FailedRole,
        AvailableRole,
        SeverityRole,
        UncertaintyRole,
        EstimateAgeRole,
        DataStateTextRole,
        LastEffectivenessRole,
        EstimateValidRole,
    };

    explicit FtcMotorStatusModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE QVariantMap get(int row) const;

    void update(const mavlink_merivus_ftc_motor_status_t& message);
    void setStale(bool stale);

    static QString faultTypeText(uint8_t faultType);

private:
    struct MotorData {
        double health = -1.0;
        double effectiveness = -1.0;
        double faultProbability = -1.0;
        double confidence = -1.0;
        double uncertainty = -1.0;
        double estimateAge = -1.0;
        int estimatorState = 0;
        int diagnosisState = 0;
        bool estimateValid = false;
        bool observable = false;
        uint8_t faultType = MERIVUS_FTC_FAULT_NONE;
        bool degraded = false;
        bool failed = false;
        bool available = false;
    };

    QVariant _roleValue(const MotorData& motor, int row, int role) const;
    static double _decodePercentage(uint8_t value);

    std::array<MotorData, 12> _motors{};
    int _motorCount = 0;
    bool _stale = false;
};

class VehicleFtcStatusFactGroup : public FactGroup
{
    Q_OBJECT

public:
    explicit VehicleFtcStatusFactGroup(QObject* parent = nullptr);

    Q_PROPERTY(bool enabled READ enabled NOTIFY statusChanged)
    Q_PROPERTY(bool available READ available NOTIFY statusChanged)
    Q_PROPERTY(QDateTime lastUpdate READ lastUpdate NOTIFY statusChanged)
    Q_PROPERTY(bool stale READ stale NOTIFY statusChanged)
    Q_PROPERTY(int protocolVersion READ protocolVersion NOTIFY statusChanged)
    Q_PROPERTY(bool protocolCompatible READ protocolCompatible NOTIFY statusChanged)

    Q_PROPERTY(bool motorAvailable READ motorAvailable NOTIFY statusChanged)
    Q_PROPERTY(bool motorStale READ motorStale NOTIFY statusChanged)
    Q_PROPERTY(bool controlAvailable READ controlAvailable NOTIFY statusChanged)
    Q_PROPERTY(bool controlStale READ controlStale NOTIFY statusChanged)
    Q_PROPERTY(bool extremeAvailable READ extremeAvailable NOTIFY statusChanged)
    Q_PROPERTY(bool extremeStale READ extremeStale NOTIFY statusChanged)
    Q_PROPERTY(bool diagnosticsAvailable READ diagnosticsAvailable NOTIFY statusChanged)
    Q_PROPERTY(bool diagnosticsStale READ diagnosticsStale NOTIFY statusChanged)

    Q_PROPERTY(int systemState READ systemState NOTIFY statusChanged)
    Q_PROPERTY(QString systemStateText READ systemStateText NOTIFY statusChanged)
    Q_PROPERTY(QString systemSeverity READ systemSeverity NOTIFY statusChanged)
    Q_PROPERTY(int controlMode READ controlMode NOTIFY statusChanged)
    Q_PROPERTY(QString controlModeText READ controlModeText NOTIFY statusChanged)
    Q_PROPERTY(QString controlModeSeverity READ controlModeSeverity NOTIFY statusChanged)

    Q_PROPERTY(int motorCount READ motorCount NOTIFY statusChanged)
    Q_PROPERTY(QAbstractListModel* motors READ motors CONSTANT)
    Q_PROPERTY(double modelQuality READ modelQuality NOTIFY statusChanged)
    Q_PROPERTY(bool modelValid READ modelValid NOTIFY statusChanged)
    Q_PROPERTY(bool baselineLearned READ baselineLearned NOTIFY statusChanged)
    Q_PROPERTY(bool currentObservable READ currentObservable NOTIFY statusChanged)
    Q_PROPERTY(double estimateAge READ estimateAge NOTIFY statusChanged)
    Q_PROPERTY(QString modelStateText READ modelStateText NOTIFY statusChanged)
    Q_PROPERTY(QVariantMap details READ details NOTIFY statusChanged)

    Q_PROPERTY(int authorityState READ authorityState NOTIFY statusChanged)
    Q_PROPERTY(QString authorityStateText READ authorityStateText NOTIFY statusChanged)
    Q_PROPERTY(QString authoritySeverity READ authoritySeverity NOTIFY statusChanged)
    Q_PROPERTY(double rollAuthority READ rollAuthority NOTIFY statusChanged)
    Q_PROPERTY(double pitchAuthority READ pitchAuthority NOTIFY statusChanged)
    Q_PROPERTY(double yawAuthority READ yawAuthority NOTIFY statusChanged)
    Q_PROPERTY(double thrustAuthority READ thrustAuthority NOTIFY statusChanged)
    Q_PROPERTY(double minimumAttitudeAuthority READ minimumAttitudeAuthority NOTIFY statusChanged)
    Q_PROPERTY(double actuatorHeadroom READ actuatorHeadroom NOTIFY statusChanged)
    Q_PROPERTY(quint16 saturatedMask READ saturatedMask NOTIFY statusChanged)
    Q_PROPERTY(double systemConfidence READ systemConfidence NOTIFY statusChanged)

    Q_PROPERTY(int impactType READ impactType NOTIFY statusChanged)
    Q_PROPERTY(QString impactTypeText READ impactTypeText NOTIFY statusChanged)
    Q_PROPERTY(int locState READ locState NOTIFY statusChanged)
    Q_PROPERTY(QString locStateText READ locStateText NOTIFY statusChanged)
    Q_PROPERTY(QString locSeverity READ locSeverity NOTIFY statusChanged)
    Q_PROPERTY(int recoveryState READ recoveryState NOTIFY statusChanged)
    Q_PROPERTY(QString recoveryStateText READ recoveryStateText NOTIFY statusChanged)
    Q_PROPERTY(QString recoverySeverity READ recoverySeverity NOTIFY statusChanged)
    Q_PROPERTY(bool recoveryCandidate READ recoveryCandidate NOTIFY statusChanged)
    Q_PROPERTY(double recoveryProgress READ recoveryProgress NOTIFY statusChanged)
    Q_PROPERTY(double impactScore READ impactScore NOTIFY statusChanged)
    Q_PROPERTY(double impactConfidence READ impactConfidence NOTIFY statusChanged)
    Q_PROPERTY(double impactSeverity READ impactSeverity NOTIFY statusChanged)
    Q_PROPERTY(double lossOfControlScore READ lossOfControlScore NOTIFY statusChanged)
    Q_PROPERTY(quint32 lossOfControlReasonMask READ lossOfControlReasonMask NOTIFY statusChanged)
    Q_PROPERTY(quint32 recoveryTriggerMask READ recoveryTriggerMask NOTIFY statusChanged)
    Q_PROPERTY(quint32 recoveryInhibitMask READ recoveryInhibitMask NOTIFY statusChanged)

    Q_PROPERTY(double modelResidual READ modelResidual NOTIFY statusChanged)
    Q_PROPERTY(double excitation READ excitation NOTIFY statusChanged)
    Q_PROPERTY(double maneuverIntensity READ maneuverIntensity NOTIFY statusChanged)
    Q_PROPERTY(double externalDisturbanceScore READ externalDisturbanceScore NOTIFY statusChanged)
    Q_PROPERTY(double vibrationScore READ vibrationScore NOTIFY statusChanged)
    Q_PROPERTY(double allocationResidualNorm READ allocationResidualNorm NOTIFY statusChanged)
    Q_PROPERTY(double attitudeError READ attitudeError NOTIFY statusChanged)
    Q_PROPERTY(double rateError READ rateError NOTIFY statusChanged)
    Q_PROPERTY(double jerk READ jerk NOTIFY statusChanged)
    Q_PROPERTY(double accelerationMagnitude READ accelerationMagnitude NOTIFY statusChanged)
    Q_PROPERTY(double angularRate READ angularRate NOTIFY statusChanged)
    Q_PROPERTY(double angularAcceleration READ angularAcceleration NOTIFY statusChanged)
    Q_PROPERTY(quint32 systemReasonMask READ systemReasonMask NOTIFY statusChanged)
    Q_PROPERTY(bool simulationEnabled READ simulationEnabled NOTIFY statusChanged)
    Q_PROPERTY(bool simulationIntermittent READ simulationIntermittent NOTIFY statusChanged)
    Q_PROPERTY(int simulationMotorIndex READ simulationMotorIndex NOTIFY statusChanged)
    Q_PROPERTY(double simulationTargetEffectiveness READ simulationTargetEffectiveness NOTIFY statusChanged)
    Q_PROPERTY(double simulationAppliedEffectiveness READ simulationAppliedEffectiveness NOTIFY statusChanged)

    bool enabled() const { return _enabled; }
    bool available() const;
    QDateTime lastUpdate() const { return _lastUpdate; }
    bool stale() const;
    int protocolVersion() const { return _protocolVersion; }
    bool protocolCompatible() const { return _protocolCompatible; }

    bool motorAvailable() const { return _protocolCompatible && _motorReceived; }
    bool motorStale() const { return _motorStale; }
    bool controlAvailable() const { return _protocolCompatible && _controlReceived; }
    bool controlStale() const { return _controlStale; }
    bool extremeAvailable() const { return _protocolCompatible && _extremeReceived; }
    bool extremeStale() const { return _extremeStale; }
    bool diagnosticsAvailable() const { return _protocolCompatible && _diagnosticsReceived; }
    bool diagnosticsStale() const { return _diagnosticsStale; }

    int systemState() const { return _systemState; }
    QString systemStateText() const;
    QString systemSeverity() const;
    int controlMode() const { return _controlMode; }
    QString controlModeText() const;
    QString controlModeSeverity() const;

    int motorCount() const { return _motorCount; }
    QAbstractListModel* motors() { return &_motors; }
    double modelQuality() const { return _modelQuality; }
    bool modelValid() const { return _modelValid && motorAvailable() && !motorStale(); }
    bool baselineLearned() const { return _baselineLearned; }
    bool currentObservable() const { return _currentObservable && motorAvailable() && !motorStale(); }
    double estimateAge() const { return motorAvailable() && !motorStale() ? _estimateAge : -1.0; }
    QString modelStateText() const;
    QVariantMap details() const { return _details; }

    int authorityState() const { return _authorityState; }
    QString authorityStateText() const;
    QString authoritySeverity() const;
    double rollAuthority() const { return _rollAuthority; }
    double pitchAuthority() const { return _pitchAuthority; }
    double yawAuthority() const { return _yawAuthority; }
    double thrustAuthority() const { return _thrustAuthority; }
    double minimumAttitudeAuthority() const { return _minimumAttitudeAuthority; }
    double actuatorHeadroom() const { return _actuatorHeadroom; }
    quint16 saturatedMask() const { return _saturatedMask; }
    double systemConfidence() const { return _systemConfidence; }

    int impactType() const { return _impactType; }
    QString impactTypeText() const;
    int locState() const { return _locState; }
    QString locStateText() const;
    QString locSeverity() const;
    int recoveryState() const { return _recoveryState; }
    QString recoveryStateText() const;
    QString recoverySeverity() const;
    bool recoveryCandidate() const { return _recoveryCandidate; }
    double recoveryProgress() const { return _recoveryProgress; }
    double impactScore() const { return _impactScore; }
    double impactConfidence() const { return _impactConfidence; }
    double impactSeverity() const { return _impactSeverity; }
    double lossOfControlScore() const { return _lossOfControlScore; }
    quint32 lossOfControlReasonMask() const { return _lossOfControlReasonMask; }
    quint32 recoveryTriggerMask() const { return _recoveryTriggerMask; }
    quint32 recoveryInhibitMask() const { return _recoveryInhibitMask; }

    double modelResidual() const { return _modelResidual; }
    double excitation() const { return _excitation; }
    double maneuverIntensity() const { return _maneuverIntensity; }
    double externalDisturbanceScore() const { return _externalDisturbanceScore; }
    double vibrationScore() const { return _vibrationScore; }
    double allocationResidualNorm() const { return _allocationResidualNorm; }
    double attitudeError() const { return _attitudeError; }
    double rateError() const { return _rateError; }
    double jerk() const { return _jerk; }
    double accelerationMagnitude() const { return _accelerationMagnitude; }
    double angularRate() const { return _angularRate; }
    double angularAcceleration() const { return _angularAcceleration; }
    quint32 systemReasonMask() const { return _systemReasonMask; }
    bool simulationEnabled() const { return _simulationEnabled; }
    bool simulationIntermittent() const { return _simulationIntermittent; }
    int simulationMotorIndex() const { return _simulationMotorIndex; }
    double simulationTargetEffectiveness() const { return _simulationTargetEffectiveness; }
    double simulationAppliedEffectiveness() const { return _simulationAppliedEffectiveness; }

    void handleMessage(Vehicle* vehicle, mavlink_message_t& message) override;

signals:
    void statusChanged();

private slots:
    void _refreshStale();

private:
    static constexpr int ProtocolVersion = 2;
    static constexpr qint64 StaleTimeoutMs = 3000;

    static double _decodePercentage(uint8_t value);
    void _markReceived(qint64& receivedAt, bool& received, bool& stale, unsigned stream, uint8_t version);
    QString _severityForRecovery() const;

    FtcMotorStatusModel _motors;
    QTimer _staleTimer;
    QDateTime _lastUpdate;
    int _protocolVersion = 0;
    bool _protocolCompatible = true;
    std::array<bool, 4> _streamCompatible{{true, true, true, true}};
    bool _enabled = false;
    bool _motorReceived = false;
    bool _controlReceived = false;
    bool _extremeReceived = false;
    bool _diagnosticsReceived = false;
    bool _motorStale = false;
    bool _controlStale = false;
    bool _extremeStale = false;
    bool _diagnosticsStale = false;
    qint64 _motorReceivedAt = 0;
    qint64 _controlReceivedAt = 0;
    qint64 _extremeReceivedAt = 0;
    qint64 _diagnosticsReceivedAt = 0;

    int _systemState = MERIVUS_FTC_SYSTEM_STATE_DISABLED;
    int _controlMode = MERIVUS_FTC_CONTROL_MODE_DISABLED;
    int _motorCount = 0;
    double _modelQuality = -1.0;
    bool _modelValid = false;
    bool _baselineLearned = false;
    bool _currentObservable = false;
    bool _authorityValid = false;
    int _estimatorState = 0;
    double _estimateAge = -1.0;
    QVariantMap _details;
    int _authorityState = MERIVUS_FTC_AUTHORITY_STATE_FULL_CONTROL;
    double _rollAuthority = -1.0;
    double _pitchAuthority = -1.0;
    double _yawAuthority = -1.0;
    double _thrustAuthority = -1.0;
    double _minimumAttitudeAuthority = -1.0;
    double _actuatorHeadroom = -1.0;
    quint16 _saturatedMask = 0;
    double _systemConfidence = -1.0;

    int _impactType = MERIVUS_FTC_IMPACT_TYPE_NONE;
    int _locState = MERIVUS_FTC_LOC_STATE_NORMAL;
    int _recoveryState = MERIVUS_FTC_RECOVERY_STATE_DISABLED;
    bool _recoveryCandidate = false;
    double _recoveryProgress = -1.0;
    double _impactScore = -1.0;
    double _impactConfidence = -1.0;
    double _impactSeverity = -1.0;
    double _lossOfControlScore = -1.0;
    quint32 _lossOfControlReasonMask = 0;
    quint32 _recoveryTriggerMask = 0;
    quint32 _recoveryInhibitMask = 0;

    double _modelResidual = 0.0;
    double _excitation = 0.0;
    double _maneuverIntensity = 0.0;
    double _externalDisturbanceScore = 0.0;
    double _vibrationScore = 0.0;
    double _allocationResidualNorm = 0.0;
    double _attitudeError = 0.0;
    double _rateError = 0.0;
    double _jerk = 0.0;
    double _accelerationMagnitude = 0.0;
    double _angularRate = 0.0;
    double _angularAcceleration = 0.0;
    quint32 _systemReasonMask = 0;
    bool _simulationEnabled = false;
    bool _simulationIntermittent = false;
    int _simulationMotorIndex = -1;
    double _simulationTargetEffectiveness = -1.0;
    double _simulationAppliedEffectiveness = -1.0;
};
