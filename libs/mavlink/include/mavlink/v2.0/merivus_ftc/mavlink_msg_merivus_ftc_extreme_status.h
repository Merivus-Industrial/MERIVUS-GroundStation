#pragma once
// MESSAGE MERIVUS_FTC_EXTREME_STATUS PACKING

#define MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS 60002


typedef struct __mavlink_merivus_ftc_extreme_status_t {
 uint64_t time_usec; /*< [us] PX4 monotonic publication timestamp.*/
 uint64_t event_time_usec; /*< [us] Most recent extreme-event timestamp.*/
 uint32_t loss_of_control_reason_mask; /*<  Loss-of-control reason bit mask.*/
 uint32_t recovery_trigger_mask; /*<  Recovery-candidate trigger bit mask.*/
 uint32_t recovery_inhibit_mask; /*<  Recovery-candidate inhibit bit mask.*/
 uint8_t protocol_version; /*<  MERIVUS FTC telemetry contract version.*/
 uint8_t impact_type; /*<  Observed impact type.*/
 uint8_t loc_state; /*<  Loss-of-control state.*/
 uint8_t recovery_state; /*<  Recovery-candidate state.*/
 uint8_t flags; /*<  Validity and event flags.*/
 uint8_t impact_score_pct; /*<  Impact score.*/
 uint8_t impact_confidence_pct; /*<  Impact confidence.*/
 uint8_t impact_severity_pct; /*<  Impact severity.*/
 uint8_t loss_of_control_score_pct; /*<  Loss-of-control score.*/
 uint8_t recovery_progress_pct; /*<  Recovery-candidate state progress.*/
} mavlink_merivus_ftc_extreme_status_t;

#define MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_LEN 38
#define MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_MIN_LEN 38
#define MAVLINK_MSG_ID_60002_LEN 38
#define MAVLINK_MSG_ID_60002_MIN_LEN 38

#define MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_CRC 136
#define MAVLINK_MSG_ID_60002_CRC 136



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_MERIVUS_FTC_EXTREME_STATUS { \
    60002, \
    "MERIVUS_FTC_EXTREME_STATUS", \
    15, \
    {  { "time_usec", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_merivus_ftc_extreme_status_t, time_usec) }, \
         { "event_time_usec", NULL, MAVLINK_TYPE_UINT64_T, 0, 8, offsetof(mavlink_merivus_ftc_extreme_status_t, event_time_usec) }, \
         { "loss_of_control_reason_mask", NULL, MAVLINK_TYPE_UINT32_T, 0, 16, offsetof(mavlink_merivus_ftc_extreme_status_t, loss_of_control_reason_mask) }, \
         { "recovery_trigger_mask", NULL, MAVLINK_TYPE_UINT32_T, 0, 20, offsetof(mavlink_merivus_ftc_extreme_status_t, recovery_trigger_mask) }, \
         { "recovery_inhibit_mask", NULL, MAVLINK_TYPE_UINT32_T, 0, 24, offsetof(mavlink_merivus_ftc_extreme_status_t, recovery_inhibit_mask) }, \
         { "protocol_version", NULL, MAVLINK_TYPE_UINT8_T, 0, 28, offsetof(mavlink_merivus_ftc_extreme_status_t, protocol_version) }, \
         { "impact_type", NULL, MAVLINK_TYPE_UINT8_T, 0, 29, offsetof(mavlink_merivus_ftc_extreme_status_t, impact_type) }, \
         { "loc_state", NULL, MAVLINK_TYPE_UINT8_T, 0, 30, offsetof(mavlink_merivus_ftc_extreme_status_t, loc_state) }, \
         { "recovery_state", NULL, MAVLINK_TYPE_UINT8_T, 0, 31, offsetof(mavlink_merivus_ftc_extreme_status_t, recovery_state) }, \
         { "flags", NULL, MAVLINK_TYPE_UINT8_T, 0, 32, offsetof(mavlink_merivus_ftc_extreme_status_t, flags) }, \
         { "impact_score_pct", NULL, MAVLINK_TYPE_UINT8_T, 0, 33, offsetof(mavlink_merivus_ftc_extreme_status_t, impact_score_pct) }, \
         { "impact_confidence_pct", NULL, MAVLINK_TYPE_UINT8_T, 0, 34, offsetof(mavlink_merivus_ftc_extreme_status_t, impact_confidence_pct) }, \
         { "impact_severity_pct", NULL, MAVLINK_TYPE_UINT8_T, 0, 35, offsetof(mavlink_merivus_ftc_extreme_status_t, impact_severity_pct) }, \
         { "loss_of_control_score_pct", NULL, MAVLINK_TYPE_UINT8_T, 0, 36, offsetof(mavlink_merivus_ftc_extreme_status_t, loss_of_control_score_pct) }, \
         { "recovery_progress_pct", NULL, MAVLINK_TYPE_UINT8_T, 0, 37, offsetof(mavlink_merivus_ftc_extreme_status_t, recovery_progress_pct) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_MERIVUS_FTC_EXTREME_STATUS { \
    "MERIVUS_FTC_EXTREME_STATUS", \
    15, \
    {  { "time_usec", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_merivus_ftc_extreme_status_t, time_usec) }, \
         { "event_time_usec", NULL, MAVLINK_TYPE_UINT64_T, 0, 8, offsetof(mavlink_merivus_ftc_extreme_status_t, event_time_usec) }, \
         { "loss_of_control_reason_mask", NULL, MAVLINK_TYPE_UINT32_T, 0, 16, offsetof(mavlink_merivus_ftc_extreme_status_t, loss_of_control_reason_mask) }, \
         { "recovery_trigger_mask", NULL, MAVLINK_TYPE_UINT32_T, 0, 20, offsetof(mavlink_merivus_ftc_extreme_status_t, recovery_trigger_mask) }, \
         { "recovery_inhibit_mask", NULL, MAVLINK_TYPE_UINT32_T, 0, 24, offsetof(mavlink_merivus_ftc_extreme_status_t, recovery_inhibit_mask) }, \
         { "protocol_version", NULL, MAVLINK_TYPE_UINT8_T, 0, 28, offsetof(mavlink_merivus_ftc_extreme_status_t, protocol_version) }, \
         { "impact_type", NULL, MAVLINK_TYPE_UINT8_T, 0, 29, offsetof(mavlink_merivus_ftc_extreme_status_t, impact_type) }, \
         { "loc_state", NULL, MAVLINK_TYPE_UINT8_T, 0, 30, offsetof(mavlink_merivus_ftc_extreme_status_t, loc_state) }, \
         { "recovery_state", NULL, MAVLINK_TYPE_UINT8_T, 0, 31, offsetof(mavlink_merivus_ftc_extreme_status_t, recovery_state) }, \
         { "flags", NULL, MAVLINK_TYPE_UINT8_T, 0, 32, offsetof(mavlink_merivus_ftc_extreme_status_t, flags) }, \
         { "impact_score_pct", NULL, MAVLINK_TYPE_UINT8_T, 0, 33, offsetof(mavlink_merivus_ftc_extreme_status_t, impact_score_pct) }, \
         { "impact_confidence_pct", NULL, MAVLINK_TYPE_UINT8_T, 0, 34, offsetof(mavlink_merivus_ftc_extreme_status_t, impact_confidence_pct) }, \
         { "impact_severity_pct", NULL, MAVLINK_TYPE_UINT8_T, 0, 35, offsetof(mavlink_merivus_ftc_extreme_status_t, impact_severity_pct) }, \
         { "loss_of_control_score_pct", NULL, MAVLINK_TYPE_UINT8_T, 0, 36, offsetof(mavlink_merivus_ftc_extreme_status_t, loss_of_control_score_pct) }, \
         { "recovery_progress_pct", NULL, MAVLINK_TYPE_UINT8_T, 0, 37, offsetof(mavlink_merivus_ftc_extreme_status_t, recovery_progress_pct) }, \
         } \
}
#endif

/**
 * @brief Pack a merivus_ftc_extreme_status message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_usec [us] PX4 monotonic publication timestamp.
 * @param event_time_usec [us] Most recent extreme-event timestamp.
 * @param loss_of_control_reason_mask  Loss-of-control reason bit mask.
 * @param recovery_trigger_mask  Recovery-candidate trigger bit mask.
 * @param recovery_inhibit_mask  Recovery-candidate inhibit bit mask.
 * @param protocol_version  MERIVUS FTC telemetry contract version.
 * @param impact_type  Observed impact type.
 * @param loc_state  Loss-of-control state.
 * @param recovery_state  Recovery-candidate state.
 * @param flags  Validity and event flags.
 * @param impact_score_pct  Impact score.
 * @param impact_confidence_pct  Impact confidence.
 * @param impact_severity_pct  Impact severity.
 * @param loss_of_control_score_pct  Loss-of-control score.
 * @param recovery_progress_pct  Recovery-candidate state progress.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_merivus_ftc_extreme_status_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint64_t time_usec, uint64_t event_time_usec, uint32_t loss_of_control_reason_mask, uint32_t recovery_trigger_mask, uint32_t recovery_inhibit_mask, uint8_t protocol_version, uint8_t impact_type, uint8_t loc_state, uint8_t recovery_state, uint8_t flags, uint8_t impact_score_pct, uint8_t impact_confidence_pct, uint8_t impact_severity_pct, uint8_t loss_of_control_score_pct, uint8_t recovery_progress_pct)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_LEN];
    _mav_put_uint64_t(buf, 0, time_usec);
    _mav_put_uint64_t(buf, 8, event_time_usec);
    _mav_put_uint32_t(buf, 16, loss_of_control_reason_mask);
    _mav_put_uint32_t(buf, 20, recovery_trigger_mask);
    _mav_put_uint32_t(buf, 24, recovery_inhibit_mask);
    _mav_put_uint8_t(buf, 28, protocol_version);
    _mav_put_uint8_t(buf, 29, impact_type);
    _mav_put_uint8_t(buf, 30, loc_state);
    _mav_put_uint8_t(buf, 31, recovery_state);
    _mav_put_uint8_t(buf, 32, flags);
    _mav_put_uint8_t(buf, 33, impact_score_pct);
    _mav_put_uint8_t(buf, 34, impact_confidence_pct);
    _mav_put_uint8_t(buf, 35, impact_severity_pct);
    _mav_put_uint8_t(buf, 36, loss_of_control_score_pct);
    _mav_put_uint8_t(buf, 37, recovery_progress_pct);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_LEN);
#else
    mavlink_merivus_ftc_extreme_status_t packet;
    packet.time_usec = time_usec;
    packet.event_time_usec = event_time_usec;
    packet.loss_of_control_reason_mask = loss_of_control_reason_mask;
    packet.recovery_trigger_mask = recovery_trigger_mask;
    packet.recovery_inhibit_mask = recovery_inhibit_mask;
    packet.protocol_version = protocol_version;
    packet.impact_type = impact_type;
    packet.loc_state = loc_state;
    packet.recovery_state = recovery_state;
    packet.flags = flags;
    packet.impact_score_pct = impact_score_pct;
    packet.impact_confidence_pct = impact_confidence_pct;
    packet.impact_severity_pct = impact_severity_pct;
    packet.loss_of_control_score_pct = loss_of_control_score_pct;
    packet.recovery_progress_pct = recovery_progress_pct;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_MIN_LEN, MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_LEN, MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_CRC);
}

/**
 * @brief Pack a merivus_ftc_extreme_status message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param time_usec [us] PX4 monotonic publication timestamp.
 * @param event_time_usec [us] Most recent extreme-event timestamp.
 * @param loss_of_control_reason_mask  Loss-of-control reason bit mask.
 * @param recovery_trigger_mask  Recovery-candidate trigger bit mask.
 * @param recovery_inhibit_mask  Recovery-candidate inhibit bit mask.
 * @param protocol_version  MERIVUS FTC telemetry contract version.
 * @param impact_type  Observed impact type.
 * @param loc_state  Loss-of-control state.
 * @param recovery_state  Recovery-candidate state.
 * @param flags  Validity and event flags.
 * @param impact_score_pct  Impact score.
 * @param impact_confidence_pct  Impact confidence.
 * @param impact_severity_pct  Impact severity.
 * @param loss_of_control_score_pct  Loss-of-control score.
 * @param recovery_progress_pct  Recovery-candidate state progress.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_merivus_ftc_extreme_status_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint64_t time_usec,uint64_t event_time_usec,uint32_t loss_of_control_reason_mask,uint32_t recovery_trigger_mask,uint32_t recovery_inhibit_mask,uint8_t protocol_version,uint8_t impact_type,uint8_t loc_state,uint8_t recovery_state,uint8_t flags,uint8_t impact_score_pct,uint8_t impact_confidence_pct,uint8_t impact_severity_pct,uint8_t loss_of_control_score_pct,uint8_t recovery_progress_pct)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_LEN];
    _mav_put_uint64_t(buf, 0, time_usec);
    _mav_put_uint64_t(buf, 8, event_time_usec);
    _mav_put_uint32_t(buf, 16, loss_of_control_reason_mask);
    _mav_put_uint32_t(buf, 20, recovery_trigger_mask);
    _mav_put_uint32_t(buf, 24, recovery_inhibit_mask);
    _mav_put_uint8_t(buf, 28, protocol_version);
    _mav_put_uint8_t(buf, 29, impact_type);
    _mav_put_uint8_t(buf, 30, loc_state);
    _mav_put_uint8_t(buf, 31, recovery_state);
    _mav_put_uint8_t(buf, 32, flags);
    _mav_put_uint8_t(buf, 33, impact_score_pct);
    _mav_put_uint8_t(buf, 34, impact_confidence_pct);
    _mav_put_uint8_t(buf, 35, impact_severity_pct);
    _mav_put_uint8_t(buf, 36, loss_of_control_score_pct);
    _mav_put_uint8_t(buf, 37, recovery_progress_pct);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_LEN);
#else
    mavlink_merivus_ftc_extreme_status_t packet;
    packet.time_usec = time_usec;
    packet.event_time_usec = event_time_usec;
    packet.loss_of_control_reason_mask = loss_of_control_reason_mask;
    packet.recovery_trigger_mask = recovery_trigger_mask;
    packet.recovery_inhibit_mask = recovery_inhibit_mask;
    packet.protocol_version = protocol_version;
    packet.impact_type = impact_type;
    packet.loc_state = loc_state;
    packet.recovery_state = recovery_state;
    packet.flags = flags;
    packet.impact_score_pct = impact_score_pct;
    packet.impact_confidence_pct = impact_confidence_pct;
    packet.impact_severity_pct = impact_severity_pct;
    packet.loss_of_control_score_pct = loss_of_control_score_pct;
    packet.recovery_progress_pct = recovery_progress_pct;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_MIN_LEN, MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_LEN, MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_CRC);
}

/**
 * @brief Encode a merivus_ftc_extreme_status struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param merivus_ftc_extreme_status C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_merivus_ftc_extreme_status_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_merivus_ftc_extreme_status_t* merivus_ftc_extreme_status)
{
    return mavlink_msg_merivus_ftc_extreme_status_pack(system_id, component_id, msg, merivus_ftc_extreme_status->time_usec, merivus_ftc_extreme_status->event_time_usec, merivus_ftc_extreme_status->loss_of_control_reason_mask, merivus_ftc_extreme_status->recovery_trigger_mask, merivus_ftc_extreme_status->recovery_inhibit_mask, merivus_ftc_extreme_status->protocol_version, merivus_ftc_extreme_status->impact_type, merivus_ftc_extreme_status->loc_state, merivus_ftc_extreme_status->recovery_state, merivus_ftc_extreme_status->flags, merivus_ftc_extreme_status->impact_score_pct, merivus_ftc_extreme_status->impact_confidence_pct, merivus_ftc_extreme_status->impact_severity_pct, merivus_ftc_extreme_status->loss_of_control_score_pct, merivus_ftc_extreme_status->recovery_progress_pct);
}

/**
 * @brief Encode a merivus_ftc_extreme_status struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param merivus_ftc_extreme_status C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_merivus_ftc_extreme_status_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_merivus_ftc_extreme_status_t* merivus_ftc_extreme_status)
{
    return mavlink_msg_merivus_ftc_extreme_status_pack_chan(system_id, component_id, chan, msg, merivus_ftc_extreme_status->time_usec, merivus_ftc_extreme_status->event_time_usec, merivus_ftc_extreme_status->loss_of_control_reason_mask, merivus_ftc_extreme_status->recovery_trigger_mask, merivus_ftc_extreme_status->recovery_inhibit_mask, merivus_ftc_extreme_status->protocol_version, merivus_ftc_extreme_status->impact_type, merivus_ftc_extreme_status->loc_state, merivus_ftc_extreme_status->recovery_state, merivus_ftc_extreme_status->flags, merivus_ftc_extreme_status->impact_score_pct, merivus_ftc_extreme_status->impact_confidence_pct, merivus_ftc_extreme_status->impact_severity_pct, merivus_ftc_extreme_status->loss_of_control_score_pct, merivus_ftc_extreme_status->recovery_progress_pct);
}

/**
 * @brief Send a merivus_ftc_extreme_status message
 * @param chan MAVLink channel to send the message
 *
 * @param time_usec [us] PX4 monotonic publication timestamp.
 * @param event_time_usec [us] Most recent extreme-event timestamp.
 * @param loss_of_control_reason_mask  Loss-of-control reason bit mask.
 * @param recovery_trigger_mask  Recovery-candidate trigger bit mask.
 * @param recovery_inhibit_mask  Recovery-candidate inhibit bit mask.
 * @param protocol_version  MERIVUS FTC telemetry contract version.
 * @param impact_type  Observed impact type.
 * @param loc_state  Loss-of-control state.
 * @param recovery_state  Recovery-candidate state.
 * @param flags  Validity and event flags.
 * @param impact_score_pct  Impact score.
 * @param impact_confidence_pct  Impact confidence.
 * @param impact_severity_pct  Impact severity.
 * @param loss_of_control_score_pct  Loss-of-control score.
 * @param recovery_progress_pct  Recovery-candidate state progress.
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_merivus_ftc_extreme_status_send(mavlink_channel_t chan, uint64_t time_usec, uint64_t event_time_usec, uint32_t loss_of_control_reason_mask, uint32_t recovery_trigger_mask, uint32_t recovery_inhibit_mask, uint8_t protocol_version, uint8_t impact_type, uint8_t loc_state, uint8_t recovery_state, uint8_t flags, uint8_t impact_score_pct, uint8_t impact_confidence_pct, uint8_t impact_severity_pct, uint8_t loss_of_control_score_pct, uint8_t recovery_progress_pct)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_LEN];
    _mav_put_uint64_t(buf, 0, time_usec);
    _mav_put_uint64_t(buf, 8, event_time_usec);
    _mav_put_uint32_t(buf, 16, loss_of_control_reason_mask);
    _mav_put_uint32_t(buf, 20, recovery_trigger_mask);
    _mav_put_uint32_t(buf, 24, recovery_inhibit_mask);
    _mav_put_uint8_t(buf, 28, protocol_version);
    _mav_put_uint8_t(buf, 29, impact_type);
    _mav_put_uint8_t(buf, 30, loc_state);
    _mav_put_uint8_t(buf, 31, recovery_state);
    _mav_put_uint8_t(buf, 32, flags);
    _mav_put_uint8_t(buf, 33, impact_score_pct);
    _mav_put_uint8_t(buf, 34, impact_confidence_pct);
    _mav_put_uint8_t(buf, 35, impact_severity_pct);
    _mav_put_uint8_t(buf, 36, loss_of_control_score_pct);
    _mav_put_uint8_t(buf, 37, recovery_progress_pct);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS, buf, MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_MIN_LEN, MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_LEN, MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_CRC);
#else
    mavlink_merivus_ftc_extreme_status_t packet;
    packet.time_usec = time_usec;
    packet.event_time_usec = event_time_usec;
    packet.loss_of_control_reason_mask = loss_of_control_reason_mask;
    packet.recovery_trigger_mask = recovery_trigger_mask;
    packet.recovery_inhibit_mask = recovery_inhibit_mask;
    packet.protocol_version = protocol_version;
    packet.impact_type = impact_type;
    packet.loc_state = loc_state;
    packet.recovery_state = recovery_state;
    packet.flags = flags;
    packet.impact_score_pct = impact_score_pct;
    packet.impact_confidence_pct = impact_confidence_pct;
    packet.impact_severity_pct = impact_severity_pct;
    packet.loss_of_control_score_pct = loss_of_control_score_pct;
    packet.recovery_progress_pct = recovery_progress_pct;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS, (const char *)&packet, MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_MIN_LEN, MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_LEN, MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_CRC);
#endif
}

/**
 * @brief Send a merivus_ftc_extreme_status message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_merivus_ftc_extreme_status_send_struct(mavlink_channel_t chan, const mavlink_merivus_ftc_extreme_status_t* merivus_ftc_extreme_status)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_merivus_ftc_extreme_status_send(chan, merivus_ftc_extreme_status->time_usec, merivus_ftc_extreme_status->event_time_usec, merivus_ftc_extreme_status->loss_of_control_reason_mask, merivus_ftc_extreme_status->recovery_trigger_mask, merivus_ftc_extreme_status->recovery_inhibit_mask, merivus_ftc_extreme_status->protocol_version, merivus_ftc_extreme_status->impact_type, merivus_ftc_extreme_status->loc_state, merivus_ftc_extreme_status->recovery_state, merivus_ftc_extreme_status->flags, merivus_ftc_extreme_status->impact_score_pct, merivus_ftc_extreme_status->impact_confidence_pct, merivus_ftc_extreme_status->impact_severity_pct, merivus_ftc_extreme_status->loss_of_control_score_pct, merivus_ftc_extreme_status->recovery_progress_pct);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS, (const char *)merivus_ftc_extreme_status, MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_MIN_LEN, MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_LEN, MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_CRC);
#endif
}

#if MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This variant of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_merivus_ftc_extreme_status_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint64_t time_usec, uint64_t event_time_usec, uint32_t loss_of_control_reason_mask, uint32_t recovery_trigger_mask, uint32_t recovery_inhibit_mask, uint8_t protocol_version, uint8_t impact_type, uint8_t loc_state, uint8_t recovery_state, uint8_t flags, uint8_t impact_score_pct, uint8_t impact_confidence_pct, uint8_t impact_severity_pct, uint8_t loss_of_control_score_pct, uint8_t recovery_progress_pct)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint64_t(buf, 0, time_usec);
    _mav_put_uint64_t(buf, 8, event_time_usec);
    _mav_put_uint32_t(buf, 16, loss_of_control_reason_mask);
    _mav_put_uint32_t(buf, 20, recovery_trigger_mask);
    _mav_put_uint32_t(buf, 24, recovery_inhibit_mask);
    _mav_put_uint8_t(buf, 28, protocol_version);
    _mav_put_uint8_t(buf, 29, impact_type);
    _mav_put_uint8_t(buf, 30, loc_state);
    _mav_put_uint8_t(buf, 31, recovery_state);
    _mav_put_uint8_t(buf, 32, flags);
    _mav_put_uint8_t(buf, 33, impact_score_pct);
    _mav_put_uint8_t(buf, 34, impact_confidence_pct);
    _mav_put_uint8_t(buf, 35, impact_severity_pct);
    _mav_put_uint8_t(buf, 36, loss_of_control_score_pct);
    _mav_put_uint8_t(buf, 37, recovery_progress_pct);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS, buf, MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_MIN_LEN, MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_LEN, MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_CRC);
#else
    mavlink_merivus_ftc_extreme_status_t *packet = (mavlink_merivus_ftc_extreme_status_t *)msgbuf;
    packet->time_usec = time_usec;
    packet->event_time_usec = event_time_usec;
    packet->loss_of_control_reason_mask = loss_of_control_reason_mask;
    packet->recovery_trigger_mask = recovery_trigger_mask;
    packet->recovery_inhibit_mask = recovery_inhibit_mask;
    packet->protocol_version = protocol_version;
    packet->impact_type = impact_type;
    packet->loc_state = loc_state;
    packet->recovery_state = recovery_state;
    packet->flags = flags;
    packet->impact_score_pct = impact_score_pct;
    packet->impact_confidence_pct = impact_confidence_pct;
    packet->impact_severity_pct = impact_severity_pct;
    packet->loss_of_control_score_pct = loss_of_control_score_pct;
    packet->recovery_progress_pct = recovery_progress_pct;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS, (const char *)packet, MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_MIN_LEN, MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_LEN, MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_CRC);
#endif
}
#endif

#endif

// MESSAGE MERIVUS_FTC_EXTREME_STATUS UNPACKING


/**
 * @brief Get field time_usec from merivus_ftc_extreme_status message
 *
 * @return [us] PX4 monotonic publication timestamp.
 */
static inline uint64_t mavlink_msg_merivus_ftc_extreme_status_get_time_usec(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint64_t(msg,  0);
}

/**
 * @brief Get field event_time_usec from merivus_ftc_extreme_status message
 *
 * @return [us] Most recent extreme-event timestamp.
 */
static inline uint64_t mavlink_msg_merivus_ftc_extreme_status_get_event_time_usec(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint64_t(msg,  8);
}

/**
 * @brief Get field loss_of_control_reason_mask from merivus_ftc_extreme_status message
 *
 * @return  Loss-of-control reason bit mask.
 */
static inline uint32_t mavlink_msg_merivus_ftc_extreme_status_get_loss_of_control_reason_mask(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint32_t(msg,  16);
}

/**
 * @brief Get field recovery_trigger_mask from merivus_ftc_extreme_status message
 *
 * @return  Recovery-candidate trigger bit mask.
 */
static inline uint32_t mavlink_msg_merivus_ftc_extreme_status_get_recovery_trigger_mask(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint32_t(msg,  20);
}

/**
 * @brief Get field recovery_inhibit_mask from merivus_ftc_extreme_status message
 *
 * @return  Recovery-candidate inhibit bit mask.
 */
static inline uint32_t mavlink_msg_merivus_ftc_extreme_status_get_recovery_inhibit_mask(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint32_t(msg,  24);
}

/**
 * @brief Get field protocol_version from merivus_ftc_extreme_status message
 *
 * @return  MERIVUS FTC telemetry contract version.
 */
static inline uint8_t mavlink_msg_merivus_ftc_extreme_status_get_protocol_version(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  28);
}

/**
 * @brief Get field impact_type from merivus_ftc_extreme_status message
 *
 * @return  Observed impact type.
 */
static inline uint8_t mavlink_msg_merivus_ftc_extreme_status_get_impact_type(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  29);
}

/**
 * @brief Get field loc_state from merivus_ftc_extreme_status message
 *
 * @return  Loss-of-control state.
 */
static inline uint8_t mavlink_msg_merivus_ftc_extreme_status_get_loc_state(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  30);
}

/**
 * @brief Get field recovery_state from merivus_ftc_extreme_status message
 *
 * @return  Recovery-candidate state.
 */
static inline uint8_t mavlink_msg_merivus_ftc_extreme_status_get_recovery_state(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  31);
}

/**
 * @brief Get field flags from merivus_ftc_extreme_status message
 *
 * @return  Validity and event flags.
 */
static inline uint8_t mavlink_msg_merivus_ftc_extreme_status_get_flags(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  32);
}

/**
 * @brief Get field impact_score_pct from merivus_ftc_extreme_status message
 *
 * @return  Impact score.
 */
static inline uint8_t mavlink_msg_merivus_ftc_extreme_status_get_impact_score_pct(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  33);
}

/**
 * @brief Get field impact_confidence_pct from merivus_ftc_extreme_status message
 *
 * @return  Impact confidence.
 */
static inline uint8_t mavlink_msg_merivus_ftc_extreme_status_get_impact_confidence_pct(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  34);
}

/**
 * @brief Get field impact_severity_pct from merivus_ftc_extreme_status message
 *
 * @return  Impact severity.
 */
static inline uint8_t mavlink_msg_merivus_ftc_extreme_status_get_impact_severity_pct(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  35);
}

/**
 * @brief Get field loss_of_control_score_pct from merivus_ftc_extreme_status message
 *
 * @return  Loss-of-control score.
 */
static inline uint8_t mavlink_msg_merivus_ftc_extreme_status_get_loss_of_control_score_pct(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  36);
}

/**
 * @brief Get field recovery_progress_pct from merivus_ftc_extreme_status message
 *
 * @return  Recovery-candidate state progress.
 */
static inline uint8_t mavlink_msg_merivus_ftc_extreme_status_get_recovery_progress_pct(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  37);
}

/**
 * @brief Decode a merivus_ftc_extreme_status message into a struct
 *
 * @param msg The message to decode
 * @param merivus_ftc_extreme_status C-struct to decode the message contents into
 */
static inline void mavlink_msg_merivus_ftc_extreme_status_decode(const mavlink_message_t* msg, mavlink_merivus_ftc_extreme_status_t* merivus_ftc_extreme_status)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    merivus_ftc_extreme_status->time_usec = mavlink_msg_merivus_ftc_extreme_status_get_time_usec(msg);
    merivus_ftc_extreme_status->event_time_usec = mavlink_msg_merivus_ftc_extreme_status_get_event_time_usec(msg);
    merivus_ftc_extreme_status->loss_of_control_reason_mask = mavlink_msg_merivus_ftc_extreme_status_get_loss_of_control_reason_mask(msg);
    merivus_ftc_extreme_status->recovery_trigger_mask = mavlink_msg_merivus_ftc_extreme_status_get_recovery_trigger_mask(msg);
    merivus_ftc_extreme_status->recovery_inhibit_mask = mavlink_msg_merivus_ftc_extreme_status_get_recovery_inhibit_mask(msg);
    merivus_ftc_extreme_status->protocol_version = mavlink_msg_merivus_ftc_extreme_status_get_protocol_version(msg);
    merivus_ftc_extreme_status->impact_type = mavlink_msg_merivus_ftc_extreme_status_get_impact_type(msg);
    merivus_ftc_extreme_status->loc_state = mavlink_msg_merivus_ftc_extreme_status_get_loc_state(msg);
    merivus_ftc_extreme_status->recovery_state = mavlink_msg_merivus_ftc_extreme_status_get_recovery_state(msg);
    merivus_ftc_extreme_status->flags = mavlink_msg_merivus_ftc_extreme_status_get_flags(msg);
    merivus_ftc_extreme_status->impact_score_pct = mavlink_msg_merivus_ftc_extreme_status_get_impact_score_pct(msg);
    merivus_ftc_extreme_status->impact_confidence_pct = mavlink_msg_merivus_ftc_extreme_status_get_impact_confidence_pct(msg);
    merivus_ftc_extreme_status->impact_severity_pct = mavlink_msg_merivus_ftc_extreme_status_get_impact_severity_pct(msg);
    merivus_ftc_extreme_status->loss_of_control_score_pct = mavlink_msg_merivus_ftc_extreme_status_get_loss_of_control_score_pct(msg);
    merivus_ftc_extreme_status->recovery_progress_pct = mavlink_msg_merivus_ftc_extreme_status_get_recovery_progress_pct(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_LEN? msg->len : MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_LEN;
        memset(merivus_ftc_extreme_status, 0, MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_LEN);
    memcpy(merivus_ftc_extreme_status, _MAV_PAYLOAD(msg), len);
#endif
}
