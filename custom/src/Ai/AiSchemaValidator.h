#pragma once

#include "ActionProposal.h"

#include <QJsonValue>
#include <QString>

class AiSchemaValidator
{
public:
    static ActionProposal validate(const QString& requestId,
                                   const QJsonValue& proposalValue,
                                   const QString& provider,
                                   const QString& model);

private:
    static bool _validateObjectShape(const QJsonObject& object, QString* error);
    static bool _validateArguments(const QJsonObject& arguments, QString* error);
    static bool _containsDangerousKey(const QString& key);
    static bool _containsDangerousString(const QString& value);
    static bool _hasDangerousStructure(const QJsonValue& value, int depth, QString* error);
    static int _jsonDepth(const QJsonValue& value, int depth = 0);

    static constexpr int kMaxCommandLength = 128;
    static constexpr int kMaxSummaryLength = 1000;
    static constexpr int kMaxArgumentKeys = 24;
    static constexpr int kMaxArgumentStringLength = 512;
    static constexpr int kMaxJsonDepth = 5;
};
