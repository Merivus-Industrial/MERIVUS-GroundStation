/****************************************************************************
 *
 * (c) 2022 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "HealthAndArmingCheckReport.h"
#include "QGCMAVLink.h"

#include <algorithm>

#include <libevents/libs/cpp/generated/events_generated.h>

namespace {

bool isPositionFailure(const events::HealthAndArmingChecks::Check& check)
{
    if (check.health_component) {
        const QString componentName = QString::fromStdString(check.health_component->name).toLower();
        if (componentName == QStringLiteral("gps")
            || componentName.contains(QStringLiteral("position"))) {
            return true;
        }
    }

    const QString message = QString::fromStdString(check.message).toLower();
    return message.contains(QStringLiteral("position"))
        || message.contains(QStringLiteral("gps"))
        || message.contains(QStringLiteral("gnss"));
}

QString translateTakeoffFailureReason(const QString& reason)
{
    if (reason == QStringLiteral("No valid local position estimate")) {
        return HealthAndArmingCheckReport::tr("No valid local position estimate");
    }
    if (reason == QStringLiteral("No valid global position estimate")) {
        return HealthAndArmingCheckReport::tr("No valid global position estimate");
    }
    if (reason == QStringLiteral("No valid altitude estimate")) {
        return HealthAndArmingCheckReport::tr("No valid altitude estimate");
    }
    if (reason == QStringLiteral("GPS fix too low")) {
        return HealthAndArmingCheckReport::tr("GPS fix too low");
    }
    if (reason == QStringLiteral("Not enough GPS Satellites")) {
        return HealthAndArmingCheckReport::tr("Not enough GPS Satellites");
    }
    if (reason == QStringLiteral("GPS PDOP too high")) {
        return HealthAndArmingCheckReport::tr("GPS PDOP too high");
    }
    if (reason == QStringLiteral("GPS Horizontal Position Error too high")) {
        return HealthAndArmingCheckReport::tr("GPS Horizontal Position Error too high");
    }
    if (reason == QStringLiteral("GPS Vertical Position Error too high")) {
        return HealthAndArmingCheckReport::tr("GPS Vertical Position Error too high");
    }
    if (reason == QStringLiteral("GPS Speed Accuracy too low")) {
        return HealthAndArmingCheckReport::tr("GPS Speed Accuracy too low");
    }
    if (reason == QStringLiteral("GPS Horizontal Position Drift too high")) {
        return HealthAndArmingCheckReport::tr("GPS Horizontal Position Drift too high");
    }
    if (reason == QStringLiteral("GPS Vertical Position Drift too high")) {
        return HealthAndArmingCheckReport::tr("GPS Vertical Position Drift too high");
    }
    if (reason == QStringLiteral("GPS Horizontal Speed Drift too high")) {
        return HealthAndArmingCheckReport::tr("GPS Horizontal Speed Drift too high");
    }
    if (reason == QStringLiteral("GPS Vertical Speed Drift too high")) {
        return HealthAndArmingCheckReport::tr("GPS Vertical Speed Drift too high");
    }
    if (reason == QStringLiteral("Estimator not using GPS")) {
        return HealthAndArmingCheckReport::tr("Estimator not using GPS");
    }
    if (reason == QStringLiteral("Poor GPS Quality")) {
        return HealthAndArmingCheckReport::tr("Poor GPS Quality");
    }
    return reason;
}

}

HealthAndArmingCheckReport::HealthAndArmingCheckReport() = default;

HealthAndArmingCheckReport::~HealthAndArmingCheckReport()
{
    _problemsForCurrentMode->clearAndDeleteContents();
}

void HealthAndArmingCheckReport::update(uint8_t compid, const events::HealthAndArmingChecks::Results& results,
        int flightModeGroup, bool healthResultsUpdated)
{
    if (compid != MAV_COMP_ID_AUTOPILOT1) {
        // only autopilot supported atm
        return;
    }
    if (flightModeGroup == -1) {
        qWarning() << "Flight mode group not set";
        return;
    }
    _supported = true;

    _problemsForCurrentMode->clearAndDeleteContents();
    _hasWarningsOrErrors = false;
    for (const auto& check : results.checks(flightModeGroup)) {
        QString severity = "";
        if (events::externalLogLevel(check.log_levels) <= events::Log::Error) {
            severity = "error";
            _hasWarningsOrErrors = true;
        } else if (events::externalLogLevel(check.log_levels) <= events::Log::Warning) {
            severity = "warning";
            _hasWarningsOrErrors = true;
        }
        QString description = QString::fromStdString(check.description);
        _problemsForCurrentMode->append(new HealthAndArmingCheckProblem(QString::fromStdString(check.message),
                description.replace("\n", "<br/>"), severity));
    }

    _canArm = results.canArm(flightModeGroup);
    if (_missionModeGroup != -1) {
        // TODO: use results.canRun(_missionModeGroup) while armed
        _canStartMission = results.canArm(_missionModeGroup);
    }
    if (_takeoffModeGroup != -1) {
        _canTakeoff = results.canArm(_takeoffModeGroup);
        _takeoffFailureReasons.clear();
        _takeoffHasPositionFailure = false;
        for (const auto& check : results.checks(_takeoffModeGroup)) {
            if (events::externalLogLevel(check.log_levels) > events::Log::Error) {
                continue;
            }

            const QString reason = translateTakeoffFailureReason(QString::fromStdString(check.message));
            if (!reason.isEmpty() && !_takeoffFailureReasons.contains(reason)) {
                _takeoffFailureReasons.append(reason);
            }
            _takeoffHasPositionFailure = _takeoffHasPositionFailure || isPositionFailure(check);
        }
    }

    const auto& healthComponents = results.healthComponents().health_components;

    // GPS state
    const auto gpsStateIter = healthComponents.find("gps");
    if (gpsStateIter != healthComponents.end()) {
        const events::HealthAndArmingChecks::HealthComponent& gpsState = gpsStateIter->second;
        if (gpsState.health.error || gpsState.arming_check.error) {
            _gpsState = "red";
        } else if (gpsState.health.warning || gpsState.arming_check.warning) {
            _gpsState = "yellow";
        } else {
            _gpsState = "green";
        }
    }

    if (healthResultsUpdated) {
        ++_updateSequence;
    }
    emit updated();
}

void HealthAndArmingCheckReport::setModeGroups(int takeoffModeGroup, int missionModeGroup)
{
    _takeoffModeGroup = takeoffModeGroup;
    _missionModeGroup = missionModeGroup;
}

QString HealthAndArmingCheckReport::takeoffFailureMessage(int maximumReasons) const
{
    QString message = _takeoffHasPositionFailure
        ? tr("No valid local position/GPS accuracy check failed.")
        : tr("Takeoff check failed.");

    const int reasonCount = std::min(std::max(maximumReasons, 0), _takeoffFailureReasons.count());
    for (int i = 0; i < reasonCount; ++i) {
        message += QStringLiteral("\n\u2022 ") + _takeoffFailureReasons.at(i);
    }

    const int remainingCount = _takeoffFailureReasons.count() - reasonCount;
    if (remainingCount > 0) {
        message += QStringLiteral("\n")
            + tr("%1 additional issue(s); see the preflight check panel.").arg(remainingCount);
    }

    return message;
}
