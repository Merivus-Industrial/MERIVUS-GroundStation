#pragma once

#include "ActionProposal.h"

class AiCommandPolicy
{
public:
    static ActionProposal evaluate(ActionProposal proposal);

private:
    static bool _isReadOnlyCommand(const QString& command);
    static bool _isUiOnlyCommand(const QString& command);
    static bool _isMissionPreviewCommand(const QString& command);
    static bool _isHighRiskPreviewCommand(const QString& command);
    static bool _isCriticalDeniedCommand(const QString& command);

    static bool _validateArgumentsForCommand(const QString& command, const QVariantMap& arguments, QString* error);
    static bool _validateOptionalVehicleId(const QVariantMap& arguments, QString* error);
    static bool _validateRequiredVehicleId(const QVariantMap& arguments, QString* error);
    static bool _validateCoordinate(const QVariantMap& arguments, bool requireAltitude, QString* error);
    static bool _validateTakeoff(const QVariantMap& arguments, QString* error);
    static bool _isPositiveInteger(const QVariant& value);
};
