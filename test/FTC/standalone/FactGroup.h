#pragma once
#include <QObject>
#include "QGCMAVLink.h"
class Vehicle;
// Isolate the production FTC decoder/model from the application-owned Fact registry.
class FactGroup : public QObject
{
public:
    explicit FactGroup(int, QObject* parent = nullptr) : QObject(parent) {}
    virtual void handleMessage(Vehicle*, mavlink_message_t&) {}
protected:
    void _setTelemetryAvailable(bool) {}
};
