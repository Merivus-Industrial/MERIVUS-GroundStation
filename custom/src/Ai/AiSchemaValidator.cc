#include "AiSchemaValidator.h"

#include <QJsonArray>
#include <QJsonObject>

ActionProposal AiSchemaValidator::validate(const QString& requestId,
                                           const QJsonValue& proposalValue,
                                           const QString& provider,
                                           const QString& model)
{
    ActionProposal proposal;
    proposal.requestId = requestId;
    proposal.agentProvider = provider;
    proposal.agentModel = model;
    proposal.source = QStringLiteral("agent");
    proposal.validationStatus = ActionProposal::ValidationStatus::InvalidSchema;
    proposal.policyDecision = ActionProposal::PolicyDecision::Deny;
    proposal.localRisk = ActionProposal::RiskLevel::Critical;
    proposal.executable = false;

    if (proposalValue.isNull() || proposalValue.isUndefined()) {
        proposal.hasProposal = false;
        proposal.validationStatus = ActionProposal::ValidationStatus::Valid;
        proposal.policyDecision = ActionProposal::PolicyDecision::PreviewOnly;
        proposal.localRisk = ActionProposal::RiskLevel::Informational;
        proposal.reason = QStringLiteral("Agent returned no structured proposal.");
        return proposal;
    }

    proposal.hasProposal = true;
    if (!proposalValue.isObject()) {
        proposal.reason = QStringLiteral("proposal must be an object or null.");
        return proposal;
    }

    const QJsonObject object = proposalValue.toObject();
    QString error;
    if (!_validateObjectShape(object, &error)) {
        proposal.reason = error;
        return proposal;
    }

    const QJsonValue commandValue = object.value(QStringLiteral("command"));
    if (!commandValue.isString() || commandValue.toString().trimmed().isEmpty()) {
        proposal.validationStatus = ActionProposal::ValidationStatus::MissingCommand;
        proposal.reason = QStringLiteral("proposal.command is required.");
        return proposal;
    }

    proposal.command = commandValue.toString().trimmed();
    if (proposal.command.length() > kMaxCommandLength || _containsDangerousString(proposal.command)) {
        proposal.validationStatus = ActionProposal::ValidationStatus::InvalidArguments;
        proposal.reason = QStringLiteral("proposal.command is too long or unsafe.");
        return proposal;
    }

    const QJsonValue argumentsValue = object.value(QStringLiteral("arguments"));
    if (!argumentsValue.isObject()) {
        proposal.validationStatus = ActionProposal::ValidationStatus::InvalidArguments;
        proposal.reason = QStringLiteral("proposal.arguments must be an object.");
        return proposal;
    }

    const QJsonObject argumentsObject = argumentsValue.toObject();
    if (!_validateArguments(argumentsObject, &error)) {
        proposal.validationStatus = ActionProposal::ValidationStatus::InvalidArguments;
        proposal.reason = error;
        return proposal;
    }
    proposal.arguments = argumentsObject.toVariantMap();

    const QJsonValue summaryValue = object.value(QStringLiteral("summary"));
    if (!summaryValue.isString()) {
        proposal.reason = QStringLiteral("proposal.summary must be a string.");
        return proposal;
    }
    proposal.summary = summaryValue.toString().trimmed();
    if (proposal.summary.length() > kMaxSummaryLength || _containsDangerousString(proposal.summary)) {
        proposal.validationStatus = ActionProposal::ValidationStatus::InvalidArguments;
        proposal.reason = QStringLiteral("proposal.summary is too long or unsafe.");
        return proposal;
    }

    if (object.value(QStringLiteral("source")).isString()) {
        const QString source = object.value(QStringLiteral("source")).toString().trimmed();
        if (!source.isEmpty() && source.length() <= 64 && !_containsDangerousString(source)) {
            proposal.source = source;
        }
    }

    proposal.validationStatus = ActionProposal::ValidationStatus::Valid;
    proposal.policyDecision = ActionProposal::PolicyDecision::PreviewOnly;
    proposal.localRisk = ActionProposal::RiskLevel::Medium;
    proposal.reason = QStringLiteral("Schema accepted; local policy evaluation pending.");
    return proposal;
}

bool AiSchemaValidator::_validateObjectShape(const QJsonObject& object, QString* error)
{
    if (_jsonDepth(object) > kMaxJsonDepth) {
        *error = QStringLiteral("proposal JSON is too deeply nested.");
        return false;
    }

    for (auto it = object.constBegin(); it != object.constEnd(); ++it) {
        const QString key = it.key();
        if (key == QStringLiteral("risk") ||
            key == QStringLiteral("localRisk") ||
            key == QStringLiteral("local_risk") ||
            key == QStringLiteral("requiresConfirmation") ||
            key == QStringLiteral("requires_confirmation") ||
            key == QStringLiteral("policyDecision") ||
            key == QStringLiteral("policy_decision") ||
            key == QStringLiteral("executable") ||
            key == QStringLiteral("executed")) {
            continue;
        }

        if (_containsDangerousKey(key)) {
            *error = QStringLiteral("proposal contains unsafe field: %1").arg(key);
            return false;
        }

        if (key == QStringLiteral("command") ||
            key == QStringLiteral("arguments") ||
            key == QStringLiteral("summary") ||
            key == QStringLiteral("source")) {
            continue;
        }

        QString structureError;
        if (_hasDangerousStructure(it.value(), 0, &structureError)) {
            *error = structureError;
            return false;
        }
    }

    return true;
}

bool AiSchemaValidator::_validateArguments(const QJsonObject& arguments, QString* error)
{
    if (arguments.size() > kMaxArgumentKeys) {
        *error = QStringLiteral("proposal.arguments has too many keys.");
        return false;
    }

    for (auto it = arguments.constBegin(); it != arguments.constEnd(); ++it) {
        const QString key = it.key();
        if (key.length() > 64 || _containsDangerousKey(key)) {
            *error = QStringLiteral("proposal.arguments contains unsafe key: %1").arg(key);
            return false;
        }
        if (it.value().isArray()) {
            *error = QStringLiteral("proposal.arguments arrays are not accepted.");
            return false;
        }
        if (it.value().isString()) {
            const QString value = it.value().toString();
            if (value.length() > kMaxArgumentStringLength || _containsDangerousString(value)) {
                *error = QStringLiteral("proposal.arguments contains unsafe string value.");
                return false;
            }
        }
        QString structureError;
        if (_hasDangerousStructure(it.value(), 0, &structureError)) {
            *error = structureError;
            return false;
        }
    }

    return true;
}

bool AiSchemaValidator::_containsDangerousKey(const QString& key)
{
    const QString lower = key.toLower();
    return lower.contains(QStringLiteral("execute")) ||
           lower.contains(QStringLiteral("sendcommand")) ||
           lower == QStringLiteral("send") ||
           lower == QStringLiteral("run") ||
           lower.contains(QStringLiteral("shell")) ||
           lower.contains(QStringLiteral("script")) ||
           lower.contains(QStringLiteral("qprocess")) ||
           lower.contains(QStringLiteral("px4_batch")) ||
           lower.contains(QStringLiteral("mavlink_raw")) ||
           lower.contains(QStringLiteral("password")) ||
           lower.contains(QStringLiteral("api_key")) ||
           lower.contains(QStringLiteral("token"));
}

bool AiSchemaValidator::_containsDangerousString(const QString& value)
{
    const QString lower = value.toLower();
    return lower.contains(QStringLiteral("qprocess")) ||
           lower.contains(QStringLiteral("powershell")) ||
           lower.contains(QStringLiteral("cmd.exe")) ||
           lower.contains(QStringLiteral("/bin/sh")) ||
           lower.contains(QStringLiteral("bash -")) ||
           lower.contains(QStringLiteral("rm -rf")) ||
           lower.contains(QStringLiteral("format ")) ||
           lower.contains(QStringLiteral("sendcommand")) ||
           lower.contains(QStringLiteral("guidedmodetakeoff"));
}

bool AiSchemaValidator::_hasDangerousStructure(const QJsonValue& value, int depth, QString* error)
{
    if (depth > kMaxJsonDepth) {
        *error = QStringLiteral("proposal JSON is too deeply nested.");
        return true;
    }

    if (value.isObject()) {
        const QJsonObject object = value.toObject();
        for (auto it = object.constBegin(); it != object.constEnd(); ++it) {
            if (_containsDangerousKey(it.key())) {
                *error = QStringLiteral("proposal contains nested unsafe field: %1").arg(it.key());
                return true;
            }
            if (_hasDangerousStructure(it.value(), depth + 1, error)) {
                return true;
            }
        }
    } else if (value.isArray()) {
        const QJsonArray array = value.toArray();
        if (array.size() > 0) {
            *error = QStringLiteral("proposal nested arrays are not accepted.");
            return true;
        }
    } else if (value.isString() && _containsDangerousString(value.toString())) {
        *error = QStringLiteral("proposal contains unsafe string value.");
        return true;
    }

    return false;
}

int AiSchemaValidator::_jsonDepth(const QJsonValue& value, int depth)
{
    int maxDepth = depth;
    if (value.isObject()) {
        const QJsonObject object = value.toObject();
        for (auto it = object.constBegin(); it != object.constEnd(); ++it) {
            maxDepth = qMax(maxDepth, _jsonDepth(it.value(), depth + 1));
        }
    } else if (value.isArray()) {
        const QJsonArray array = value.toArray();
        for (const QJsonValue& child : array) {
            maxDepth = qMax(maxDepth, _jsonDepth(child, depth + 1));
        }
    }
    return maxDepth;
}
