#include "AiCommandPolicy.h"
#include "AiSchemaValidator.h"

#include <QCoreApplication>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QStringList>

namespace {
int gPassed = 0;
int gFailed = 0;

QJsonObject makeProposal(const QString& command,
                         const QVariantMap& arguments = QVariantMap(),
                         const QString& summary = QStringLiteral("summary"))
{
    QJsonObject object;
    object.insert(QStringLiteral("command"), command);
    object.insert(QStringLiteral("arguments"), QJsonObject::fromVariantMap(arguments));
    object.insert(QStringLiteral("summary"), summary);
    object.insert(QStringLiteral("source"), QStringLiteral("mock-agent"));
    return object;
}

ActionProposal evaluate(const QJsonValue& value)
{
    return AiCommandPolicy::evaluate(AiSchemaValidator::validate(
        QStringLiteral("test-request"),
        value,
        QStringLiteral("mock"),
        QStringLiteral("mock-v1")));
}

void expect(bool condition, const QString& name, const QString& detail = QString())
{
    if (condition) {
        ++gPassed;
        QTextStream(stdout) << "PASS " << name << Qt::endl;
        return;
    }
    ++gFailed;
    QTextStream(stderr) << "FAIL " << name << " " << detail << Qt::endl;
}

void expectStatus(const QString& name,
                  const ActionProposal& proposal,
                  ActionProposal::ValidationStatus validation,
                  ActionProposal::PolicyDecision decision,
                  ActionProposal::RiskLevel risk)
{
    expect(proposal.validationStatus == validation &&
               proposal.policyDecision == decision &&
               proposal.localRisk == risk,
           name,
           QStringLiteral("got validation=%1 decision=%2 risk=%3 reason=%4")
               .arg(ActionProposal::validationStatusToString(proposal.validationStatus))
               .arg(ActionProposal::policyDecisionToString(proposal.policyDecision))
               .arg(ActionProposal::riskLevelToString(proposal.localRisk))
               .arg(proposal.reason));
}
}

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    expectStatus(QStringLiteral("null proposal is accepted as no-op"),
                 evaluate(QJsonValue(QJsonValue::Null)),
                 ActionProposal::ValidationStatus::Valid,
                 ActionProposal::PolicyDecision::PreviewOnly,
                 ActionProposal::RiskLevel::Informational);

    QJsonObject missingCommand;
    missingCommand.insert(QStringLiteral("arguments"), QJsonObject());
    missingCommand.insert(QStringLiteral("summary"), QStringLiteral("summary"));
    expectStatus(QStringLiteral("missing command"), evaluate(missingCommand),
                 ActionProposal::ValidationStatus::MissingCommand,
                 ActionProposal::PolicyDecision::Deny,
                 ActionProposal::RiskLevel::Critical);

    expectStatus(QStringLiteral("empty command"), evaluate(makeProposal(QString())),
                 ActionProposal::ValidationStatus::MissingCommand,
                 ActionProposal::PolicyDecision::Deny,
                 ActionProposal::RiskLevel::Critical);

    expectStatus(QStringLiteral("overlong command"), evaluate(makeProposal(QString(129, QLatin1Char('a')))),
                 ActionProposal::ValidationStatus::InvalidArguments,
                 ActionProposal::PolicyDecision::Deny,
                 ActionProposal::RiskLevel::Critical);

    QJsonObject missingArguments;
    missingArguments.insert(QStringLiteral("command"), QStringLiteral("vehicle.query_status"));
    missingArguments.insert(QStringLiteral("summary"), QStringLiteral("summary"));
    expectStatus(QStringLiteral("missing arguments"), evaluate(missingArguments),
                 ActionProposal::ValidationStatus::InvalidArguments,
                 ActionProposal::PolicyDecision::Deny,
                 ActionProposal::RiskLevel::Critical);

    QJsonObject arrayArguments = makeProposal(QStringLiteral("mission.analyze"));
    QJsonObject argsWithArray;
    QJsonArray array;
    array.append(1);
    argsWithArray.insert(QStringLiteral("items"), array);
    arrayArguments.insert(QStringLiteral("arguments"), argsWithArray);
    expectStatus(QStringLiteral("arguments array rejected"), evaluate(arrayArguments),
                 ActionProposal::ValidationStatus::InvalidArguments,
                 ActionProposal::PolicyDecision::Deny,
                 ActionProposal::RiskLevel::Critical);

    QJsonObject missingSummary;
    missingSummary.insert(QStringLiteral("command"), QStringLiteral("vehicle.query_status"));
    missingSummary.insert(QStringLiteral("arguments"), QJsonObject());
    expectStatus(QStringLiteral("missing summary"), evaluate(missingSummary),
                 ActionProposal::ValidationStatus::InvalidSchema,
                 ActionProposal::PolicyDecision::Deny,
                 ActionProposal::RiskLevel::Critical);

    expectStatus(QStringLiteral("overlong summary"), evaluate(makeProposal(QStringLiteral("vehicle.query_status"), QVariantMap(), QString(1001, QLatin1Char('s')))),
                 ActionProposal::ValidationStatus::InvalidArguments,
                 ActionProposal::PolicyDecision::Deny,
                 ActionProposal::RiskLevel::Critical);

    QJsonObject topExecute = makeProposal(QStringLiteral("vehicle.query_status"));
    topExecute.insert(QStringLiteral("execute_now"), true);
    expectStatus(QStringLiteral("dangerous top-level execute field"), evaluate(topExecute),
                 ActionProposal::ValidationStatus::InvalidSchema,
                 ActionProposal::PolicyDecision::Deny,
                 ActionProposal::RiskLevel::Critical);

    QVariantMap nestedRun;
    nestedRun.insert(QStringLiteral("safe"), QVariantMap{{QStringLiteral("run"), QStringLiteral("now")}});
    expectStatus(QStringLiteral("nested run field rejected"), evaluate(makeProposal(QStringLiteral("mission.analyze"), nestedRun)),
                 ActionProposal::ValidationStatus::InvalidArguments,
                 ActionProposal::PolicyDecision::Deny,
                 ActionProposal::RiskLevel::Critical);

    QJsonObject rawMavlink = makeProposal(QStringLiteral("mavlink.send_raw"));
    QJsonObject rawArgs;
    QJsonArray params;
    params.append(76);
    params.append(1);
    rawArgs.insert(QStringLiteral("params"), params);
    rawMavlink.insert(QStringLiteral("arguments"), rawArgs);
    expectStatus(QStringLiteral("raw MAVLink param array rejected"), evaluate(rawMavlink),
                 ActionProposal::ValidationStatus::InvalidArguments,
                 ActionProposal::PolicyDecision::Deny,
                 ActionProposal::RiskLevel::Critical);

    expectStatus(QStringLiteral("unknown command denied"), evaluate(makeProposal(QStringLiteral("vehicle.magic"))),
                 ActionProposal::ValidationStatus::UnknownCommand,
                 ActionProposal::PolicyDecision::Deny,
                 ActionProposal::RiskLevel::Critical);

    expectStatus(QStringLiteral("read-only status allowed"), evaluate(makeProposal(QStringLiteral("vehicle.query_status"))),
                 ActionProposal::ValidationStatus::Valid,
                 ActionProposal::PolicyDecision::AllowReadOnly,
                 ActionProposal::RiskLevel::Low);

    expectStatus(QStringLiteral("log explain informational"), evaluate(makeProposal(QStringLiteral("log.explain_error"))),
                 ActionProposal::ValidationStatus::Valid,
                 ActionProposal::PolicyDecision::AllowReadOnly,
                 ActionProposal::RiskLevel::Informational);

    expectStatus(QStringLiteral("ui select vehicle allowed"), evaluate(makeProposal(QStringLiteral("ui.select_vehicle"), QVariantMap{{QStringLiteral("vehicle_id"), 1}})),
                 ActionProposal::ValidationStatus::Valid,
                 ActionProposal::PolicyDecision::AllowUiOnly,
                 ActionProposal::RiskLevel::Low);

    expectStatus(QStringLiteral("ui select invalid vehicle id"), evaluate(makeProposal(QStringLiteral("ui.select_vehicle"), QVariantMap{{QStringLiteral("vehicle_id"), -1}})),
                 ActionProposal::ValidationStatus::InvalidArguments,
                 ActionProposal::PolicyDecision::Deny,
                 ActionProposal::RiskLevel::Critical);

    expectStatus(QStringLiteral("map focus coordinate allowed"), evaluate(makeProposal(QStringLiteral("map.focus_coordinate"), QVariantMap{{QStringLiteral("latitude"), 31.2}, {QStringLiteral("longitude"), 121.4}})),
                 ActionProposal::ValidationStatus::Valid,
                 ActionProposal::PolicyDecision::AllowUiOnly,
                 ActionProposal::RiskLevel::Low);

    expectStatus(QStringLiteral("map focus invalid latitude"), evaluate(makeProposal(QStringLiteral("map.focus_coordinate"), QVariantMap{{QStringLiteral("latitude"), 91.0}, {QStringLiteral("longitude"), 121.4}})),
                 ActionProposal::ValidationStatus::InvalidArguments,
                 ActionProposal::PolicyDecision::Deny,
                 ActionProposal::RiskLevel::Critical);

    expectStatus(QStringLiteral("mission draft preview"), evaluate(makeProposal(QStringLiteral("mission.create_draft"))),
                 ActionProposal::ValidationStatus::Valid,
                 ActionProposal::PolicyDecision::PreviewOnly,
                 ActionProposal::RiskLevel::Medium);

    ActionProposal takeoff = evaluate(makeProposal(QStringLiteral("vehicle.takeoff"), QVariantMap{{QStringLiteral("altitude_m"), 30.0}}));
    expectStatus(QStringLiteral("takeoff preview high risk"), takeoff,
                 ActionProposal::ValidationStatus::Valid,
                 ActionProposal::PolicyDecision::PreviewOnly,
                 ActionProposal::RiskLevel::High);
    expect(takeoff.requiresConfirmation && !takeoff.executable, QStringLiteral("takeoff requires confirmation but is not executable"));

    expectStatus(QStringLiteral("takeoff altitude too high"), evaluate(makeProposal(QStringLiteral("vehicle.takeoff"), QVariantMap{{QStringLiteral("altitude_m"), 130.0}})),
                 ActionProposal::ValidationStatus::InvalidArguments,
                 ActionProposal::PolicyDecision::Deny,
                 ActionProposal::RiskLevel::Critical);

    expectStatus(QStringLiteral("goto preview high risk"), evaluate(makeProposal(QStringLiteral("vehicle.goto"), QVariantMap{{QStringLiteral("latitude"), 31.2}, {QStringLiteral("longitude"), 121.4}, {QStringLiteral("altitude_m"), 50.0}})),
                 ActionProposal::ValidationStatus::Valid,
                 ActionProposal::PolicyDecision::PreviewOnly,
                 ActionProposal::RiskLevel::High);

    expectStatus(QStringLiteral("param write denied"), evaluate(makeProposal(QStringLiteral("param.write"), QVariantMap{{QStringLiteral("name"), QStringLiteral("COM_ARM_WO_GPS")}, {QStringLiteral("value"), 1}})),
                 ActionProposal::ValidationStatus::Valid,
                 ActionProposal::PolicyDecision::Deny,
                 ActionProposal::RiskLevel::Critical);

    expectStatus(QStringLiteral("mavlink raw denied without params"), evaluate(makeProposal(QStringLiteral("mavlink.send_raw"))),
                 ActionProposal::ValidationStatus::InvalidArguments,
                 ActionProposal::PolicyDecision::Deny,
                 ActionProposal::RiskLevel::Critical);

    QJsonObject forged = makeProposal(QStringLiteral("vehicle.takeoff"), QVariantMap{{QStringLiteral("altitude_m"), 20.0}});
    forged.insert(QStringLiteral("risk"), QStringLiteral("Low"));
    forged.insert(QStringLiteral("requires_confirmation"), false);
    forged.insert(QStringLiteral("executable"), true);
    forged.insert(QStringLiteral("executed"), true);
    ActionProposal forgedResult = evaluate(forged);
    expectStatus(QStringLiteral("agent-forged local risk fields ignored"), forgedResult,
                 ActionProposal::ValidationStatus::Valid,
                 ActionProposal::PolicyDecision::PreviewOnly,
                 ActionProposal::RiskLevel::High);
    expect(forgedResult.requiresConfirmation && !forgedResult.executable, QStringLiteral("forged executable cannot override local policy"));

    QTextStream(stdout) << "AiIntentPolicyTest passed=" << gPassed << " failed=" << gFailed << Qt::endl;
    return gFailed == 0 ? 0 : 1;
}