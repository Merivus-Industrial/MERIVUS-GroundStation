/** @file
 *    @brief MAVLink comm protocol testsuite generated from merivus_ftc.xml
 *    @see https://mavlink.io/en/
 */
#pragma once
#ifndef MERIVUS_FTC_TESTSUITE_H
#define MERIVUS_FTC_TESTSUITE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAVLINK_TEST_ALL
#define MAVLINK_TEST_ALL

static void mavlink_test_merivus_ftc(uint8_t, uint8_t, mavlink_message_t *last_msg);

static void mavlink_test_all(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{

    mavlink_test_merivus_ftc(system_id, component_id, last_msg);
}
#endif




static void mavlink_test_merivus_ftc_motor_status(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
    mavlink_status_t *status = mavlink_get_channel_status(MAVLINK_COMM_0);
        if ((status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) && MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS >= 256) {
            return;
        }
#endif
    mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
    mavlink_merivus_ftc_motor_status_t packet_in = {
        93372036854775807ULL,17651,17755,{ 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52 },{ 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88 },{ 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124 },{ 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160 },{ 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196 },221,32,99,166,233,44,93372036854780721ULL,619.0,{ 647.0, 648.0, 649.0, 650.0, 651.0, 652.0, 653.0, 654.0, 655.0, 656.0, 657.0, 658.0 },{ 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46 },71,138,205
    };
    mavlink_merivus_ftc_motor_status_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        packet1.time_usec = packet_in.time_usec;
        packet1.degraded_mask = packet_in.degraded_mask;
        packet1.failed_mask = packet_in.failed_mask;
        packet1.protocol_version = packet_in.protocol_version;
        packet1.system_state = packet_in.system_state;
        packet1.monitor_state = packet_in.monitor_state;
        packet1.motor_count = packet_in.motor_count;
        packet1.flags = packet_in.flags;
        packet1.model_quality_pct = packet_in.model_quality_pct;
        packet1.last_valid_timestamp = packet_in.last_valid_timestamp;
        packet1.estimate_age = packet_in.estimate_age;
        packet1.estimator_state = packet_in.estimator_state;
        packet1.baseline_learned = packet_in.baseline_learned;
        packet1.current_observable = packet_in.current_observable;

        mav_array_memcpy(packet1.health_pct, packet_in.health_pct, sizeof(uint8_t)*12);
        mav_array_memcpy(packet1.effectiveness_pct, packet_in.effectiveness_pct, sizeof(uint8_t)*12);
        mav_array_memcpy(packet1.fault_probability_pct, packet_in.fault_probability_pct, sizeof(uint8_t)*12);
        mav_array_memcpy(packet1.confidence_pct, packet_in.confidence_pct, sizeof(uint8_t)*12);
        mav_array_memcpy(packet1.fault_type, packet_in.fault_type, sizeof(uint8_t)*12);
        mav_array_memcpy(packet1.estimate_uncertainty, packet_in.estimate_uncertainty, sizeof(float)*12);
        mav_array_memcpy(packet1.diagnosis_state, packet_in.diagnosis_state, sizeof(uint8_t)*12);

#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
        if (status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) {
           // cope with extensions
           memset(MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_MIN_LEN + (char *)&packet1, 0, sizeof(packet1)-MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS_MIN_LEN);
        }
#endif
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_merivus_ftc_motor_status_encode(system_id, component_id, &msg, &packet1);
    mavlink_msg_merivus_ftc_motor_status_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_merivus_ftc_motor_status_pack(system_id, component_id, &msg , packet1.time_usec , packet1.degraded_mask , packet1.failed_mask , packet1.health_pct , packet1.effectiveness_pct , packet1.fault_probability_pct , packet1.confidence_pct , packet1.fault_type , packet1.protocol_version , packet1.system_state , packet1.monitor_state , packet1.motor_count , packet1.flags , packet1.model_quality_pct , packet1.last_valid_timestamp , packet1.estimate_age , packet1.estimate_uncertainty , packet1.diagnosis_state , packet1.estimator_state , packet1.baseline_learned , packet1.current_observable );
    mavlink_msg_merivus_ftc_motor_status_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_merivus_ftc_motor_status_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.time_usec , packet1.degraded_mask , packet1.failed_mask , packet1.health_pct , packet1.effectiveness_pct , packet1.fault_probability_pct , packet1.confidence_pct , packet1.fault_type , packet1.protocol_version , packet1.system_state , packet1.monitor_state , packet1.motor_count , packet1.flags , packet1.model_quality_pct , packet1.last_valid_timestamp , packet1.estimate_age , packet1.estimate_uncertainty , packet1.diagnosis_state , packet1.estimator_state , packet1.baseline_learned , packet1.current_observable );
    mavlink_msg_merivus_ftc_motor_status_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
            comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
    mavlink_msg_merivus_ftc_motor_status_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_merivus_ftc_motor_status_send(MAVLINK_COMM_1 , packet1.time_usec , packet1.degraded_mask , packet1.failed_mask , packet1.health_pct , packet1.effectiveness_pct , packet1.fault_probability_pct , packet1.confidence_pct , packet1.fault_type , packet1.protocol_version , packet1.system_state , packet1.monitor_state , packet1.motor_count , packet1.flags , packet1.model_quality_pct , packet1.last_valid_timestamp , packet1.estimate_age , packet1.estimate_uncertainty , packet1.diagnosis_state , packet1.estimator_state , packet1.baseline_learned , packet1.current_observable );
    mavlink_msg_merivus_ftc_motor_status_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

#ifdef MAVLINK_HAVE_GET_MESSAGE_INFO
    MAVLINK_ASSERT(mavlink_get_message_info_by_name("MERIVUS_FTC_MOTOR_STATUS") != NULL);
    MAVLINK_ASSERT(mavlink_get_message_info_by_id(MAVLINK_MSG_ID_MERIVUS_FTC_MOTOR_STATUS) != NULL);
#endif
}

static void mavlink_test_merivus_ftc_control_status(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
    mavlink_status_t *status = mavlink_get_channel_status(MAVLINK_COMM_0);
        if ((status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) && MAVLINK_MSG_ID_MERIVUS_FTC_CONTROL_STATUS >= 256) {
            return;
        }
#endif
    mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
    mavlink_merivus_ftc_control_status_t packet_in = {
        93372036854775807ULL,17651,163,230,41,108,175,242,53,120,187,254,65,132,199,10,{ 185.0, 186.0, 187.0 },{ 269.0, 270.0, 271.0 },353.0,381.0,409.0,963500584,963500792,493.0,521.0,233,44
    };
    mavlink_merivus_ftc_control_status_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        packet1.time_usec = packet_in.time_usec;
        packet1.saturated_mask = packet_in.saturated_mask;
        packet1.protocol_version = packet_in.protocol_version;
        packet1.system_state = packet_in.system_state;
        packet1.authority_state = packet_in.authority_state;
        packet1.control_mode = packet_in.control_mode;
        packet1.recovery_state = packet_in.recovery_state;
        packet1.flags = packet_in.flags;
        packet1.roll_authority_pct = packet_in.roll_authority_pct;
        packet1.pitch_authority_pct = packet_in.pitch_authority_pct;
        packet1.yaw_authority_pct = packet_in.yaw_authority_pct;
        packet1.thrust_authority_pct = packet_in.thrust_authority_pct;
        packet1.minimum_attitude_authority_pct = packet_in.minimum_attitude_authority_pct;
        packet1.actuator_headroom_pct = packet_in.actuator_headroom_pct;
        packet1.system_confidence_pct = packet_in.system_confidence_pct;
        packet1.recovery_progress_pct = packet_in.recovery_progress_pct;
        packet1.thrust_up = packet_in.thrust_up;
        packet1.thrust_down = packet_in.thrust_down;
        packet1.reachable_residual = packet_in.reachable_residual;
        packet1.allocation_fallback = packet_in.allocation_fallback;
        packet1.recovery_fallback = packet_in.recovery_fallback;
        packet1.arbitration_weight = packet_in.arbitration_weight;
        packet1.reentry_weight = packet_in.reentry_weight;
        packet1.allocation_active = packet_in.allocation_active;
        packet1.recovery_active = packet_in.recovery_active;

        mav_array_memcpy(packet1.positive_authority, packet_in.positive_authority, sizeof(float)*3);
        mav_array_memcpy(packet1.negative_authority, packet_in.negative_authority, sizeof(float)*3);

#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
        if (status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) {
           // cope with extensions
           memset(MAVLINK_MSG_ID_MERIVUS_FTC_CONTROL_STATUS_MIN_LEN + (char *)&packet1, 0, sizeof(packet1)-MAVLINK_MSG_ID_MERIVUS_FTC_CONTROL_STATUS_MIN_LEN);
        }
#endif
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_merivus_ftc_control_status_encode(system_id, component_id, &msg, &packet1);
    mavlink_msg_merivus_ftc_control_status_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_merivus_ftc_control_status_pack(system_id, component_id, &msg , packet1.time_usec , packet1.saturated_mask , packet1.protocol_version , packet1.system_state , packet1.authority_state , packet1.control_mode , packet1.recovery_state , packet1.flags , packet1.roll_authority_pct , packet1.pitch_authority_pct , packet1.yaw_authority_pct , packet1.thrust_authority_pct , packet1.minimum_attitude_authority_pct , packet1.actuator_headroom_pct , packet1.system_confidence_pct , packet1.recovery_progress_pct , packet1.positive_authority , packet1.negative_authority , packet1.thrust_up , packet1.thrust_down , packet1.reachable_residual , packet1.allocation_fallback , packet1.recovery_fallback , packet1.arbitration_weight , packet1.reentry_weight , packet1.allocation_active , packet1.recovery_active );
    mavlink_msg_merivus_ftc_control_status_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_merivus_ftc_control_status_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.time_usec , packet1.saturated_mask , packet1.protocol_version , packet1.system_state , packet1.authority_state , packet1.control_mode , packet1.recovery_state , packet1.flags , packet1.roll_authority_pct , packet1.pitch_authority_pct , packet1.yaw_authority_pct , packet1.thrust_authority_pct , packet1.minimum_attitude_authority_pct , packet1.actuator_headroom_pct , packet1.system_confidence_pct , packet1.recovery_progress_pct , packet1.positive_authority , packet1.negative_authority , packet1.thrust_up , packet1.thrust_down , packet1.reachable_residual , packet1.allocation_fallback , packet1.recovery_fallback , packet1.arbitration_weight , packet1.reentry_weight , packet1.allocation_active , packet1.recovery_active );
    mavlink_msg_merivus_ftc_control_status_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
            comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
    mavlink_msg_merivus_ftc_control_status_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_merivus_ftc_control_status_send(MAVLINK_COMM_1 , packet1.time_usec , packet1.saturated_mask , packet1.protocol_version , packet1.system_state , packet1.authority_state , packet1.control_mode , packet1.recovery_state , packet1.flags , packet1.roll_authority_pct , packet1.pitch_authority_pct , packet1.yaw_authority_pct , packet1.thrust_authority_pct , packet1.minimum_attitude_authority_pct , packet1.actuator_headroom_pct , packet1.system_confidence_pct , packet1.recovery_progress_pct , packet1.positive_authority , packet1.negative_authority , packet1.thrust_up , packet1.thrust_down , packet1.reachable_residual , packet1.allocation_fallback , packet1.recovery_fallback , packet1.arbitration_weight , packet1.reentry_weight , packet1.allocation_active , packet1.recovery_active );
    mavlink_msg_merivus_ftc_control_status_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

#ifdef MAVLINK_HAVE_GET_MESSAGE_INFO
    MAVLINK_ASSERT(mavlink_get_message_info_by_name("MERIVUS_FTC_CONTROL_STATUS") != NULL);
    MAVLINK_ASSERT(mavlink_get_message_info_by_id(MAVLINK_MSG_ID_MERIVUS_FTC_CONTROL_STATUS) != NULL);
#endif
}

static void mavlink_test_merivus_ftc_extreme_status(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
    mavlink_status_t *status = mavlink_get_channel_status(MAVLINK_COMM_0);
        if ((status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) && MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS >= 256) {
            return;
        }
#endif
    mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
    mavlink_merivus_ftc_extreme_status_t packet_in = {
        93372036854775807ULL,93372036854776311ULL,963498296,963498504,963498712,89,156,223,34,101,168,235,46,113,180
    };
    mavlink_merivus_ftc_extreme_status_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        packet1.time_usec = packet_in.time_usec;
        packet1.event_time_usec = packet_in.event_time_usec;
        packet1.loss_of_control_reason_mask = packet_in.loss_of_control_reason_mask;
        packet1.recovery_trigger_mask = packet_in.recovery_trigger_mask;
        packet1.recovery_inhibit_mask = packet_in.recovery_inhibit_mask;
        packet1.protocol_version = packet_in.protocol_version;
        packet1.impact_type = packet_in.impact_type;
        packet1.loc_state = packet_in.loc_state;
        packet1.recovery_state = packet_in.recovery_state;
        packet1.flags = packet_in.flags;
        packet1.impact_score_pct = packet_in.impact_score_pct;
        packet1.impact_confidence_pct = packet_in.impact_confidence_pct;
        packet1.impact_severity_pct = packet_in.impact_severity_pct;
        packet1.loss_of_control_score_pct = packet_in.loss_of_control_score_pct;
        packet1.recovery_progress_pct = packet_in.recovery_progress_pct;


#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
        if (status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) {
           // cope with extensions
           memset(MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_MIN_LEN + (char *)&packet1, 0, sizeof(packet1)-MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS_MIN_LEN);
        }
#endif
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_merivus_ftc_extreme_status_encode(system_id, component_id, &msg, &packet1);
    mavlink_msg_merivus_ftc_extreme_status_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_merivus_ftc_extreme_status_pack(system_id, component_id, &msg , packet1.time_usec , packet1.event_time_usec , packet1.loss_of_control_reason_mask , packet1.recovery_trigger_mask , packet1.recovery_inhibit_mask , packet1.protocol_version , packet1.impact_type , packet1.loc_state , packet1.recovery_state , packet1.flags , packet1.impact_score_pct , packet1.impact_confidence_pct , packet1.impact_severity_pct , packet1.loss_of_control_score_pct , packet1.recovery_progress_pct );
    mavlink_msg_merivus_ftc_extreme_status_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_merivus_ftc_extreme_status_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.time_usec , packet1.event_time_usec , packet1.loss_of_control_reason_mask , packet1.recovery_trigger_mask , packet1.recovery_inhibit_mask , packet1.protocol_version , packet1.impact_type , packet1.loc_state , packet1.recovery_state , packet1.flags , packet1.impact_score_pct , packet1.impact_confidence_pct , packet1.impact_severity_pct , packet1.loss_of_control_score_pct , packet1.recovery_progress_pct );
    mavlink_msg_merivus_ftc_extreme_status_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
            comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
    mavlink_msg_merivus_ftc_extreme_status_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_merivus_ftc_extreme_status_send(MAVLINK_COMM_1 , packet1.time_usec , packet1.event_time_usec , packet1.loss_of_control_reason_mask , packet1.recovery_trigger_mask , packet1.recovery_inhibit_mask , packet1.protocol_version , packet1.impact_type , packet1.loc_state , packet1.recovery_state , packet1.flags , packet1.impact_score_pct , packet1.impact_confidence_pct , packet1.impact_severity_pct , packet1.loss_of_control_score_pct , packet1.recovery_progress_pct );
    mavlink_msg_merivus_ftc_extreme_status_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

#ifdef MAVLINK_HAVE_GET_MESSAGE_INFO
    MAVLINK_ASSERT(mavlink_get_message_info_by_name("MERIVUS_FTC_EXTREME_STATUS") != NULL);
    MAVLINK_ASSERT(mavlink_get_message_info_by_id(MAVLINK_MSG_ID_MERIVUS_FTC_EXTREME_STATUS) != NULL);
#endif
}

static void mavlink_test_merivus_ftc_diagnostics(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
    mavlink_status_t *status = mavlink_get_channel_status(MAVLINK_COMM_0);
        if ((status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) && MAVLINK_MSG_ID_MERIVUS_FTC_DIAGNOSTICS >= 256) {
            return;
        }
#endif
    mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
    mavlink_merivus_ftc_diagnostics_t packet_in = {
        93372036854775807ULL,73.0,101.0,129.0,157.0,185.0,213.0,241.0,269.0,297.0,325.0,353.0,381.0,963500376,185,252,63,130,197,472.0,500.0,528.0,963501468,963501676,612.0,{ 640.0, 641.0, 642.0 },{ 724.0, 725.0, 726.0 },152,219,30,97
    };
    mavlink_merivus_ftc_diagnostics_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        packet1.time_usec = packet_in.time_usec;
        packet1.model_residual = packet_in.model_residual;
        packet1.excitation = packet_in.excitation;
        packet1.maneuver_intensity = packet_in.maneuver_intensity;
        packet1.external_disturbance_score = packet_in.external_disturbance_score;
        packet1.vibration_score = packet_in.vibration_score;
        packet1.allocation_residual_norm = packet_in.allocation_residual_norm;
        packet1.attitude_error = packet_in.attitude_error;
        packet1.rate_error = packet_in.rate_error;
        packet1.jerk = packet_in.jerk;
        packet1.acceleration_magnitude = packet_in.acceleration_magnitude;
        packet1.angular_rate = packet_in.angular_rate;
        packet1.angular_acceleration = packet_in.angular_acceleration;
        packet1.system_reason_mask = packet_in.system_reason_mask;
        packet1.protocol_version = packet_in.protocol_version;
        packet1.flags = packet_in.flags;
        packet1.simulation_motor_index = packet_in.simulation_motor_index;
        packet1.simulation_target_effectiveness_pct = packet_in.simulation_target_effectiveness_pct;
        packet1.simulation_applied_effectiveness_pct = packet_in.simulation_applied_effectiveness_pct;
        packet1.condition_number = packet_in.condition_number;
        packet1.rigid_body_activity = packet_in.rigid_body_activity;
        packet1.model_prediction_residual = packet_in.model_prediction_residual;
        packet1.update_count = packet_in.update_count;
        packet1.reset_count = packet_in.reset_count;
        packet1.mass = packet_in.mass;
        packet1.mass_state = packet_in.mass_state;
        packet1.inertia_state = packet_in.inertia_state;
        packet1.cg_state = packet_in.cg_state;
        packet1.estimator_flags = packet_in.estimator_flags;

        mav_array_memcpy(packet1.inertia, packet_in.inertia, sizeof(float)*3);
        mav_array_memcpy(packet1.cg_offset, packet_in.cg_offset, sizeof(float)*3);

#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
        if (status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) {
           // cope with extensions
           memset(MAVLINK_MSG_ID_MERIVUS_FTC_DIAGNOSTICS_MIN_LEN + (char *)&packet1, 0, sizeof(packet1)-MAVLINK_MSG_ID_MERIVUS_FTC_DIAGNOSTICS_MIN_LEN);
        }
#endif
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_merivus_ftc_diagnostics_encode(system_id, component_id, &msg, &packet1);
    mavlink_msg_merivus_ftc_diagnostics_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_merivus_ftc_diagnostics_pack(system_id, component_id, &msg , packet1.time_usec , packet1.model_residual , packet1.excitation , packet1.maneuver_intensity , packet1.external_disturbance_score , packet1.vibration_score , packet1.allocation_residual_norm , packet1.attitude_error , packet1.rate_error , packet1.jerk , packet1.acceleration_magnitude , packet1.angular_rate , packet1.angular_acceleration , packet1.system_reason_mask , packet1.protocol_version , packet1.flags , packet1.simulation_motor_index , packet1.simulation_target_effectiveness_pct , packet1.simulation_applied_effectiveness_pct , packet1.condition_number , packet1.rigid_body_activity , packet1.model_prediction_residual , packet1.update_count , packet1.reset_count , packet1.mass , packet1.inertia , packet1.cg_offset , packet1.mass_state , packet1.inertia_state , packet1.cg_state , packet1.estimator_flags );
    mavlink_msg_merivus_ftc_diagnostics_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_merivus_ftc_diagnostics_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.time_usec , packet1.model_residual , packet1.excitation , packet1.maneuver_intensity , packet1.external_disturbance_score , packet1.vibration_score , packet1.allocation_residual_norm , packet1.attitude_error , packet1.rate_error , packet1.jerk , packet1.acceleration_magnitude , packet1.angular_rate , packet1.angular_acceleration , packet1.system_reason_mask , packet1.protocol_version , packet1.flags , packet1.simulation_motor_index , packet1.simulation_target_effectiveness_pct , packet1.simulation_applied_effectiveness_pct , packet1.condition_number , packet1.rigid_body_activity , packet1.model_prediction_residual , packet1.update_count , packet1.reset_count , packet1.mass , packet1.inertia , packet1.cg_offset , packet1.mass_state , packet1.inertia_state , packet1.cg_state , packet1.estimator_flags );
    mavlink_msg_merivus_ftc_diagnostics_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
            comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
    mavlink_msg_merivus_ftc_diagnostics_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_merivus_ftc_diagnostics_send(MAVLINK_COMM_1 , packet1.time_usec , packet1.model_residual , packet1.excitation , packet1.maneuver_intensity , packet1.external_disturbance_score , packet1.vibration_score , packet1.allocation_residual_norm , packet1.attitude_error , packet1.rate_error , packet1.jerk , packet1.acceleration_magnitude , packet1.angular_rate , packet1.angular_acceleration , packet1.system_reason_mask , packet1.protocol_version , packet1.flags , packet1.simulation_motor_index , packet1.simulation_target_effectiveness_pct , packet1.simulation_applied_effectiveness_pct , packet1.condition_number , packet1.rigid_body_activity , packet1.model_prediction_residual , packet1.update_count , packet1.reset_count , packet1.mass , packet1.inertia , packet1.cg_offset , packet1.mass_state , packet1.inertia_state , packet1.cg_state , packet1.estimator_flags );
    mavlink_msg_merivus_ftc_diagnostics_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

#ifdef MAVLINK_HAVE_GET_MESSAGE_INFO
    MAVLINK_ASSERT(mavlink_get_message_info_by_name("MERIVUS_FTC_DIAGNOSTICS") != NULL);
    MAVLINK_ASSERT(mavlink_get_message_info_by_id(MAVLINK_MSG_ID_MERIVUS_FTC_DIAGNOSTICS) != NULL);
#endif
}

static void mavlink_test_merivus_ftc(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
    mavlink_test_merivus_ftc_motor_status(system_id, component_id, last_msg);
    mavlink_test_merivus_ftc_control_status(system_id, component_id, last_msg);
    mavlink_test_merivus_ftc_extreme_status(system_id, component_id, last_msg);
    mavlink_test_merivus_ftc_diagnostics(system_id, component_id, last_msg);
}

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // MERIVUS_FTC_TESTSUITE_H
