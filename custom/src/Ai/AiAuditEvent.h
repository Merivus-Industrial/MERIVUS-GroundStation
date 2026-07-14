#pragma once

#include "ActionProposal.h"

#include <QLoggingCategory>
#include <QString>

Q_DECLARE_LOGGING_CATEGORY(MerivusAiPolicyLog)

class AiAuditEvent
{
public:
    static void recordProposal(const ActionProposal& proposal, const QString& sessionId);
};
