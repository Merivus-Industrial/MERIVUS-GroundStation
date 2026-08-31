/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include "FactGroup.h"
#include "QGCMAVLink.h"

class Vehicle;

class VehicleEscStatusFactGroup : public FactGroup
{
    Q_OBJECT

public:
    VehicleEscStatusFactGroup(QObject* parent = nullptr);

    Q_PROPERTY(bool received READ received NOTIFY receivedChanged)
    Q_PROPERTY(bool infoReceived READ infoReceived NOTIFY infoReceivedChanged)

    Q_PROPERTY(Fact* index              READ index              CONSTANT)
    Q_PROPERTY(Fact* count              READ count              CONSTANT)
    Q_PROPERTY(Fact* connectionType     READ connectionType     CONSTANT)
    Q_PROPERTY(Fact* onlineFlags        READ onlineFlags        CONSTANT)

    Q_PROPERTY(Fact* rpmFirst           READ rpmFirst           CONSTANT)
    Q_PROPERTY(Fact* rpmSecond          READ rpmSecond          CONSTANT)
    Q_PROPERTY(Fact* rpmThird           READ rpmThird           CONSTANT)
    Q_PROPERTY(Fact* rpmFourth          READ rpmFourth          CONSTANT)

    Q_PROPERTY(Fact* currentFirst       READ currentFirst       CONSTANT)
    Q_PROPERTY(Fact* currentSecond      READ currentSecond      CONSTANT)
    Q_PROPERTY(Fact* currentThird       READ currentThird       CONSTANT)
    Q_PROPERTY(Fact* currentFourth      READ currentFourth      CONSTANT)

    Q_PROPERTY(Fact* voltageFirst       READ voltageFirst       CONSTANT)
    Q_PROPERTY(Fact* voltageSecond      READ voltageSecond      CONSTANT)
    Q_PROPERTY(Fact* voltageThird       READ voltageThird       CONSTANT)
    Q_PROPERTY(Fact* voltageFourth      READ voltageFourth      CONSTANT)

    Q_PROPERTY(Fact* temperatureFirst   READ temperatureFirst   CONSTANT)
    Q_PROPERTY(Fact* temperatureSecond  READ temperatureSecond  CONSTANT)
    Q_PROPERTY(Fact* temperatureThird   READ temperatureThird   CONSTANT)
    Q_PROPERTY(Fact* temperatureFourth  READ temperatureFourth  CONSTANT)

    Fact* index                         () { return &_indexFact; }
    Fact* count                         () { return &_countFact; }
    Fact* connectionType                () { return &_connectionTypeFact; }
    Fact* onlineFlags                   () { return &_onlineFlagsFact; }

    Fact* rpmFirst                      () { return &_rpmFirstFact; }
    Fact* rpmSecond                     () { return &_rpmSecondFact; }
    Fact* rpmThird                      () { return &_rpmThirdFact; }
    Fact* rpmFourth                     () { return &_rpmFourthFact; }

    Fact* currentFirst                  () { return &_currentFirstFact; }
    Fact* currentSecond                 () { return &_currentSecondFact; }
    Fact* currentThird                  () { return &_currentThirdFact; }
    Fact* currentFourth                 () { return &_currentFourthFact; }

    Fact* voltageFirst                  () { return &_voltageFirstFact; }
    Fact* voltageSecond                 () { return &_voltageSecondFact; }
    Fact* voltageThird                  () { return &_voltageThirdFact; }
    Fact* voltageFourth                 () { return &_voltageFourthFact; }

    Fact* temperatureFirst              () { return &_temperatureFirstFact; }
    Fact* temperatureSecond             () { return &_temperatureSecondFact; }
    Fact* temperatureThird              () { return &_temperatureThirdFact; }
    Fact* temperatureFourth             () { return &_temperatureFourthFact; }

    bool received                       () const { return _received; }
    bool infoReceived                   () const { return _infoReceived; }

    // Overrides from FactGroup
    void handleMessage(Vehicle* vehicle, mavlink_message_t& message) override;

    static const char* _indexFactName;
    static const char* _countFactName;
    static const char* _connectionTypeFactName;
    static const char* _onlineFlagsFactName;

    static const char* _rpmFirstFactName;
    static const char* _rpmSecondFactName;
    static const char* _rpmThirdFactName;
    static const char* _rpmFourthFactName;

    static const char* _currentFirstFactName;
    static const char* _currentSecondFactName;
    static const char* _currentThirdFactName;
    static const char* _currentFourthFactName;

    static const char* _voltageFirstFactName;
    static const char* _voltageSecondFactName;
    static const char* _voltageThirdFactName;
    static const char* _voltageFourthFactName;

    static const char* _temperatureFirstFactName;
    static const char* _temperatureSecondFactName;
    static const char* _temperatureThirdFactName;
    static const char* _temperatureFourthFactName;

signals:
    void receivedChanged();
    void infoReceivedChanged();

private:
    bool _received = false;
    bool _infoReceived = false;

    Fact _indexFact;
    Fact _countFact;
    Fact _connectionTypeFact;
    Fact _onlineFlagsFact;

    Fact _rpmFirstFact;
    Fact _rpmSecondFact;
    Fact _rpmThirdFact;
    Fact _rpmFourthFact;

    Fact _currentFirstFact;
    Fact _currentSecondFact;
    Fact _currentThirdFact;
    Fact _currentFourthFact;

    Fact _voltageFirstFact;
    Fact _voltageSecondFact;
    Fact _voltageThirdFact;
    Fact _voltageFourthFact;

    Fact _temperatureFirstFact;
    Fact _temperatureSecondFact;
    Fact _temperatureThirdFact;
    Fact _temperatureFourthFact;
};
