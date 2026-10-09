/****************************************************************************
 * Copyright (c) 2026 MERIVUS. All rights reserved.
 ****************************************************************************/

#include "VehicleFtcStatusFactGroup.h"

#include <QtMath>

FtcMotorStatusModel::FtcMotorStatusModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int FtcMotorStatusModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : _motorCount;
}

QVariant FtcMotorStatusModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= _motorCount) {
        return {};
    }

    return _roleValue(_motors[static_cast<size_t>(index.row())], index.row(), role);
}

QHash<int, QByteArray> FtcMotorStatusModel::roleNames() const
{
    return {
        {MotorIndexRole, "motorIndex"},
        {HealthRole, "health"},
        {EffectivenessRole, "effectiveness"},
        {FaultProbabilityRole, "faultProbability"},
        {ConfidenceRole, "confidence"},
        {FaultTypeRole, "faultType"},
        {FaultTypeTextRole, "faultTypeText"},
        {DegradedRole, "degraded"},
        {FailedRole, "failed"},
        {AvailableRole, "available"},
        {SeverityRole, "severity"},
        {UncertaintyRole, "uncertainty"}, {EstimateAgeRole, "estimateAge"},
        {DataStateTextRole, "dataStateText"}, {LastEffectivenessRole, "lastEffectiveness"},
        {EstimateValidRole, "estimateValid"},
    };
}

QVariantMap FtcMotorStatusModel::get(int row) const
{
    QVariantMap result;

    if (row < 0 || row >= _motorCount) {
        return result;
    }

    const auto roles = roleNames();
    const MotorData& motor = _motors[static_cast<size_t>(row)];

    for (auto it = roles.cbegin(); it != roles.cend(); ++it) {
        result.insert(QString::fromUtf8(it.value()), _roleValue(motor, row, it.key()));
    }

    return result;
}

void FtcMotorStatusModel::update(const mavlink_merivus_ftc_motor_status_t& message)
{
    const int nextCount = qMin<int>(message.motor_count, static_cast<int>(_motors.size()));
    const bool resetModel = nextCount != _motorCount;

    if (resetModel) {
        beginResetModel();
        _motorCount = nextCount;
    }

    for (int i = 0; i < static_cast<int>(_motors.size()); ++i) {
        MotorData& motor = _motors[static_cast<size_t>(i)];
        motor.available = i < _motorCount;
        motor.estimateValid = (message.flags & MERIVUS_FTC_MOTOR_FLAGS_MODEL_VALID) != 0;
        motor.observable = message.current_observable != 0;
        motor.estimatorState = message.estimator_state;
        motor.diagnosisState = message.diagnosis_state[i];
        motor.uncertainty = qIsFinite(message.estimate_uncertainty[i]) ? message.estimate_uncertainty[i] : -1.0;
        motor.estimateAge = qIsFinite(message.estimate_age) ? message.estimate_age : -1.0;
        motor.health = _decodePercentage(message.health_pct[i]);
        motor.effectiveness = _decodePercentage(message.effectiveness_pct[i]);
        motor.faultProbability = _decodePercentage(message.fault_probability_pct[i]);
        motor.confidence = _decodePercentage(message.confidence_pct[i]);
        motor.faultType = message.fault_type[i];
        motor.degraded = (message.degraded_mask & (1U << i)) != 0;
        motor.failed = (message.failed_mask & (1U << i)) != 0;
    }

    _stale = false;

    if (resetModel) {
        endResetModel();
    } else if (_motorCount > 0) {
        emit dataChanged(index(0, 0), index(_motorCount - 1, 0));
    }
}

void FtcMotorStatusModel::setStale(bool stale)
{
    if (_stale == stale) {
        return;
    }

    _stale = stale;

    if (_motorCount > 0) {
        emit dataChanged(index(0, 0), index(_motorCount - 1, 0),
                         {AvailableRole, SeverityRole, HealthRole, EffectivenessRole,
                          UncertaintyRole, EstimateAgeRole, EstimateValidRole, DataStateTextRole});
    }
}

QString FtcMotorStatusModel::faultTypeText(uint8_t faultType)
{
    switch (faultType) {
    case MERIVUS_FTC_FAULT_NONE: return tr("无");
    case MERIVUS_FTC_FAULT_UNKNOWN_PROPULSION_DEGRADATION: return tr("推进系统异常");
    case MERIVUS_FTC_FAULT_PROP_DAMAGE: return tr("桨叶受损");
    case MERIVUS_FTC_FAULT_MOTOR_DEGRADATION: return tr("电机性能下降");
    case MERIVUS_FTC_FAULT_MOTOR_STOP: return tr("电机停转");
    case MERIVUS_FTC_FAULT_ESC_OR_POWER_FAILURE: return tr("电调或供电故障");
    case MERIVUS_FTC_FAULT_INTERMITTENT_PROPULSION_FAILURE: return tr("间歇性推进故障");
    case MERIVUS_FTC_FAULT_MECHANICAL_IMBALANCE: return tr("机械不平衡");
    case MERIVUS_FTC_FAULT_EXTERNAL_DISTURBANCE: return tr("外部扰动");
    case MERIVUS_FTC_FAULT_MODEL_MISMATCH: return tr("模型失配");
    default: return tr("未知");
    }
}

QVariant FtcMotorStatusModel::_roleValue(const MotorData& motor, int row, int role) const
{
    switch (role) {
    case MotorIndexRole: return row;
    case HealthRole: return motor.estimateValid && !_stale ? motor.health : -1.0;
    case EffectivenessRole: return motor.estimateValid && !_stale ? motor.effectiveness : -1.0;
    case FaultProbabilityRole: return motor.faultProbability;
    case ConfidenceRole: return motor.confidence;
    case FaultTypeRole: return motor.faultType;
    case FaultTypeTextRole: return faultTypeText(motor.faultType);
    case DegradedRole: return motor.degraded;
    case FailedRole: return motor.failed;
    case AvailableRole: return motor.available && !_stale;
    case SeverityRole: return _stale || !motor.available ? "unavailable" : (motor.failed ? "critical" : (motor.degraded || !motor.estimateValid || !motor.observable ? "warning" : "normal"));
    case UncertaintyRole: return _stale ? -1.0 : motor.uncertainty;
    case EstimateAgeRole: return _stale ? -1.0 : motor.estimateAge;
    case LastEffectivenessRole: return motor.effectiveness;
    case EstimateValidRole: return motor.estimateValid && !_stale;
    case DataStateTextRole:
        if (!motor.available) return tr("消息未收到");
        if (_stale) return tr("消息已过期");
        if (motor.diagnosisState == 5) return tr("故障");
        if (motor.diagnosisState == 4) return tr("退化");
        if (motor.estimatorState == 0) return tr("未初始化");
        if (motor.estimatorState == 1) return tr("学习中");
        if (motor.estimatorState == 6) return tr("历史估计过期");
        if (motor.estimatorState == 7) return tr("模型不可用");
        if (!motor.observable) return tr("当前不可观测");
        if (!motor.estimateValid) return tr("估计质量不足");
        return tr("有效");
    default: return {};
    }
}

double FtcMotorStatusModel::_decodePercentage(uint8_t value)
{
    return value == UINT8_MAX ? -1.0 : static_cast<double>(value) / 2.0;
}

VehicleFtcStatusFactGroup::VehicleFtcStatusFactGroup(QObject* parent)
    : FactGroup(0, parent)
    , _motors(this)
{
    _staleTimer.setInterval(500);
    connect(&_staleTimer, &QTimer::timeout, this, &VehicleFtcStatusFactGroup::_refreshStale);
    _staleTimer.start();
}

bool VehicleFtcStatusFactGroup::available() const
{
    return _protocolCompatible && (_motorReceived || _controlReceived || _extremeReceived || _diagnosticsReceived);
}

bool VehicleFtcStatusFactGroup::stale() const
{
    if (!available()) {
        return false;
    }

    return (!_motorReceived || _motorStale)
        && (!_controlReceived || _controlStale)
        && (!_extremeReceived || _extremeStale)
        && (!_diagnosticsReceived || _diagnosticsStale);
}

void VehicleFtcStatusFactGroup::handleMessage(Vehicle* /*vehicle*/, mavlink_message_t& message)
{
    if (message.msgid == MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS) {
        mavlink_merivus_ftc_motor_status_t content{};
        mavlink_msg_merivus_ftc_motor_status_decode(&message, &content);
        _markReceived(_motorReceivedAt, _motorReceived, _motorStale, 0, content.protocol_version);

        if (content.protocol_version == ProtocolVersion) {
            _motors.update(content);
            _motorCount = qMin<int>(content.motor_count, 12);
            _systemState = content.system_state;
            _enabled = (content.flags & MERIVUS_FTC_MOTOR_FLAGS_MONITOR_ENABLED) != 0;
            _modelQuality = _decodePercentage(content.model_quality_pct);
            _modelValid = (content.flags & MERIVUS_FTC_MOTOR_FLAGS_MODEL_VALID) != 0;
            _baselineLearned = content.baseline_learned != 0;
            _currentObservable = content.current_observable != 0;
            _estimatorState = content.estimator_state;
            _estimateAge = qIsFinite(content.estimate_age) ? content.estimate_age : -1.0;
        }
    } else if (message.msgid == MAVLINK_MSG_ID_MERIVUS_FTC_CONTROL_STATUS) {
        mavlink_merivus_ftc_control_status_t content{};
        mavlink_msg_merivus_ftc_control_status_decode(&message, &content);
        _markReceived(_controlReceivedAt, _controlReceived, _controlStale, 1, content.protocol_version);

        if (content.protocol_version == ProtocolVersion) {
            _systemState = content.system_state;
            _controlMode = content.control_mode;
            _authorityState = content.authority_state;
            _authorityValid = (content.flags & MERIVUS_FTC_CONTROL_FLAGS_AUTHORITY_VALID) != 0;
            QVariantList positive, negative;
            for (int axis = 0; axis < 3; ++axis) { positive.append(content.positive_authority[axis]); negative.append(content.negative_authority[axis]); }
            _details["positiveAuthority"] = positive;
            _details["negativeAuthority"] = negative;
            _details["thrustUp"] = content.thrust_up;
            _details["thrustDown"] = content.thrust_down;
            _details["reachableResidual"] = content.reachable_residual;
            _details["allocationFallback"] = content.allocation_fallback;
            _details["recoveryFallback"] = content.recovery_fallback;
            _details["arbitrationWeight"] = content.arbitration_weight;
            _details["reentryWeight"] = content.reentry_weight;
            _details["allocationActive"] = content.allocation_active;
            _details["recoveryActive"] = content.recovery_active;

            _recoveryState = content.recovery_state;
            _enabled = (content.flags & MERIVUS_FTC_CONTROL_FLAGS_MONITOR_ENABLED) != 0;
            _recoveryCandidate = (content.flags & MERIVUS_FTC_CONTROL_FLAGS_RECOVERY_CANDIDATE_VALID) != 0;
            _rollAuthority = _decodePercentage(content.roll_authority_pct);
            _pitchAuthority = _decodePercentage(content.pitch_authority_pct);
            _yawAuthority = _decodePercentage(content.yaw_authority_pct);
            _thrustAuthority = _decodePercentage(content.thrust_authority_pct);
            _minimumAttitudeAuthority = _decodePercentage(content.minimum_attitude_authority_pct);
            _actuatorHeadroom = _decodePercentage(content.actuator_headroom_pct);
            _systemConfidence = _decodePercentage(content.system_confidence_pct);
            _recoveryProgress = _decodePercentage(content.recovery_progress_pct);
            _saturatedMask = content.saturated_mask;
        }
    } else if (message.msgid == MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS) {
        mavlink_merivus_ftc_extreme_status_t content{};
        mavlink_msg_merivus_ftc_extreme_status_decode(&message, &content);
        _markReceived(_extremeReceivedAt, _extremeReceived, _extremeStale, 2, content.protocol_version);

        if (content.protocol_version == ProtocolVersion) {
            _impactType = content.impact_type;
            _locState = content.loc_state;
            _recoveryState = content.recovery_state;
            _recoveryCandidate = (content.flags & MERIVUS_FTC_EXTREME_FLAGS_RECOVERY_CANDIDATE_VALID) != 0;
            _impactScore = _decodePercentage(content.impact_score_pct);
            _impactConfidence = _decodePercentage(content.impact_confidence_pct);
            _impactSeverity = _decodePercentage(content.impact_severity_pct);
            _lossOfControlScore = _decodePercentage(content.loss_of_control_score_pct);
            _recoveryProgress = _decodePercentage(content.recovery_progress_pct);
            _lossOfControlReasonMask = content.loss_of_control_reason_mask;
            _recoveryTriggerMask = content.recovery_trigger_mask;
            _recoveryInhibitMask = content.recovery_inhibit_mask;
        }
    } else if (message.msgid == MAVLINK_MSG_ID_MERIVUS_FTC_DIAGNOSTICS) {
        mavlink_merivus_ftc_diagnostics_t content{};
        mavlink_msg_merivus_ftc_diagnostics_decode(&message, &content);
        _markReceived(_diagnosticsReceivedAt, _diagnosticsReceived, _diagnosticsStale, 3, content.protocol_version);

        if (content.protocol_version == ProtocolVersion) {
            _details["conditionNumber"] = content.condition_number;
            _details["rigidBodyActivity"] = content.rigid_body_activity;
            _details["predictionResidual"] = content.model_prediction_residual;
            _details["updateCount"] = content.update_count;
            _details["resetCount"] = content.reset_count;
            _details["mass"] = content.mass;
            _details["massState"] = content.mass_state;
            _details["inertiaState"] = content.inertia_state;
            _details["cgState"] = content.cg_state;
            _details["estimatorFlags"] = content.estimator_flags;
            _modelResidual = content.model_residual;
            _excitation = content.excitation;
            _maneuverIntensity = content.maneuver_intensity;
            _externalDisturbanceScore = content.external_disturbance_score;
            _vibrationScore = content.vibration_score;
            _allocationResidualNorm = content.allocation_residual_norm;
            _attitudeError = content.attitude_error;
            _rateError = content.rate_error;
            _jerk = content.jerk;
            _accelerationMagnitude = content.acceleration_magnitude;
            _angularRate = content.angular_rate;
            _angularAcceleration = content.angular_acceleration;
            _systemReasonMask = content.system_reason_mask;
            _simulationEnabled = (content.flags & MERIVUS_FTC_DIAGNOSTIC_FLAGS_SIMULATION_ENABLED) != 0;
            _simulationIntermittent = (content.flags & MERIVUS_FTC_DIAGNOSTIC_FLAGS_SIMULATION_INTERMITTENT) != 0;
            _simulationMotorIndex = _simulationEnabled ? content.simulation_motor_index : -1;
            _simulationTargetEffectiveness = _decodePercentage(content.simulation_target_effectiveness_pct);
            _simulationAppliedEffectiveness = _decodePercentage(content.simulation_applied_effectiveness_pct);
        }
    } else {
        return;
    }

    _setTelemetryAvailable(available());
    emit statusChanged();
}

void VehicleFtcStatusFactGroup::_markReceived(qint64& receivedAt, bool& received, bool& staleValue, unsigned stream, uint8_t version)
{
    receivedAt = QDateTime::currentMSecsSinceEpoch();
    received = true;
    staleValue = false;
    _lastUpdate = QDateTime::currentDateTimeUtc();
    _protocolVersion = version;
    _streamCompatible[stream] = version == ProtocolVersion;
    _protocolCompatible = true;
    for (bool compatible : _streamCompatible) { _protocolCompatible &= compatible; }
}

void VehicleFtcStatusFactGroup::_refreshStale()
{
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    const bool motorStale = _motorReceived && now - _motorReceivedAt > StaleTimeoutMs;
    const bool controlStale = _controlReceived && now - _controlReceivedAt > StaleTimeoutMs;
    const bool extremeStale = _extremeReceived && now - _extremeReceivedAt > StaleTimeoutMs;
    const bool diagnosticsStale = _diagnosticsReceived && now - _diagnosticsReceivedAt > StaleTimeoutMs;

    if (motorStale == _motorStale && controlStale == _controlStale
        && extremeStale == _extremeStale && diagnosticsStale == _diagnosticsStale) {
        return;
    }

    _motorStale = motorStale;
    _controlStale = controlStale;
    _extremeStale = extremeStale;
    _diagnosticsStale = diagnosticsStale;
    _motors.setStale(_motorStale);
    emit statusChanged();
}

double VehicleFtcStatusFactGroup::_decodePercentage(uint8_t value)
{
    return value == UINT8_MAX ? -1.0 : static_cast<double>(value) / 2.0;
}

QString VehicleFtcStatusFactGroup::modelStateText() const
{
    if (!motorAvailable()) return tr("消息未收到");
    if (motorStale()) return tr("消息已过期");
    switch (_estimatorState) {
    case 0: return tr("未初始化");
    case 1: return tr("学习中");
    case 2: return tr("基线已学习");
    case 3: return tr("可观测，估计质量不足");
    case 4: return tr("当前不可观测");
    case 5: return modelValid() ? tr("有效") : tr("模型不可用");
    case 6: return tr("历史估计过期");
    default: return tr("模型不可用");
    }
}

QString VehicleFtcStatusFactGroup::systemStateText() const
{
    if (!_protocolCompatible) return tr("协议不兼容");
    if (!available()) return tr("消息未收到");
    if ((!motorAvailable() || motorStale()) && (!controlAvailable() || controlStale())) return tr("消息已过期");
    if (_systemState == MERIVUS_FTC_SYSTEM_STATE_NORMAL && !modelValid()) return modelStateText();
    switch (_systemState) {
    case MERIVUS_FTC_SYSTEM_STATE_DISABLED: return tr("关闭");
    case MERIVUS_FTC_SYSTEM_STATE_INITIALIZING: return tr("初始化中");
    case MERIVUS_FTC_SYSTEM_STATE_CALIBRATING: return tr("学习中");
    case MERIVUS_FTC_SYSTEM_STATE_READY: return tr("模型就绪，等待控制裕度");
    case MERIVUS_FTC_SYSTEM_STATE_UNOBSERVABLE: return tr("当前不可观测");
    case MERIVUS_FTC_SYSTEM_STATE_SHADOW: return tr("影子分配");
    case MERIVUS_FTC_SYSTEM_STATE_RECOVERY_CANDIDATE: return tr("恢复候选");
    case MERIVUS_FTC_SYSTEM_STATE_ACTIVE_ALLOCATION: return tr("主动分配");
    case MERIVUS_FTC_SYSTEM_STATE_NORMAL: return tr("正常");
    case MERIVUS_FTC_SYSTEM_STATE_DEGRADED: return tr("性能下降");
    case MERIVUS_FTC_SYSTEM_STATE_FAULT_CONFIRMED: return tr("故障已确认");
    case MERIVUS_FTC_SYSTEM_STATE_RECOVERY_READY: return tr("恢复候选就绪");
    case MERIVUS_FTC_SYSTEM_STATE_RECOVERY_ACTIVE: return tr("主动恢复");
    case MERIVUS_FTC_SYSTEM_STATE_EMERGENCY_LAND: return tr("紧急降落");
    case MERIVUS_FTC_SYSTEM_STATE_FAILED: return tr("系统失败");
    default: return tr("未知");
    }
}

QString VehicleFtcStatusFactGroup::systemSeverity() const
{
    if ((!motorAvailable() || motorStale()) && (!controlAvailable() || controlStale())) return "unavailable";
    if (_systemState == MERIVUS_FTC_SYSTEM_STATE_FAILED || _controlMode == MERIVUS_FTC_CONTROL_MODE_ACTIVE) return "critical";
    if (!modelValid() || !_currentObservable) return "warning";
    if (_systemState == MERIVUS_FTC_SYSTEM_STATE_NORMAL || _systemState == MERIVUS_FTC_SYSTEM_STATE_SHADOW) return "normal";
    if (_systemState == MERIVUS_FTC_SYSTEM_STATE_DISABLED) return "unavailable";
    if (_systemState == MERIVUS_FTC_SYSTEM_STATE_DEGRADED || _systemState == MERIVUS_FTC_SYSTEM_STATE_RECOVERY_READY) return "warning";
    return "critical";
}

QString VehicleFtcStatusFactGroup::controlModeText() const
{
    if (!controlAvailable()) return tr("消息未收到");
    if (controlStale()) return tr("消息已过期");
    switch (_controlMode) {
    case MERIVUS_FTC_CONTROL_MODE_DISABLED: return tr("DISABLED · 关闭");
    case MERIVUS_FTC_CONTROL_MODE_OBSERVE: return tr("OBSERVE · 只观测");
    case MERIVUS_FTC_CONTROL_MODE_SHADOW: return tr("SHADOW · 影子分配");
    case MERIVUS_FTC_CONTROL_MODE_CANDIDATE: return tr("CANDIDATE · 恢复候选");
    case MERIVUS_FTC_CONTROL_MODE_ACTIVE: return tr("ACTIVE · 已接入控制");
    default: return tr("未知");
    }
}

QString VehicleFtcStatusFactGroup::controlModeSeverity() const
{
    if (!controlAvailable() || controlStale()) return "unavailable";
    if (!controlAvailable() || controlStale()) return "unavailable";
    if (_controlMode == MERIVUS_FTC_CONTROL_MODE_ACTIVE) return "critical";
    if (_controlMode == MERIVUS_FTC_CONTROL_MODE_CANDIDATE) return "warning";
    return _controlMode == MERIVUS_FTC_CONTROL_MODE_DISABLED ? "unavailable" : "normal";
}

QString VehicleFtcStatusFactGroup::authorityStateText() const
{
    if (!controlAvailable()) return tr("消息未收到");
    if (controlStale()) return tr("消息已过期");
    if (!_authorityValid) return tr("控制裕度不可用");
    switch (_authorityState) {
    case MERIVUS_FTC_AUTHORITY_STATE_FULL_CONTROL: return tr("控制裕度完整");
    case MERIVUS_FTC_AUTHORITY_STATE_DEGRADED_CONTROL: return tr("控制能力下降");
    case MERIVUS_FTC_AUTHORITY_STATE_YAW_UNCONTROLLABLE: return tr("偏航不可控");
    case MERIVUS_FTC_AUTHORITY_STATE_ATTITUDE_DEGRADED: return tr("姿态控制下降");
    case MERIVUS_FTC_AUTHORITY_STATE_THRUST_INSUFFICIENT: return tr("推力不足");
    case MERIVUS_FTC_AUTHORITY_STATE_RECOVERY_ONLY: return tr("仅保留恢复能力");
    case MERIVUS_FTC_AUTHORITY_STATE_UNCONTROLLABLE: return tr("不可控");
    default: return tr("未知");
    }
}

QString VehicleFtcStatusFactGroup::authoritySeverity() const
{
    if (!controlAvailable() || controlStale()) return "unavailable";
    if (!_authorityValid) return "warning";
    if (_authorityState == MERIVUS_FTC_AUTHORITY_STATE_FULL_CONTROL) return "normal";
    if (_authorityState <= MERIVUS_FTC_AUTHORITY_STATE_ATTITUDE_DEGRADED) return "warning";
    return "critical";
}

QString VehicleFtcStatusFactGroup::impactTypeText() const
{
    switch (_impactType) {
    case MERIVUS_FTC_IMPACT_TYPE_NONE: return tr("无撞击");
    case MERIVUS_FTC_IMPACT_TYPE_EXTERNAL: return tr("外部撞击");
    case MERIVUS_FTC_IMPACT_TYPE_HARD_LANDING: return tr("重着陆");
    default: return tr("未知");
    }
}

QString VehicleFtcStatusFactGroup::locStateText() const
{
    switch (_locState) {
    case MERIVUS_FTC_LOC_STATE_NORMAL: return tr("正常");
    case MERIVUS_FTC_LOC_STATE_DISTURBED: return tr("受扰");
    case MERIVUS_FTC_LOC_STATE_RECOVERY_RECOMMENDED: return tr("建议恢复");
    case MERIVUS_FTC_LOC_STATE_LOSS_OF_CONTROL: return tr("失控");
    case MERIVUS_FTC_LOC_STATE_UNRECOVERABLE: return tr("不可恢复");
    default: return tr("未知");
    }
}

QString VehicleFtcStatusFactGroup::locSeverity() const
{
    if (!extremeAvailable() || extremeStale()) return "unavailable";
    if (_locState == MERIVUS_FTC_LOC_STATE_NORMAL) return "normal";
    if (_locState <= MERIVUS_FTC_LOC_STATE_RECOVERY_RECOMMENDED) return "warning";
    return "critical";
}

QString VehicleFtcStatusFactGroup::recoveryStateText() const
{
    switch (_recoveryState) {
    case MERIVUS_FTC_RECOVERY_STATE_DISABLED: return tr("关闭");
    case MERIVUS_FTC_RECOVERY_STATE_MONITORING: return tr("监测");
    case MERIVUS_FTC_RECOVERY_STATE_DISTURBANCE_DETECTED: return tr("检测到强扰动");
    case MERIVUS_FTC_RECOVERY_STATE_RATE_DAMPING: return tr("角速度抑制");
    case MERIVUS_FTC_RECOVERY_STATE_THRUST_VECTOR_RECOVERY: return tr("推力方向恢复");
    case MERIVUS_FTC_RECOVERY_STATE_ATTITUDE_RECOVERY: return tr("姿态恢复");
    case MERIVUS_FTC_RECOVERY_STATE_VERTICAL_SPEED_RECOVERY: return tr("垂直速度恢复");
    case MERIVUS_FTC_RECOVERY_STATE_ALTITUDE_STABILIZATION: return tr("高度稳定");
    case MERIVUS_FTC_RECOVERY_STATE_CONTROL_REENTRY: return tr("恢复正常控制");
    case MERIVUS_FTC_RECOVERY_STATE_EMERGENCY_LAND: return tr("紧急降落");
    case MERIVUS_FTC_RECOVERY_STATE_ABORTED: return tr("已中止");
    case MERIVUS_FTC_RECOVERY_STATE_FAILED: return tr("恢复失败");
    default: return tr("未知");
    }
}

QString VehicleFtcStatusFactGroup::recoverySeverity() const
{
    return _severityForRecovery();
}

QString VehicleFtcStatusFactGroup::_severityForRecovery() const
{
    if ((!controlAvailable() || controlStale()) && (!extremeAvailable() || extremeStale())) return "unavailable";
    if (_recoveryState <= MERIVUS_FTC_RECOVERY_STATE_MONITORING) return "normal";
    if (_recoveryState == MERIVUS_FTC_RECOVERY_STATE_EMERGENCY_LAND || _recoveryState == MERIVUS_FTC_RECOVERY_STATE_FAILED) return "critical";
    return "warning";
}
