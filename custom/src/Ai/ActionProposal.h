#pragma once

#include <QJsonObject>
#include <QString>
#include <QVariantMap>

class ActionProposal
{
public:
    enum class ValidationStatus {
        Valid,
        InvalidSchema,
        MissingCommand,
        InvalidArguments,
        UnknownCommand,
    };

    enum class PolicyDecision {
        AllowReadOnly,
        AllowUiOnly,
        PreviewOnly,
        RequiresConfirmation,
        Deny,
    };

    enum class RiskLevel {
        Informational,
        Low,
        Medium,
        High,
        Critical,
    };

    QString requestId;
    QString command;
    QVariantMap arguments;
    QString summary;
    QString source;
    QString agentProvider;
    QString agentModel;

    ValidationStatus validationStatus = ValidationStatus::InvalidSchema;
    PolicyDecision policyDecision = PolicyDecision::Deny;
    RiskLevel localRisk = RiskLevel::Critical;
    bool requiresConfirmation = false;
    QString reason;
    bool executable = false;

    bool hasProposal = false;

    QVariantMap toVariantMap() const;
    QString argumentsSummary() const;

    static QString validationStatusToString(ValidationStatus status);
    static QString policyDecisionToString(PolicyDecision decision);
    static QString riskLevelToString(RiskLevel risk);
};
