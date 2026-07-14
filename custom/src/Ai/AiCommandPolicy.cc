#include "AiCommandPolicy.h"

#include <QSet>

ActionProposal AiCommandPolicy::evaluate(ActionProposal proposal)
{
    proposal.executable = false;

    if (!proposal.hasProposal) {
        proposal.policyDecision = ActionProposal::PolicyDecision::PreviewOnly;
        proposal.localRisk = ActionProposal::RiskLevel::Informational;
        proposal.requiresConfirmation = false;
        proposal.reason = QStringLiteral("No structured proposal to evaluate.");
        return proposal;
    }

    if (proposal.validationStatus != ActionProposal::ValidationStatus::Valid) {
        proposal.policyDecision = ActionProposal::PolicyDecision::Deny;
        proposal.localRisk = ActionProposal::RiskLevel::Critical;
        proposal.requiresConfirmation = false;
        if (proposal.reason.isEmpty()) {
            proposal.reason = QStringLiteral("Schema validation failed.");
        }
        return proposal;
    }

    QString argumentError;
    if (!_validateArgumentsForCommand(proposal.command, proposal.arguments, &argumentError)) {
        proposal.validationStatus = ActionProposal::ValidationStatus::InvalidArguments;
        proposal.policyDecision = ActionProposal::PolicyDecision::Deny;
        proposal.localRisk = ActionProposal::RiskLevel::Critical;
        proposal.requiresConfirmation = false;
        proposal.reason = argumentError;
        return proposal;
    }

    if (_isReadOnlyCommand(proposal.command)) {
        proposal.policyDecision = ActionProposal::PolicyDecision::AllowReadOnly;
        proposal.localRisk = proposal.command == QStringLiteral("log.explain_error")
                             ? ActionProposal::RiskLevel::Informational
                             : ActionProposal::RiskLevel::Low;
        proposal.requiresConfirmation = false;
        proposal.reason = QStringLiteral("Read-only proposal allowed for display only.");
        return proposal;
    }

    if (_isUiOnlyCommand(proposal.command)) {
        proposal.policyDecision = ActionProposal::PolicyDecision::AllowUiOnly;
        proposal.localRisk = ActionProposal::RiskLevel::Low;
        proposal.requiresConfirmation = false;
        proposal.reason = QStringLiteral("UI-only proposal is allowed for display only; no UI action is executed in this phase.");
        return proposal;
    }

    if (_isMissionPreviewCommand(proposal.command)) {
        proposal.policyDecision = ActionProposal::PolicyDecision::PreviewOnly;
        proposal.localRisk = ActionProposal::RiskLevel::Medium;
        proposal.requiresConfirmation = false;
        proposal.reason = QStringLiteral("Mission proposal may be previewed only; no upload or execution is available.");
        return proposal;
    }

    if (_isCriticalDeniedCommand(proposal.command)) {
        proposal.policyDecision = ActionProposal::PolicyDecision::Deny;
        proposal.localRisk = ActionProposal::RiskLevel::Critical;
        proposal.requiresConfirmation = true;
        proposal.reason = QStringLiteral("Command is denied by local policy.");
        return proposal;
    }

    if (_isHighRiskPreviewCommand(proposal.command)) {
        proposal.policyDecision = ActionProposal::PolicyDecision::PreviewOnly;
        proposal.localRisk = ActionProposal::RiskLevel::High;
        proposal.requiresConfirmation = true;
        proposal.reason = QStringLiteral("Flight-affecting command is preview-only; MERIVUS does not execute AI flight actions.");
        return proposal;
    }

    proposal.validationStatus = ActionProposal::ValidationStatus::UnknownCommand;
    proposal.policyDecision = ActionProposal::PolicyDecision::Deny;
    proposal.localRisk = ActionProposal::RiskLevel::Critical;
    proposal.requiresConfirmation = false;
    proposal.reason = QStringLiteral("Unknown command denied by local policy.");
    return proposal;
}

bool AiCommandPolicy::_isReadOnlyCommand(const QString& command)
{
    static const QSet<QString> commands = {
        QStringLiteral("vehicle.query_status"),
        QStringLiteral("vehicle.query_battery"),
        QStringLiteral("vehicle.query_position"),
        QStringLiteral("vehicle.query_rtk"),
        QStringLiteral("log.explain_error"),
    };
    return commands.contains(command);
}

bool AiCommandPolicy::_isUiOnlyCommand(const QString& command)
{
    static const QSet<QString> commands = {
        QStringLiteral("ui.select_vehicle"),
        QStringLiteral("ui.open_page"),
        QStringLiteral("map.focus_coordinate"),
    };
    return commands.contains(command);
}

bool AiCommandPolicy::_isMissionPreviewCommand(const QString& command)
{
    static const QSet<QString> commands = {
        QStringLiteral("mission.create_draft"),
        QStringLiteral("mission.analyze"),
    };
    return commands.contains(command);
}

bool AiCommandPolicy::_isHighRiskPreviewCommand(const QString& command)
{
    static const QSet<QString> commands = {
        QStringLiteral("vehicle.arm"),
        QStringLiteral("vehicle.force_arm"),
        QStringLiteral("vehicle.takeoff"),
        QStringLiteral("vehicle.land"),
        QStringLiteral("vehicle.rtl"),
        QStringLiteral("vehicle.pause"),
        QStringLiteral("vehicle.goto"),
        QStringLiteral("mission.upload"),
        QStringLiteral("mission.start"),
    };
    return commands.contains(command);
}

bool AiCommandPolicy::_isCriticalDeniedCommand(const QString& command)
{
    static const QSet<QString> commands = {
        QStringLiteral("param.write"),
        QStringLiteral("mavlink.send_raw"),
    };
    return commands.contains(command);
}

bool AiCommandPolicy::_validateArgumentsForCommand(const QString& command, const QVariantMap& arguments, QString* error)
{
    if (_isReadOnlyCommand(command)) {
        return _validateOptionalVehicleId(arguments, error);
    }
    if (command == QStringLiteral("ui.select_vehicle")) {
        return _validateRequiredVehicleId(arguments, error);
    }
    if (command == QStringLiteral("ui.open_page")) {
        const QString page = arguments.value(QStringLiteral("page")).toString().trimmed();
        if (page.isEmpty() || page.length() > 80) {
            *error = QStringLiteral("ui.open_page requires a short page argument.");
            return false;
        }
        return true;
    }
    if (command == QStringLiteral("map.focus_coordinate") || command == QStringLiteral("vehicle.goto")) {
        return _validateCoordinate(arguments, command == QStringLiteral("vehicle.goto"), error);
    }
    if (command == QStringLiteral("vehicle.takeoff")) {
        return _validateTakeoff(arguments, error);
    }
    if (command == QStringLiteral("vehicle.land") ||
        command == QStringLiteral("vehicle.rtl") ||
        command == QStringLiteral("vehicle.pause") ||
        command == QStringLiteral("vehicle.arm") ||
        command == QStringLiteral("vehicle.force_arm")) {
        return _validateOptionalVehicleId(arguments, error);
    }
    if (command == QStringLiteral("mission.analyze") ||
        command == QStringLiteral("mission.create_draft") ||
        command == QStringLiteral("mission.upload") ||
        command == QStringLiteral("mission.start")) {
        return true;
    }
    if (command == QStringLiteral("param.write")) {
        if (arguments.value(QStringLiteral("name")).toString().trimmed().isEmpty() ||
            !arguments.contains(QStringLiteral("value"))) {
            *error = QStringLiteral("param.write requires name and value for preview, but remains denied.");
            return false;
        }
        return true;
    }
    if (command == QStringLiteral("mavlink.send_raw")) {
        *error = QStringLiteral("raw MAVLink arguments are not accepted.");
        return false;
    }
    return true;
}

bool AiCommandPolicy::_validateOptionalVehicleId(const QVariantMap& arguments, QString* error)
{
    if (!arguments.contains(QStringLiteral("vehicle_id"))) {
        return true;
    }
    if (!_isPositiveInteger(arguments.value(QStringLiteral("vehicle_id")))) {
        *error = QStringLiteral("vehicle_id must be a positive integer.");
        return false;
    }
    return true;
}

bool AiCommandPolicy::_validateRequiredVehicleId(const QVariantMap& arguments, QString* error)
{
    if (!_isPositiveInteger(arguments.value(QStringLiteral("vehicle_id")))) {
        *error = QStringLiteral("vehicle_id is required and must be a positive integer.");
        return false;
    }
    return true;
}

bool AiCommandPolicy::_validateCoordinate(const QVariantMap& arguments, bool requireAltitude, QString* error)
{
    bool okLat = false;
    bool okLon = false;
    const double latitude = arguments.value(QStringLiteral("latitude")).toDouble(&okLat);
    const double longitude = arguments.value(QStringLiteral("longitude")).toDouble(&okLon);
    if (!okLat || !okLon || latitude < -90.0 || latitude > 90.0 || longitude < -180.0 || longitude > 180.0) {
        *error = QStringLiteral("latitude and longitude must be valid WGS84 coordinates.");
        return false;
    }
    if (arguments.contains(QStringLiteral("altitude_m")) || requireAltitude) {
        bool okAlt = false;
        const double altitude = arguments.value(QStringLiteral("altitude_m")).toDouble(&okAlt);
        if (!okAlt || altitude < 0.0 || altitude > 120.0) {
            *error = QStringLiteral("altitude_m must be between 0 and 120.");
            return false;
        }
    }
    return _validateOptionalVehicleId(arguments, error);
}

bool AiCommandPolicy::_validateTakeoff(const QVariantMap& arguments, QString* error)
{
    if (!_validateOptionalVehicleId(arguments, error)) {
        return false;
    }
    if (!arguments.contains(QStringLiteral("altitude_m"))) {
        return true;
    }
    bool ok = false;
    const double altitude = arguments.value(QStringLiteral("altitude_m")).toDouble(&ok);
    if (!ok || altitude <= 0.0 || altitude > 120.0) {
        *error = QStringLiteral("takeoff altitude_m must be > 0 and <= 120.");
        return false;
    }
    return true;
}

bool AiCommandPolicy::_isPositiveInteger(const QVariant& value)
{
    bool ok = false;
    const int id = value.toInt(&ok);
    return ok && id > 0;
}
