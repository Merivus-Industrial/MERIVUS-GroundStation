#include "AiAuditEvent.h"

#include <QDateTime>

Q_LOGGING_CATEGORY(MerivusAiPolicyLog, "merivus.ai.policy")

void AiAuditEvent::recordProposal(const ActionProposal& proposal, const QString& sessionId)
{
    qCInfo(MerivusAiPolicyLog).noquote()
        << "proposal_policy"
        << "timestamp=" + QDateTime::currentDateTimeUtc().toString(Qt::ISODate)
        << "request_id=" + proposal.requestId
        << "session_id=" + sessionId
        << "command=" + proposal.command
        << "validation=" + ActionProposal::validationStatusToString(proposal.validationStatus)
        << "decision=" + ActionProposal::policyDecisionToString(proposal.policyDecision)
        << "risk=" + ActionProposal::riskLevelToString(proposal.localRisk)
        << "requires_confirmation=" + QString(proposal.requiresConfirmation ? QStringLiteral("true") : QStringLiteral("false"))
        << "executable=" + QString(proposal.executable ? QStringLiteral("true") : QStringLiteral("false"))
        << "provider=" + proposal.agentProvider
        << "model=" + proposal.agentModel
        << "reason=" + proposal.reason.left(160);
}
