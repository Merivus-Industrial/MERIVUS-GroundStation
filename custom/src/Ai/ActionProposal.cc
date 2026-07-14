#include "ActionProposal.h"

#include <QJsonDocument>

QVariantMap ActionProposal::toVariantMap() const
{
    QVariantMap object;
    object.insert(QStringLiteral("requestId"), requestId);
    object.insert(QStringLiteral("command"), command);
    object.insert(QStringLiteral("arguments"), arguments);
    object.insert(QStringLiteral("summary"), summary);
    object.insert(QStringLiteral("source"), source);
    object.insert(QStringLiteral("agentProvider"), agentProvider);
    object.insert(QStringLiteral("agentModel"), agentModel);
    object.insert(QStringLiteral("validationStatus"), validationStatusToString(validationStatus));
    object.insert(QStringLiteral("policyDecision"), policyDecisionToString(policyDecision));
    object.insert(QStringLiteral("localRisk"), riskLevelToString(localRisk));
    object.insert(QStringLiteral("requiresConfirmation"), requiresConfirmation);
    object.insert(QStringLiteral("reason"), reason);
    object.insert(QStringLiteral("executable"), executable);
    object.insert(QStringLiteral("hasProposal"), hasProposal);
    object.insert(QStringLiteral("argumentsSummary"), argumentsSummary());
    return object;
}

QString ActionProposal::argumentsSummary() const
{
    if (arguments.isEmpty()) {
        return QStringLiteral("{}");
    }
    return QString::fromUtf8(QJsonDocument(QJsonObject::fromVariantMap(arguments)).toJson(QJsonDocument::Compact));
}

QString ActionProposal::validationStatusToString(ValidationStatus status)
{
    switch (status) {
    case ValidationStatus::Valid:
        return QStringLiteral("Valid");
    case ValidationStatus::InvalidSchema:
        return QStringLiteral("InvalidSchema");
    case ValidationStatus::MissingCommand:
        return QStringLiteral("MissingCommand");
    case ValidationStatus::InvalidArguments:
        return QStringLiteral("InvalidArguments");
    case ValidationStatus::UnknownCommand:
        return QStringLiteral("UnknownCommand");
    }
    return QStringLiteral("InvalidSchema");
}

QString ActionProposal::policyDecisionToString(PolicyDecision decision)
{
    switch (decision) {
    case PolicyDecision::AllowReadOnly:
        return QStringLiteral("AllowReadOnly");
    case PolicyDecision::AllowUiOnly:
        return QStringLiteral("AllowUiOnly");
    case PolicyDecision::PreviewOnly:
        return QStringLiteral("PreviewOnly");
    case PolicyDecision::RequiresConfirmation:
        return QStringLiteral("RequiresConfirmation");
    case PolicyDecision::Deny:
        return QStringLiteral("Deny");
    }
    return QStringLiteral("Deny");
}

QString ActionProposal::riskLevelToString(RiskLevel risk)
{
    switch (risk) {
    case RiskLevel::Informational:
        return QStringLiteral("Informational");
    case RiskLevel::Low:
        return QStringLiteral("Low");
    case RiskLevel::Medium:
        return QStringLiteral("Medium");
    case RiskLevel::High:
        return QStringLiteral("High");
    case RiskLevel::Critical:
        return QStringLiteral("Critical");
    }
    return QStringLiteral("Critical");
}
