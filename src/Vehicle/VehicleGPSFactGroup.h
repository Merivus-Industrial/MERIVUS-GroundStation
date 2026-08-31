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

class VehicleGPSFactGroup : public FactGroup
{
    Q_OBJECT

public:
    VehicleGPSFactGroup(QObject* parent = nullptr);

    Q_PROPERTY(Fact* lat                READ lat                CONSTANT)
    Q_PROPERTY(Fact* lon                READ lon                CONSTANT)
    Q_PROPERTY(Fact* mgrs               READ mgrs               CONSTANT)
    Q_PROPERTY(Fact* hdop               READ hdop               CONSTANT)
    Q_PROPERTY(Fact* vdop               READ vdop               CONSTANT)
    Q_PROPERTY(Fact* altitudeMSL        READ altitudeMSL        CONSTANT)
    Q_PROPERTY(Fact* altitudeEllipsoid  READ altitudeEllipsoid  CONSTANT)
    Q_PROPERTY(Fact* groundSpeed        READ groundSpeed        CONSTANT)
    Q_PROPERTY(Fact* courseOverGround   READ courseOverGround   CONSTANT)
    Q_PROPERTY(Fact* horizontalAccuracy READ horizontalAccuracy CONSTANT)
    Q_PROPERTY(Fact* verticalAccuracy   READ verticalAccuracy   CONSTANT)
    Q_PROPERTY(Fact* speedAccuracy      READ speedAccuracy      CONSTANT)
    Q_PROPERTY(Fact* headingAccuracy    READ headingAccuracy    CONSTANT)
    Q_PROPERTY(Fact* yaw                READ yaw                CONSTANT)
    Q_PROPERTY(Fact* differentialAge    READ differentialAge    CONSTANT)
    Q_PROPERTY(Fact* differentialCount  READ differentialCount  CONSTANT)
    Q_PROPERTY(Fact* count              READ count              CONSTANT)
    Q_PROPERTY(Fact* lock               READ lock               CONSTANT)

    Fact* lat               () { return &_latFact; }
    Fact* lon               () { return &_lonFact; }
    Fact* mgrs              () { return &_mgrsFact; }
    Fact* hdop              () { return &_hdopFact; }
    Fact* vdop              () { return &_vdopFact; }
    Fact* altitudeMSL       () { return &_altitudeMSLFact; }
    Fact* altitudeEllipsoid () { return &_altitudeEllipsoidFact; }
    Fact* groundSpeed       () { return &_groundSpeedFact; }
    Fact* courseOverGround  () { return &_courseOverGroundFact; }
    Fact* horizontalAccuracy() { return &_horizontalAccuracyFact; }
    Fact* verticalAccuracy  () { return &_verticalAccuracyFact; }
    Fact* speedAccuracy     () { return &_speedAccuracyFact; }
    Fact* headingAccuracy   () { return &_headingAccuracyFact; }
    Fact* yaw               () { return &_yawFact; }
    Fact* differentialAge   () { return &_differentialAgeFact; }
    Fact* differentialCount () { return &_differentialCountFact; }
    Fact* count             () { return &_countFact; }
    Fact* lock              () { return &_lockFact; }

    // Overrides from FactGroup
    virtual void handleMessage(Vehicle* vehicle, mavlink_message_t& message) override;

    static const char* _latFactName;
    static const char* _lonFactName;
    static const char* _mgrsFactName;
    static const char* _hdopFactName;
    static const char* _vdopFactName;
    static const char* _altitudeMSLFactName;
    static const char* _altitudeEllipsoidFactName;
    static const char* _groundSpeedFactName;
    static const char* _courseOverGroundFactName;
    static const char* _horizontalAccuracyFactName;
    static const char* _verticalAccuracyFactName;
    static const char* _speedAccuracyFactName;
    static const char* _headingAccuracyFactName;
    static const char* _yawFactName;
    static const char* _differentialAgeFactName;
    static const char* _differentialCountFactName;
    static const char* _countFactName;
    static const char* _lockFactName;

protected:
    void _handleGpsRawInt   (mavlink_message_t& message);
    void _handleHighLatency (mavlink_message_t& message);
    void _handleHighLatency2(mavlink_message_t& message);

    Fact _latFact;
    Fact _lonFact;
    Fact _mgrsFact;
    Fact _hdopFact;
    Fact _vdopFact;
    Fact _altitudeMSLFact;
    Fact _altitudeEllipsoidFact;
    Fact _groundSpeedFact;
    Fact _courseOverGroundFact;
    Fact _horizontalAccuracyFact;
    Fact _verticalAccuracyFact;
    Fact _speedAccuracyFact;
    Fact _headingAccuracyFact;
    Fact _yawFact;
    Fact _differentialAgeFact;
    Fact _differentialCountFact;
    Fact _countFact;
    Fact _lockFact;
};
