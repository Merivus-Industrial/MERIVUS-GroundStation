#pragma once
// MESSAGE MERIVUS_FTC_MOTOR_STATUS PACKING

#define MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS 60000

MAVPACKED(
typedef struct __mavlink_merivus_ftc_motor_status_t {
 uint64_t time_usec; /*< [us] PX4 monotonic publication timestamp.*/
 uint16_t degraded_mask; /*<  Bit i is set when motor i is degraded.*/
 uint16_t failed_mask; /*<  Bit i is set when motor i is failed.*/
 uint8_t health_pct[12]; /*<  Observed health score. This is not remaining life.*/
 uint8_t effectiveness_pct[12]; /*<  Observed relative actuator effectiveness.*/
 uint8_t fault_probability_pct[12]; /*<  Observed fault probability.*/
 uint8_t confidence_pct[12]; /*<  Observer confidence.*/
 uint8_t fault_type[12]; /*<  Observed fault classification.*/
 uint8_t protocol_version; /*<  MERIVUS FTC telemetry contract version.*/
 uint8_t system_state; /*<  Aggregated subsystem state.*/
 uint8_t monitor_state; /*<  Motor-health observer state.*/
 uint8_t motor_count; /*<  Number of valid motor entries, maximum 12.*/
 uint8_t flags; /*<  Validity and source flags.*/
 uint8_t model_quality_pct; /*<  Rigid-body model quality using the percentage scaling above.*/
 uint64_t last_valid_timestamp; /*<  Last accepted valid parameter update timestamp in microseconds.*/
 float estimate_age; /*<  Seconds since last parameter update; infinity before first update.*/
 float estimate_uncertainty[12]; /*<  One sigma relative-effectiveness uncertainty.*/
 uint8_t diagnosis_state[12]; /*<  0 no evidence, 1 unknown, 2 unobservable, 3 valid healthy, 4 degraded, 5 failed.*/
 uint8_t estimator_state; /*<  0 uninitialized, 1 calibrating, 2 baseline learned, 3 observable, 4 temporarily unobservable, 5 valid, 6 stale, 7 invalid.*/
 uint8_t baseline_learned; /*<  Healthy baseline was learned this session.*/
 uint8_t current_observable; /*<  Current aligned sample supplies independent information.*/
}) mavlink_merivus_ftc_motor_status_t;

#define MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_LEN 153
#define MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_MIN_LEN 78
#define MAVLINK_MSG_ID_60000_LEN 153
#define MAVLINK_MSG_ID_60000_MIN_LEN 78

#define MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_CRC 29
#define MAVLINK_MSG_ID_60000_CRC 29

#define MAVLINK_MSG_MERIVUS_FTC_MOTOR_STATUS_FIELD_HEALTH_PCT_LEN 12
#define MAVLINK_MSG_MERIVUS_FTC_MOTOR_STATUS_FIELD_EFFECTIVENESS_PCT_LEN 12
#define MAVLINK_MSG_MERIVUS_FTC_MOTOR_STATUS_FIELD_FAULT_PROBABILITY_PCT_LEN 12
#define MAVLINK_MSG_MERIVUS_FTC_MOTOR_STATUS_FIELD_CONFIDENCE_PCT_LEN 12
#define MAVLINK_MSG_MERIVUS_FTC_MOTOR_STATUS_FIELD_FAULT_TYPE_LEN 12
#define MAVLINK_MSG_MERIVUS_FTC_MOTOR_STATUS_FIELD_ESTIMATE_UNCERTAINTY_LEN 12
#define MAVLINK_MSG_MERIVUS_FTC_MOTOR_STATUS_FIELD_DIAGNOSIS_STATE_LEN 12

#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_MERIVUS_FTC_MOTOR_STATUS { \
    60000, \
    "MERIVUS_FTC_MOTOR_STATUS", \
    21, \
    {  { "time_usec", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_merivus_ftc_motor_status_t, time_usec) }, \
         { "degraded_mask", NULL, MAVLINK_TYPE_UINT16_T, 0, 8, offsetof(mavlink_merivus_ftc_motor_status_t, degraded_mask) }, \
         { "failed_mask", NULL, MAVLINK_TYPE_UINT16_T, 0, 10, offsetof(mavlink_merivus_ftc_motor_status_t, failed_mask) }, \
         { "health_pct", NULL, MAVLINK_TYPE_UINT8_T, 12, 12, offsetof(mavlink_merivus_ftc_motor_status_t, health_pct) }, \
         { "effectiveness_pct", NULL, MAVLINK_TYPE_UINT8_T, 12, 24, offsetof(mavlink_merivus_ftc_motor_status_t, effectiveness_pct) }, \
         { "fault_probability_pct", NULL, MAVLINK_TYPE_UINT8_T, 12, 36, offsetof(mavlink_merivus_ftc_motor_status_t, fault_probability_pct) }, \
         { "confidence_pct", NULL, MAVLINK_TYPE_UINT8_T, 12, 48, offsetof(mavlink_merivus_ftc_motor_status_t, confidence_pct) }, \
         { "fault_type", NULL, MAVLINK_TYPE_UINT8_T, 12, 60, offsetof(mavlink_merivus_ftc_motor_status_t, fault_type) }, \
         { "protocol_version", NULL, MAVLINK_TYPE_UINT8_T, 0, 72, offsetof(mavlink_merivus_ftc_motor_status_t, protocol_version) }, \
         { "system_state", NULL, MAVLINK_TYPE_UINT8_T, 0, 73, offsetof(mavlink_merivus_ftc_motor_status_t, system_state) }, \
         { "monitor_state", NULL, MAVLINK_TYPE_UINT8_T, 0, 74, offsetof(mavlink_merivus_ftc_motor_status_t, monitor_state) }, \
         { "motor_count", NULL, MAVLINK_TYPE_UINT8_T, 0, 75, offsetof(mavlink_merivus_ftc_motor_status_t, motor_count) }, \
         { "flags", NULL, MAVLINK_TYPE_UINT8_T, 0, 76, offsetof(mavlink_merivus_ftc_motor_status_t, flags) }, \
         { "model_quality_pct", NULL, MAVLINK_TYPE_UINT8_T, 0, 77, offsetof(mavlink_merivus_ftc_motor_status_t, model_quality_pct) }, \
         { "last_valid_timestamp", NULL, MAVLINK_TYPE_UINT64_T, 0, 78, offsetof(mavlink_merivus_ftc_motor_status_t, last_valid_timestamp) }, \
         { "estimate_age", NULL, MAVLINK_TYPE_FLOAT, 0, 86, offsetof(mavlink_merivus_ftc_motor_status_t, estimate_age) }, \
         { "estimate_uncertainty", NULL, MAVLINK_TYPE_FLOAT, 12, 90, offsetof(mavlink_merivus_ftc_motor_status_t, estimate_uncertainty) }, \
         { "diagnosis_state", NULL, MAVLINK_TYPE_UINT8_T, 12, 138, offsetof(mavlink_merivus_ftc_motor_status_t, diagnosis_state) }, \
         { "estimator_state", NULL, MAVLINK_TYPE_UINT8_T, 0, 150, offsetof(mavlink_merivus_ftc_motor_status_t, estimator_state) }, \
         { "baseline_learned", NULL, MAVLINK_TYPE_UINT8_T, 0, 151, offsetof(mavlink_merivus_ftc_motor_status_t, baseline_learned) }, \
         { "current_observable", NULL, MAVLINK_TYPE_UINT8_T, 0, 152, offsetof(mavlink_merivus_ftc_motor_status_t, current_observable) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_MERIVUS_FTC_MOTOR_STATUS { \
    "MERIVUS_FTC_MOTOR_STATUS", \
    21, \
    {  { "time_usec", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_merivus_ftc_motor_status_t, time_usec) }, \
         { "degraded_mask", NULL, MAVLINK_TYPE_UINT16_T, 0, 8, offsetof(mavlink_merivus_ftc_motor_status_t, degraded_mask) }, \
         { "failed_mask", NULL, MAVLINK_TYPE_UINT16_T, 0, 10, offsetof(mavlink_merivus_ftc_motor_status_t, failed_mask) }, \
         { "health_pct", NULL, MAVLINK_TYPE_UINT8_T, 12, 12, offsetof(mavlink_merivus_ftc_motor_status_t, health_pct) }, \
         { "effectiveness_pct", NULL, MAVLINK_TYPE_UINT8_T, 12, 24, offsetof(mavlink_merivus_ftc_motor_status_t, effectiveness_pct) }, \
         { "fault_probability_pct", NULL, MAVLINK_TYPE_UINT8_T, 12, 36, offsetof(mavlink_merivus_ftc_motor_status_t, fault_probability_pct) }, \
         { "confidence_pct", NULL, MAVLINK_TYPE_UINT8_T, 12, 48, offsetof(mavlink_merivus_ftc_motor_status_t, confidence_pct) }, \
         { "fault_type", NULL, MAVLINK_TYPE_UINT8_T, 12, 60, offsetof(mavlink_merivus_ftc_motor_status_t, fault_type) }, \
         { "protocol_version", NULL, MAVLINK_TYPE_UINT8_T, 0, 72, offsetof(mavlink_merivus_ftc_motor_status_t, protocol_version) }, \
         { "system_state", NULL, MAVLINK_TYPE_UINT8_T, 0, 73, offsetof(mavlink_merivus_ftc_motor_status_t, system_state) }, \
         { "monitor_state", NULL, MAVLINK_TYPE_UINT8_T, 0, 74, offsetof(mavlink_merivus_ftc_motor_status_t, monitor_state) }, \
         { "motor_count", NULL, MAVLINK_TYPE_UINT8_T, 0, 75, offsetof(mavlink_merivus_ftc_motor_status_t, motor_count) }, \
         { "flags", NULL, MAVLINK_TYPE_UINT8_T, 0, 76, offsetof(mavlink_merivus_ftc_motor_status_t, flags) }, \
         { "model_quality_pct", NULL, MAVLINK_TYPE_UINT8_T, 0, 77, offsetof(mavlink_merivus_ftc_motor_status_t, model_quality_pct) }, \
         { "last_valid_timestamp", NULL, MAVLINK_TYPE_UINT64_T, 0, 78, offsetof(mavlink_merivus_ftc_motor_status_t, last_valid_timestamp) }, \
         { "estimate_age", NULL, MAVLINK_TYPE_FLOAT, 0, 86, offsetof(mavlink_merivus_ftc_motor_status_t, estimate_age) }, \
         { "estimate_uncertainty", NULL, MAVLINK_TYPE_FLOAT, 12, 90, offsetof(mavlink_merivus_ftc_motor_status_t, estimate_uncertainty) }, \
         { "diagnosis_state", NULL, MAVLINK_TYPE_UINT8_T, 12, 138, offsetof(mavlink_merivus_ftc_motor_status_t, diagnosis_state) }, \
         { "estimator_state", NULL, MAVLINK_TYPE_UINT8_T, 0, 150, offsetof(mavlink_merivus_ftc_motor_status_t, estimator_state) }, \
         { "baseline_learned", NULL, MAVLINK_TYPE_UINT8_T, 0, 151, offsetof(mavlink_merivus_ftc_motor_status_t, baseline_learned) }, \
         { "current_observable", NULL, MAVLINK_TYPE_UINT8_T, 0, 152, offsetof(mavlink_merivus_ftc_motor_status_t, current_observable) }, \
         } \
}
#endif

/**
 * @brief Pack a merivus_ftc_motor_status message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_usec [us] PX4 monotonic publication timestamp.
 * @param degraded_mask  Bit i is set when motor i is degraded.
 * @param failed_mask  Bit i is set when motor i is failed.
 * @param health_pct  Observed health score. This is not remaining life.
 * @param effectiveness_pct  Observed relative actuator effectiveness.
 * @param fault_probability_pct  Observed fault probability.
 * @param confidence_pct  Observer confidence.
 * @param fault_type  Observed fault classification.
 * @param protocol_version  MERIVUS FTC telemetry contract version.
 * @param system_state  Aggregated subsystem state.
 * @param monitor_state  Motor-health observer state.
 * @param motor_count  Number of valid motor entries, maximum 12.
 * @param flags  Validity and source flags.
 * @param model_quality_pct  Rigid-body model quality using the percentage scaling above.
 * @param last_valid_timestamp  Last accepted valid parameter update timestamp in microseconds.
 * @param estimate_age  Seconds since last parameter update; infinity before first update.
 * @param estimate_uncertainty  One sigma relative-effectiveness uncertainty.
 * @param diagnosis_state  0 no evidence, 1 unknown, 2 unobservable, 3 valid healthy, 4 degraded, 5 failed.
 * @param estimator_state  0 uninitialized, 1 calibrating, 2 baseline learned, 3 observable, 4 temporarily unobservable, 5 valid, 6 stale, 7 invalid.
 * @param baseline_learned  Healthy baseline was learned this session.
 * @param current_observable  Current aligned sample supplies independent information.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_merivus_ftc_motor_status_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint64_t time_usec, uint16_t degraded_mask, uint16_t failed_mask, const uint8_t *health_pct, const uint8_t *effectiveness_pct, const uint8_t *fault_probability_pct, const uint8_t *confidence_pct, const uint8_t *fault_type, uint8_t protocol_version, uint8_t system_state, uint8_t monitor_state, uint8_t motor_count, uint8_t flags, uint8_t model_quality_pct, uint64_t last_valid_timestamp, float estimate_age, const float *estimate_uncertainty, const uint8_t *diagnosis_state, uint8_t estimator_state, uint8_t baseline_learned, uint8_t current_observable)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_LEN];
    _mav_put_uint64_t(buf, 0, time_usec);
    _mav_put_uint16_t(buf, 8, degraded_mask);
    _mav_put_uint16_t(buf, 10, failed_mask);
    _mav_put_uint8_t(buf, 72, protocol_version);
    _mav_put_uint8_t(buf, 73, system_state);
    _mav_put_uint8_t(buf, 74, monitor_state);
    _mav_put_uint8_t(buf, 75, motor_count);
    _mav_put_uint8_t(buf, 76, flags);
    _mav_put_uint8_t(buf, 77, model_quality_pct);
    _mav_put_uint64_t(buf, 78, last_valid_timestamp);
    _mav_put_float(buf, 86, estimate_age);
    _mav_put_uint8_t(buf, 150, estimator_state);
    _mav_put_uint8_t(buf, 151, baseline_learned);
    _mav_put_uint8_t(buf, 152, current_observable);
    _mav_put_uint8_t_array(buf, 12, health_pct, 12);
    _mav_put_uint8_t_array(buf, 24, effectiveness_pct, 12);
    _mav_put_uint8_t_array(buf, 36, fault_probability_pct, 12);
    _mav_put_uint8_t_array(buf, 48, confidence_pct, 12);
    _mav_put_uint8_t_array(buf, 60, fault_type, 12);
    _mav_put_float_array(buf, 90, estimate_uncertainty, 12);
    _mav_put_uint8_t_array(buf, 138, diagnosis_state, 12);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_LEN);
#else
    mavlink_merivus_ftc_motor_status_t packet;
    packet.time_usec = time_usec;
    packet.degraded_mask = degraded_mask;
    packet.failed_mask = failed_mask;
    packet.protocol_version = protocol_version;
    packet.system_state = system_state;
    packet.monitor_state = monitor_state;
    packet.motor_count = motor_count;
    packet.flags = flags;
    packet.model_quality_pct = model_quality_pct;
    packet.last_valid_timestamp = last_valid_timestamp;
    packet.estimate_age = estimate_age;
    packet.estimator_state = estimator_state;
    packet.baseline_learned = baseline_learned;
    packet.current_observable = current_observable;
    mav_array_memcpy(packet.health_pct, health_pct, sizeof(uint8_t)*12);
    mav_array_memcpy(packet.effectiveness_pct, effectiveness_pct, sizeof(uint8_t)*12);
    mav_array_memcpy(packet.fault_probability_pct, fault_probability_pct, sizeof(uint8_t)*12);
    mav_array_memcpy(packet.confidence_pct, confidence_pct, sizeof(uint8_t)*12);
    mav_array_memcpy(packet.fault_type, fault_type, sizeof(uint8_t)*12);
    mav_array_memcpy(packet.estimate_uncertainty, estimate_uncertainty, sizeof(float)*12);
    mav_array_memcpy(packet.diagnosis_state, diagnosis_state, sizeof(uint8_t)*12);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_MIN_LEN, MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_LEN, MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_CRC);
}

/**
 * @brief Pack a merivus_ftc_motor_status message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param time_usec [us] PX4 monotonic publication timestamp.
 * @param degraded_mask  Bit i is set when motor i is degraded.
 * @param failed_mask  Bit i is set when motor i is failed.
 * @param health_pct  Observed health score. This is not remaining life.
 * @param effectiveness_pct  Observed relative actuator effectiveness.
 * @param fault_probability_pct  Observed fault probability.
 * @param confidence_pct  Observer confidence.
 * @param fault_type  Observed fault classification.
 * @param protocol_version  MERIVUS FTC telemetry contract version.
 * @param system_state  Aggregated subsystem state.
 * @param monitor_state  Motor-health observer state.
 * @param motor_count  Number of valid motor entries, maximum 12.
 * @param flags  Validity and source flags.
 * @param model_quality_pct  Rigid-body model quality using the percentage scaling above.
 * @param last_valid_timestamp  Last accepted valid parameter update timestamp in microseconds.
 * @param estimate_age  Seconds since last parameter update; infinity before first update.
 * @param estimate_uncertainty  One sigma relative-effectiveness uncertainty.
 * @param diagnosis_state  0 no evidence, 1 unknown, 2 unobservable, 3 valid healthy, 4 degraded, 5 failed.
 * @param estimator_state  0 uninitialized, 1 calibrating, 2 baseline learned, 3 observable, 4 temporarily unobservable, 5 valid, 6 stale, 7 invalid.
 * @param baseline_learned  Healthy baseline was learned this session.
 * @param current_observable  Current aligned sample supplies independent information.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_merivus_ftc_motor_status_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint64_t time_usec,uint16_t degraded_mask,uint16_t failed_mask,const uint8_t *health_pct,const uint8_t *effectiveness_pct,const uint8_t *fault_probability_pct,const uint8_t *confidence_pct,const uint8_t *fault_type,uint8_t protocol_version,uint8_t system_state,uint8_t monitor_state,uint8_t motor_count,uint8_t flags,uint8_t model_quality_pct,uint64_t last_valid_timestamp,float estimate_age,const float *estimate_uncertainty,const uint8_t *diagnosis_state,uint8_t estimator_state,uint8_t baseline_learned,uint8_t current_observable)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_LEN];
    _mav_put_uint64_t(buf, 0, time_usec);
    _mav_put_uint16_t(buf, 8, degraded_mask);
    _mav_put_uint16_t(buf, 10, failed_mask);
    _mav_put_uint8_t(buf, 72, protocol_version);
    _mav_put_uint8_t(buf, 73, system_state);
    _mav_put_uint8_t(buf, 74, monitor_state);
    _mav_put_uint8_t(buf, 75, motor_count);
    _mav_put_uint8_t(buf, 76, flags);
    _mav_put_uint8_t(buf, 77, model_quality_pct);
    _mav_put_uint64_t(buf, 78, last_valid_timestamp);
    _mav_put_float(buf, 86, estimate_age);
    _mav_put_uint8_t(buf, 150, estimator_state);
    _mav_put_uint8_t(buf, 151, baseline_learned);
    _mav_put_uint8_t(buf, 152, current_observable);
    _mav_put_uint8_t_array(buf, 12, health_pct, 12);
    _mav_put_uint8_t_array(buf, 24, effectiveness_pct, 12);
    _mav_put_uint8_t_array(buf, 36, fault_probability_pct, 12);
    _mav_put_uint8_t_array(buf, 48, confidence_pct, 12);
    _mav_put_uint8_t_array(buf, 60, fault_type, 12);
    _mav_put_float_array(buf, 90, estimate_uncertainty, 12);
    _mav_put_uint8_t_array(buf, 138, diagnosis_state, 12);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_LEN);
#else
    mavlink_merivus_ftc_motor_status_t packet;
    packet.time_usec = time_usec;
    packet.degraded_mask = degraded_mask;
    packet.failed_mask = failed_mask;
    packet.protocol_version = protocol_version;
    packet.system_state = system_state;
    packet.monitor_state = monitor_state;
    packet.motor_count = motor_count;
    packet.flags = flags;
    packet.model_quality_pct = model_quality_pct;
    packet.last_valid_timestamp = last_valid_timestamp;
    packet.estimate_age = estimate_age;
    packet.estimator_state = estimator_state;
    packet.baseline_learned = baseline_learned;
    packet.current_observable = current_observable;
    mav_array_memcpy(packet.health_pct, health_pct, sizeof(uint8_t)*12);
    mav_array_memcpy(packet.effectiveness_pct, effectiveness_pct, sizeof(uint8_t)*12);
    mav_array_memcpy(packet.fault_probability_pct, fault_probability_pct, sizeof(uint8_t)*12);
    mav_array_memcpy(packet.confidence_pct, confidence_pct, sizeof(uint8_t)*12);
    mav_array_memcpy(packet.fault_type, fault_type, sizeof(uint8_t)*12);
    mav_array_memcpy(packet.estimate_uncertainty, estimate_uncertainty, sizeof(float)*12);
    mav_array_memcpy(packet.diagnosis_state, diagnosis_state, sizeof(uint8_t)*12);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_MIN_LEN, MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_LEN, MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_CRC);
}

/**
 * @brief Encode a merivus_ftc_motor_status struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param merivus_ftc_motor_status C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_merivus_ftc_motor_status_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_merivus_ftc_motor_status_t* merivus_ftc_motor_status)
{
    return mavlink_msg_merivus_ftc_motor_status_pack(system_id, component_id, msg, merivus_ftc_motor_status->time_usec, merivus_ftc_motor_status->degraded_mask, merivus_ftc_motor_status->failed_mask, merivus_ftc_motor_status->health_pct, merivus_ftc_motor_status->effectiveness_pct, merivus_ftc_motor_status->fault_probability_pct, merivus_ftc_motor_status->confidence_pct, merivus_ftc_motor_status->fault_type, merivus_ftc_motor_status->protocol_version, merivus_ftc_motor_status->system_state, merivus_ftc_motor_status->monitor_state, merivus_ftc_motor_status->motor_count, merivus_ftc_motor_status->flags, merivus_ftc_motor_status->model_quality_pct, merivus_ftc_motor_status->last_valid_timestamp, merivus_ftc_motor_status->estimate_age, merivus_ftc_motor_status->estimate_uncertainty, merivus_ftc_motor_status->diagnosis_state, merivus_ftc_motor_status->estimator_state, merivus_ftc_motor_status->baseline_learned, merivus_ftc_motor_status->current_observable);
}

/**
 * @brief Encode a merivus_ftc_motor_status struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param merivus_ftc_motor_status C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_merivus_ftc_motor_status_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_merivus_ftc_motor_status_t* merivus_ftc_motor_status)
{
    return mavlink_msg_merivus_ftc_motor_status_pack_chan(system_id, component_id, chan, msg, merivus_ftc_motor_status->time_usec, merivus_ftc_motor_status->degraded_mask, merivus_ftc_motor_status->failed_mask, merivus_ftc_motor_status->health_pct, merivus_ftc_motor_status->effectiveness_pct, merivus_ftc_motor_status->fault_probability_pct, merivus_ftc_motor_status->confidence_pct, merivus_ftc_motor_status->fault_type, merivus_ftc_motor_status->protocol_version, merivus_ftc_motor_status->system_state, merivus_ftc_motor_status->monitor_state, merivus_ftc_motor_status->motor_count, merivus_ftc_motor_status->flags, merivus_ftc_motor_status->model_quality_pct, merivus_ftc_motor_status->last_valid_timestamp, merivus_ftc_motor_status->estimate_age, merivus_ftc_motor_status->estimate_uncertainty, merivus_ftc_motor_status->diagnosis_state, merivus_ftc_motor_status->estimator_state, merivus_ftc_motor_status->baseline_learned, merivus_ftc_motor_status->current_observable);
}

/**
 * @brief Send a merivus_ftc_motor_status message
 * @param chan MAVLink channel to send the message
 *
 * @param time_usec [us] PX4 monotonic publication timestamp.
 * @param degraded_mask  Bit i is set when motor i is degraded.
 * @param failed_mask  Bit i is set when motor i is failed.
 * @param health_pct  Observed health score. This is not remaining life.
 * @param effectiveness_pct  Observed relative actuator effectiveness.
 * @param fault_probability_pct  Observed fault probability.
 * @param confidence_pct  Observer confidence.
 * @param fault_type  Observed fault classification.
 * @param protocol_version  MERIVUS FTC telemetry contract version.
 * @param system_state  Aggregated subsystem state.
 * @param monitor_state  Motor-health observer state.
 * @param motor_count  Number of valid motor entries, maximum 12.
 * @param flags  Validity and source flags.
 * @param model_quality_pct  Rigid-body model quality using the percentage scaling above.
 * @param last_valid_timestamp  Last accepted valid parameter update timestamp in microseconds.
 * @param estimate_age  Seconds since last parameter update; infinity before first update.
 * @param estimate_uncertainty  One sigma relative-effectiveness uncertainty.
 * @param diagnosis_state  0 no evidence, 1 unknown, 2 unobservable, 3 valid healthy, 4 degraded, 5 failed.
 * @param estimator_state  0 uninitialized, 1 calibrating, 2 baseline learned, 3 observable, 4 temporarily unobservable, 5 valid, 6 stale, 7 invalid.
 * @param baseline_learned  Healthy baseline was learned this session.
 * @param current_observable  Current aligned sample supplies independent information.
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_merivus_ftc_motor_status_send(mavlink_channel_t chan, uint64_t time_usec, uint16_t degraded_mask, uint16_t failed_mask, const uint8_t *health_pct, const uint8_t *effectiveness_pct, const uint8_t *fault_probability_pct, const uint8_t *confidence_pct, const uint8_t *fault_type, uint8_t protocol_version, uint8_t system_state, uint8_t monitor_state, uint8_t motor_count, uint8_t flags, uint8_t model_quality_pct, uint64_t last_valid_timestamp, float estimate_age, const float *estimate_uncertainty, const uint8_t *diagnosis_state, uint8_t estimator_state, uint8_t baseline_learned, uint8_t current_observable)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_LEN];
    _mav_put_uint64_t(buf, 0, time_usec);
    _mav_put_uint16_t(buf, 8, degraded_mask);
    _mav_put_uint16_t(buf, 10, failed_mask);
    _mav_put_uint8_t(buf, 72, protocol_version);
    _mav_put_uint8_t(buf, 73, system_state);
    _mav_put_uint8_t(buf, 74, monitor_state);
    _mav_put_uint8_t(buf, 75, motor_count);
    _mav_put_uint8_t(buf, 76, flags);
    _mav_put_uint8_t(buf, 77, model_quality_pct);
    _mav_put_uint64_t(buf, 78, last_valid_timestamp);
    _mav_put_float(buf, 86, estimate_age);
    _mav_put_uint8_t(buf, 150, estimator_state);
    _mav_put_uint8_t(buf, 151, baseline_learned);
    _mav_put_uint8_t(buf, 152, current_observable);
    _mav_put_uint8_t_array(buf, 12, health_pct, 12);
    _mav_put_uint8_t_array(buf, 24, effectiveness_pct, 12);
    _mav_put_uint8_t_array(buf, 36, fault_probability_pct, 12);
    _mav_put_uint8_t_array(buf, 48, confidence_pct, 12);
    _mav_put_uint8_t_array(buf, 60, fault_type, 12);
    _mav_put_float_array(buf, 90, estimate_uncertainty, 12);
    _mav_put_uint8_t_array(buf, 138, diagnosis_state, 12);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS, buf, MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_MIN_LEN, MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_LEN, MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_CRC);
#else
    mavlink_merivus_ftc_motor_status_t packet;
    packet.time_usec = time_usec;
    packet.degraded_mask = degraded_mask;
    packet.failed_mask = failed_mask;
    packet.protocol_version = protocol_version;
    packet.system_state = system_state;
    packet.monitor_state = monitor_state;
    packet.motor_count = motor_count;
    packet.flags = flags;
    packet.model_quality_pct = model_quality_pct;
    packet.last_valid_timestamp = last_valid_timestamp;
    packet.estimate_age = estimate_age;
    packet.estimator_state = estimator_state;
    packet.baseline_learned = baseline_learned;
    packet.current_observable = current_observable;
    mav_array_memcpy(packet.health_pct, health_pct, sizeof(uint8_t)*12);
    mav_array_memcpy(packet.effectiveness_pct, effectiveness_pct, sizeof(uint8_t)*12);
    mav_array_memcpy(packet.fault_probability_pct, fault_probability_pct, sizeof(uint8_t)*12);
    mav_array_memcpy(packet.confidence_pct, confidence_pct, sizeof(uint8_t)*12);
    mav_array_memcpy(packet.fault_type, fault_type, sizeof(uint8_t)*12);
    mav_array_memcpy(packet.estimate_uncertainty, estimate_uncertainty, sizeof(float)*12);
    mav_array_memcpy(packet.diagnosis_state, diagnosis_state, sizeof(uint8_t)*12);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS, (const char *)&packet, MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_MIN_LEN, MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_LEN, MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_CRC);
#endif
}

/**
 * @brief Send a merivus_ftc_motor_status message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_merivus_ftc_motor_status_send_struct(mavlink_channel_t chan, const mavlink_merivus_ftc_motor_status_t* merivus_ftc_motor_status)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_merivus_ftc_motor_status_send(chan, merivus_ftc_motor_status->time_usec, merivus_ftc_motor_status->degraded_mask, merivus_ftc_motor_status->failed_mask, merivus_ftc_motor_status->health_pct, merivus_ftc_motor_status->effectiveness_pct, merivus_ftc_motor_status->fault_probability_pct, merivus_ftc_motor_status->confidence_pct, merivus_ftc_motor_status->fault_type, merivus_ftc_motor_status->protocol_version, merivus_ftc_motor_status->system_state, merivus_ftc_motor_status->monitor_state, merivus_ftc_motor_status->motor_count, merivus_ftc_motor_status->flags, merivus_ftc_motor_status->model_quality_pct, merivus_ftc_motor_status->last_valid_timestamp, merivus_ftc_motor_status->estimate_age, merivus_ftc_motor_status->estimate_uncertainty, merivus_ftc_motor_status->diagnosis_state, merivus_ftc_motor_status->estimator_state, merivus_ftc_motor_status->baseline_learned, merivus_ftc_motor_status->current_observable);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS, (const char *)merivus_ftc_motor_status, MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_MIN_LEN, MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_LEN, MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_CRC);
#endif
}

#if MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This variant of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_merivus_ftc_motor_status_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint64_t time_usec, uint16_t degraded_mask, uint16_t failed_mask, const uint8_t *health_pct, const uint8_t *effectiveness_pct, const uint8_t *fault_probability_pct, const uint8_t *confidence_pct, const uint8_t *fault_type, uint8_t protocol_version, uint8_t system_state, uint8_t monitor_state, uint8_t motor_count, uint8_t flags, uint8_t model_quality_pct, uint64_t last_valid_timestamp, float estimate_age, const float *estimate_uncertainty, const uint8_t *diagnosis_state, uint8_t estimator_state, uint8_t baseline_learned, uint8_t current_observable)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint64_t(buf, 0, time_usec);
    _mav_put_uint16_t(buf, 8, degraded_mask);
    _mav_put_uint16_t(buf, 10, failed_mask);
    _mav_put_uint8_t(buf, 72, protocol_version);
    _mav_put_uint8_t(buf, 73, system_state);
    _mav_put_uint8_t(buf, 74, monitor_state);
    _mav_put_uint8_t(buf, 75, motor_count);
    _mav_put_uint8_t(buf, 76, flags);
    _mav_put_uint8_t(buf, 77, model_quality_pct);
    _mav_put_uint64_t(buf, 78, last_valid_timestamp);
    _mav_put_float(buf, 86, estimate_age);
    _mav_put_uint8_t(buf, 150, estimator_state);
    _mav_put_uint8_t(buf, 151, baseline_learned);
    _mav_put_uint8_t(buf, 152, current_observable);
    _mav_put_uint8_t_array(buf, 12, health_pct, 12);
    _mav_put_uint8_t_array(buf, 24, effectiveness_pct, 12);
    _mav_put_uint8_t_array(buf, 36, fault_probability_pct, 12);
    _mav_put_uint8_t_array(buf, 48, confidence_pct, 12);
    _mav_put_uint8_t_array(buf, 60, fault_type, 12);
    _mav_put_float_array(buf, 90, estimate_uncertainty, 12);
    _mav_put_uint8_t_array(buf, 138, diagnosis_state, 12);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS, buf, MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_MIN_LEN, MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_LEN, MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_CRC);
#else
    mavlink_merivus_ftc_motor_status_t *packet = (mavlink_merivus_ftc_motor_status_t *)msgbuf;
    packet->time_usec = time_usec;
    packet->degraded_mask = degraded_mask;
    packet->failed_mask = failed_mask;
    packet->protocol_version = protocol_version;
    packet->system_state = system_state;
    packet->monitor_state = monitor_state;
    packet->motor_count = motor_count;
    packet->flags = flags;
    packet->model_quality_pct = model_quality_pct;
    packet->last_valid_timestamp = last_valid_timestamp;
    packet->estimate_age = estimate_age;
    packet->estimator_state = estimator_state;
    packet->baseline_learned = baseline_learned;
    packet->current_observable = current_observable;
    mav_array_memcpy(packet->health_pct, health_pct, sizeof(uint8_t)*12);
    mav_array_memcpy(packet->effectiveness_pct, effectiveness_pct, sizeof(uint8_t)*12);
    mav_array_memcpy(packet->fault_probability_pct, fault_probability_pct, sizeof(uint8_t)*12);
    mav_array_memcpy(packet->confidence_pct, confidence_pct, sizeof(uint8_t)*12);
    mav_array_memcpy(packet->fault_type, fault_type, sizeof(uint8_t)*12);
    mav_array_memcpy(packet->estimate_uncertainty, estimate_uncertainty, sizeof(float)*12);
    mav_array_memcpy(packet->diagnosis_state, diagnosis_state, sizeof(uint8_t)*12);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS, (const char *)packet, MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_MIN_LEN, MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_LEN, MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_CRC);
#endif
}
#endif

#endif

// MESSAGE MERIVUS_FTC_MOTOR_STATUS UNPACKING


/**
 * @brief Get field time_usec from merivus_ftc_motor_status message
 *
 * @return [us] PX4 monotonic publication timestamp.
 */
static inline uint64_t mavlink_msg_merivus_ftc_motor_status_get_time_usec(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint64_t(msg,  0);
}

/**
 * @brief Get field degraded_mask from merivus_ftc_motor_status message
 *
 * @return  Bit i is set when motor i is degraded.
 */
static inline uint16_t mavlink_msg_merivus_ftc_motor_status_get_degraded_mask(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint16_t(msg,  8);
}

/**
 * @brief Get field failed_mask from merivus_ftc_motor_status message
 *
 * @return  Bit i is set when motor i is failed.
 */
static inline uint16_t mavlink_msg_merivus_ftc_motor_status_get_failed_mask(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint16_t(msg,  10);
}

/**
 * @brief Get field health_pct from merivus_ftc_motor_status message
 *
 * @return  Observed health score. This is not remaining life.
 */
static inline uint16_t mavlink_msg_merivus_ftc_motor_status_get_health_pct(const mavlink_message_t* msg, uint8_t *health_pct)
{
    return _MAV_RETURN_uint8_t_array(msg, health_pct, 12,  12);
}

/**
 * @brief Get field effectiveness_pct from merivus_ftc_motor_status message
 *
 * @return  Observed relative actuator effectiveness.
 */
static inline uint16_t mavlink_msg_merivus_ftc_motor_status_get_effectiveness_pct(const mavlink_message_t* msg, uint8_t *effectiveness_pct)
{
    return _MAV_RETURN_uint8_t_array(msg, effectiveness_pct, 12,  24);
}

/**
 * @brief Get field fault_probability_pct from merivus_ftc_motor_status message
 *
 * @return  Observed fault probability.
 */
static inline uint16_t mavlink_msg_merivus_ftc_motor_status_get_fault_probability_pct(const mavlink_message_t* msg, uint8_t *fault_probability_pct)
{
    return _MAV_RETURN_uint8_t_array(msg, fault_probability_pct, 12,  36);
}

/**
 * @brief Get field confidence_pct from merivus_ftc_motor_status message
 *
 * @return  Observer confidence.
 */
static inline uint16_t mavlink_msg_merivus_ftc_motor_status_get_confidence_pct(const mavlink_message_t* msg, uint8_t *confidence_pct)
{
    return _MAV_RETURN_uint8_t_array(msg, confidence_pct, 12,  48);
}

/**
 * @brief Get field fault_type from merivus_ftc_motor_status message
 *
 * @return  Observed fault classification.
 */
static inline uint16_t mavlink_msg_merivus_ftc_motor_status_get_fault_type(const mavlink_message_t* msg, uint8_t *fault_type)
{
    return _MAV_RETURN_uint8_t_array(msg, fault_type, 12,  60);
}

/**
 * @brief Get field protocol_version from merivus_ftc_motor_status message
 *
 * @return  MERIVUS FTC telemetry contract version.
 */
static inline uint8_t mavlink_msg_merivus_ftc_motor_status_get_protocol_version(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  72);
}

/**
 * @brief Get field system_state from merivus_ftc_motor_status message
 *
 * @return  Aggregated subsystem state.
 */
static inline uint8_t mavlink_msg_merivus_ftc_motor_status_get_system_state(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  73);
}

/**
 * @brief Get field monitor_state from merivus_ftc_motor_status message
 *
 * @return  Motor-health observer state.
 */
static inline uint8_t mavlink_msg_merivus_ftc_motor_status_get_monitor_state(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  74);
}

/**
 * @brief Get field motor_count from merivus_ftc_motor_status message
 *
 * @return  Number of valid motor entries, maximum 12.
 */
static inline uint8_t mavlink_msg_merivus_ftc_motor_status_get_motor_count(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  75);
}

/**
 * @brief Get field flags from merivus_ftc_motor_status message
 *
 * @return  Validity and source flags.
 */
static inline uint8_t mavlink_msg_merivus_ftc_motor_status_get_flags(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  76);
}

/**
 * @brief Get field model_quality_pct from merivus_ftc_motor_status message
 *
 * @return  Rigid-body model quality using the percentage scaling above.
 */
static inline uint8_t mavlink_msg_merivus_ftc_motor_status_get_model_quality_pct(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  77);
}

/**
 * @brief Get field last_valid_timestamp from merivus_ftc_motor_status message
 *
 * @return  Last accepted valid parameter update timestamp in microseconds.
 */
static inline uint64_t mavlink_msg_merivus_ftc_motor_status_get_last_valid_timestamp(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint64_t(msg,  78);
}

/**
 * @brief Get field estimate_age from merivus_ftc_motor_status message
 *
 * @return  Seconds since last parameter update; infinity before first update.
 */
static inline float mavlink_msg_merivus_ftc_motor_status_get_estimate_age(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  86);
}

/**
 * @brief Get field estimate_uncertainty from merivus_ftc_motor_status message
 *
 * @return  One sigma relative-effectiveness uncertainty.
 */
static inline uint16_t mavlink_msg_merivus_ftc_motor_status_get_estimate_uncertainty(const mavlink_message_t* msg, float *estimate_uncertainty)
{
    return _MAV_RETURN_float_array(msg, estimate_uncertainty, 12,  90);
}

/**
 * @brief Get field diagnosis_state from merivus_ftc_motor_status message
 *
 * @return  0 no evidence, 1 unknown, 2 unobservable, 3 valid healthy, 4 degraded, 5 failed.
 */
static inline uint16_t mavlink_msg_merivus_ftc_motor_status_get_diagnosis_state(const mavlink_message_t* msg, uint8_t *diagnosis_state)
{
    return _MAV_RETURN_uint8_t_array(msg, diagnosis_state, 12,  138);
}

/**
 * @brief Get field estimator_state from merivus_ftc_motor_status message
 *
 * @return  0 uninitialized, 1 calibrating, 2 baseline learned, 3 observable, 4 temporarily unobservable, 5 valid, 6 stale, 7 invalid.
 */
static inline uint8_t mavlink_msg_merivus_ftc_motor_status_get_estimator_state(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  150);
}

/**
 * @brief Get field baseline_learned from merivus_ftc_motor_status message
 *
 * @return  Healthy baseline was learned this session.
 */
static inline uint8_t mavlink_msg_merivus_ftc_motor_status_get_baseline_learned(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  151);
}

/**
 * @brief Get field current_observable from merivus_ftc_motor_status message
 *
 * @return  Current aligned sample supplies independent information.
 */
static inline uint8_t mavlink_msg_merivus_ftc_motor_status_get_current_observable(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  152);
}

/**
 * @brief Decode a merivus_ftc_motor_status message into a struct
 *
 * @param msg The message to decode
 * @param merivus_ftc_motor_status C-struct to decode the message contents into
 */
static inline void mavlink_msg_merivus_ftc_motor_status_decode(const mavlink_message_t* msg, mavlink_merivus_ftc_motor_status_t* merivus_ftc_motor_status)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    merivus_ftc_motor_status->time_usec = mavlink_msg_merivus_ftc_motor_status_get_time_usec(msg);
    merivus_ftc_motor_status->degraded_mask = mavlink_msg_merivus_ftc_motor_status_get_degraded_mask(msg);
    merivus_ftc_motor_status->failed_mask = mavlink_msg_merivus_ftc_motor_status_get_failed_mask(msg);
    mavlink_msg_merivus_ftc_motor_status_get_health_pct(msg, merivus_ftc_motor_status->health_pct);
    mavlink_msg_merivus_ftc_motor_status_get_effectiveness_pct(msg, merivus_ftc_motor_status->effectiveness_pct);
    mavlink_msg_merivus_ftc_motor_status_get_fault_probability_pct(msg, merivus_ftc_motor_status->fault_probability_pct);
    mavlink_msg_merivus_ftc_motor_status_get_confidence_pct(msg, merivus_ftc_motor_status->confidence_pct);
    mavlink_msg_merivus_ftc_motor_status_get_fault_type(msg, merivus_ftc_motor_status->fault_type);
    merivus_ftc_motor_status->protocol_version = mavlink_msg_merivus_ftc_motor_status_get_protocol_version(msg);
    merivus_ftc_motor_status->system_state = mavlink_msg_merivus_ftc_motor_status_get_system_state(msg);
    merivus_ftc_motor_status->monitor_state = mavlink_msg_merivus_ftc_motor_status_get_monitor_state(msg);
    merivus_ftc_motor_status->motor_count = mavlink_msg_merivus_ftc_motor_status_get_motor_count(msg);
    merivus_ftc_motor_status->flags = mavlink_msg_merivus_ftc_motor_status_get_flags(msg);
    merivus_ftc_motor_status->model_quality_pct = mavlink_msg_merivus_ftc_motor_status_get_model_quality_pct(msg);
    merivus_ftc_motor_status->last_valid_timestamp = mavlink_msg_merivus_ftc_motor_status_get_last_valid_timestamp(msg);
    merivus_ftc_motor_status->estimate_age = mavlink_msg_merivus_ftc_motor_status_get_estimate_age(msg);
    mavlink_msg_merivus_ftc_motor_status_get_estimate_uncertainty(msg, merivus_ftc_motor_status->estimate_uncertainty);
    mavlink_msg_merivus_ftc_motor_status_get_diagnosis_state(msg, merivus_ftc_motor_status->diagnosis_state);
    merivus_ftc_motor_status->estimator_state = mavlink_msg_merivus_ftc_motor_status_get_estimator_state(msg);
    merivus_ftc_motor_status->baseline_learned = mavlink_msg_merivus_ftc_motor_status_get_baseline_learned(msg);
    merivus_ftc_motor_status->current_observable = mavlink_msg_merivus_ftc_motor_status_get_current_observable(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_LEN? msg->len : MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_LEN;
        memset(merivus_ftc_motor_status, 0, MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_LEN);
    memcpy(merivus_ftc_motor_status, _MAV_PAYLOAD(msg), len);
#endif
}
