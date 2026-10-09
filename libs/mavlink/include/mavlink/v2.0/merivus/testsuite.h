/** @file
 *    @brief MAVLink comm protocol testsuite generated from merivus.xml
 *    @see https://mavlink.io/en/
 */
#pragma once
#ifndef MERIVUS_TESTSUITE_H
#define MERIVUS_TESTSUITE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAVLINK_TEST_ALL
#define MAVLINK_TEST_ALL
static void mavlink_test_all(uint8_t, uint8_t, mavlink_message_t *last_msg);
static void mavlink_test_merivus_ftc(uint8_t, uint8_t, mavlink_message_t *last_msg);
static void mavlink_test_merivus(uint8_t, uint8_t, mavlink_message_t *last_msg);

static void mavlink_test_all(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
    mavlink_test_all(system_id, component_id, last_msg);
    mavlink_test_merivus_ftc(system_id, component_id, last_msg);
    mavlink_test_merivus(system_id, component_id, last_msg);
}
#endif

#include "../all/testsuite.h"
#include "../merivus_ftc/testsuite.h"



static void mavlink_test_merivus(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{

}

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // MERIVUS_TESTSUITE_H
