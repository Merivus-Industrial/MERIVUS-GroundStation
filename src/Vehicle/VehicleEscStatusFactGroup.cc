/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "VehicleEscStatusFactGroup.h"
#include "Vehicle.h"

const char* VehicleEscStatusFactGroup::_indexFactName =                             "index";
const char* VehicleEscStatusFactGroup::_countFactName =                             "count";
const char* VehicleEscStatusFactGroup::_connectionTypeFactName =                    "connectionType";
const char* VehicleEscStatusFactGroup::_onlineFlagsFactName =                       "onlineFlags";

const char* VehicleEscStatusFactGroup::_rpmFirstFactName =                          "rpm1";
const char* VehicleEscStatusFactGroup::_rpmSecondFactName =                         "rpm2";
const char* VehicleEscStatusFactGroup::_rpmThirdFactName =                          "rpm3";
const char* VehicleEscStatusFactGroup::_rpmFourthFactName =                         "rpm4";

const char* VehicleEscStatusFactGroup::_currentFirstFactName =                      "current1";
const char* VehicleEscStatusFactGroup::_currentSecondFactName =                     "current2";
const char* VehicleEscStatusFactGroup::_currentThirdFactName =                      "current3";
const char* VehicleEscStatusFactGroup::_currentFourthFactName =                     "current4";

const char* VehicleEscStatusFactGroup::_voltageFirstFactName =                      "voltage1";
const char* VehicleEscStatusFactGroup::_voltageSecondFactName =                     "voltage2";
const char* VehicleEscStatusFactGroup::_voltageThirdFactName =                      "voltage3";
const char* VehicleEscStatusFactGroup::_voltageFourthFactName =                     "voltage4";

const char* VehicleEscStatusFactGroup::_temperatureFirstFactName =                  "temperature1";
const char* VehicleEscStatusFactGroup::_temperatureSecondFactName =                 "temperature2";
const char* VehicleEscStatusFactGroup::_temperatureThirdFactName =                  "temperature3";
const char* VehicleEscStatusFactGroup::_temperatureFourthFactName =                 "temperature4";

VehicleEscStatusFactGroup::VehicleEscStatusFactGroup(QObject* parent)
    : FactGroup                         (1000, ":/json/Vehicle/EscStatusFactGroup.json", parent)
    , _indexFact                        (0, _indexFactName,                         FactMetaData::valueTypeUint8)
    , _countFact                        (0, _countFactName,                         FactMetaData::valueTypeUint8)
    , _connectionTypeFact               (0, _connectionTypeFactName,                FactMetaData::valueTypeUint8)
    , _onlineFlagsFact                  (0, _onlineFlagsFactName,                    FactMetaData::valueTypeUint8)

    , _rpmFirstFact                     (0, _rpmFirstFactName,                      FactMetaData::valueTypeFloat)
    , _rpmSecondFact                    (0, _rpmSecondFactName,                     FactMetaData::valueTypeFloat)
    , _rpmThirdFact                     (0, _rpmThirdFactName,                      FactMetaData::valueTypeFloat)
    , _rpmFourthFact                    (0, _rpmFourthFactName,                     FactMetaData::valueTypeFloat)

    , _currentFirstFact                 (0, _currentFirstFactName,                  FactMetaData::valueTypeFloat)
    , _currentSecondFact                (0, _currentSecondFactName,                 FactMetaData::valueTypeFloat)
    , _currentThirdFact                 (0, _currentThirdFactName,                  FactMetaData::valueTypeFloat)
    , _currentFourthFact                (0, _currentFourthFactName,                 FactMetaData::valueTypeFloat)

    , _voltageFirstFact                 (0, _voltageFirstFactName,                  FactMetaData::valueTypeFloat)
    , _voltageSecondFact                (0, _voltageSecondFactName,                 FactMetaData::valueTypeFloat)
    , _voltageThirdFact                 (0, _voltageThirdFactName,                  FactMetaData::valueTypeFloat)
    , _voltageFourthFact                (0, _voltageFourthFactName,                 FactMetaData::valueTypeFloat)

    , _temperatureFirstFact             (0, _temperatureFirstFactName,              FactMetaData::valueTypeFloat)
    , _temperatureSecondFact            (0, _temperatureSecondFactName,             FactMetaData::valueTypeFloat)
    , _temperatureThirdFact             (0, _temperatureThirdFactName,              FactMetaData::valueTypeFloat)
    , _temperatureFourthFact            (0, _temperatureFourthFactName,             FactMetaData::valueTypeFloat)
{
    _addFact(&_indexFact,                       _indexFactName);
    _addFact(&_countFact,                       _countFactName);
    _addFact(&_connectionTypeFact,              _connectionTypeFactName);
    _addFact(&_onlineFlagsFact,                 _onlineFlagsFactName);

    _addFact(&_rpmFirstFact,                    _rpmFirstFactName);
    _addFact(&_rpmSecondFact,                   _rpmSecondFactName);
    _addFact(&_rpmThirdFact,                    _rpmThirdFactName);
    _addFact(&_rpmFourthFact,                   _rpmFourthFactName);

    _addFact(&_currentFirstFact,                _currentFirstFactName);
    _addFact(&_currentSecondFact,               _currentSecondFactName);
    _addFact(&_currentThirdFact,                _currentThirdFactName);
    _addFact(&_currentFourthFact,               _currentFourthFactName);

    _addFact(&_voltageFirstFact,                _voltageFirstFactName);
    _addFact(&_voltageSecondFact,               _voltageSecondFactName);
    _addFact(&_voltageThirdFact,                _voltageThirdFactName);
    _addFact(&_voltageFourthFact,               _voltageFourthFactName);

    _addFact(&_temperatureFirstFact,            _temperatureFirstFactName);
    _addFact(&_temperatureSecondFact,           _temperatureSecondFactName);
    _addFact(&_temperatureThirdFact,            _temperatureThirdFactName);
    _addFact(&_temperatureFourthFact,           _temperatureFourthFactName);
}

void VehicleEscStatusFactGroup::handleMessage(Vehicle* /* vehicle */, mavlink_message_t& message)
{
    if (message.msgid == MAVLINK_MSG_ID_ESC_INFO) {
        mavlink_esc_info_t content;
        mavlink_msg_esc_info_decode(&message, &content);

        if (content.index != 0) {
            return;
        }

        if (!_infoReceived) {
            _infoReceived = true;
            emit infoReceivedChanged();
        }

        count()->setRawValue(content.count);
        connectionType()->setRawValue(content.connection_type);
        onlineFlags()->setRawValue(content.info);
        temperatureFirst()->setRawValue(content.temperature[0] == INT16_MAX ? qQNaN() : content.temperature[0] / 100.f);
        temperatureSecond()->setRawValue(content.temperature[1] == INT16_MAX ? qQNaN() : content.temperature[1] / 100.f);
        temperatureThird()->setRawValue(content.temperature[2] == INT16_MAX ? qQNaN() : content.temperature[2] / 100.f);
        temperatureFourth()->setRawValue(content.temperature[3] == INT16_MAX ? qQNaN() : content.temperature[3] / 100.f);
        return;
    }

    if (message.msgid != MAVLINK_MSG_ID_ESC_STATUS) {
        return;
    }

    mavlink_esc_status_t content;
    mavlink_msg_esc_status_decode(&message, &content);

    // This FactGroup represents motors 1-4. ESC_STATUS uses index as the first
    // ESC in the four-item block, so a later block must not overwrite it.
    if (content.index != 0) {
        return;
    }

    if (!_received) {
        _received = true;
        emit receivedChanged();
    }

    index()->setRawValue                        (content.index);

    rpmFirst()->setRawValue                     (content.rpm[0]);
    rpmSecond()->setRawValue                    (content.rpm[1]);
    rpmThird()->setRawValue                     (content.rpm[2]);
    rpmFourth()->setRawValue                    (content.rpm[3]);

    currentFirst()->setRawValue                 (content.current[0]);
    currentSecond()->setRawValue                (content.current[1]);
    currentThird()->setRawValue                 (content.current[2]);
    currentFourth()->setRawValue                (content.current[3]);

    voltageFirst()->setRawValue                 (content.voltage[0]);
    voltageSecond()->setRawValue                (content.voltage[1]);
    voltageThird()->setRawValue                 (content.voltage[2]);
    voltageFourth()->setRawValue                (content.voltage[3]);
}
