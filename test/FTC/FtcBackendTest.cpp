#include "VehicleFtcStatusFactGroup.h"
#include <QtTest>
#include <QSignalSpy>

class FtcBackendTest : public QObject
{
    Q_OBJECT
    static mavlink_merivus_ftc_motor_status_t motor()
    {
        mavlink_merivus_ftc_motor_status_t m{};
        m.protocol_version = 2; m.motor_count = 4;
        m.system_state = MERIVUS_FTC_SYSTEM_STATE_NORMAL;
        m.flags = MERIVUS_FTC_MOTOR_FLAGS_MONITOR_ENABLED;
        m.estimator_state = 1;
        for (int i = 0; i < 12; ++i) {
            m.health_pct[i] = m.effectiveness_pct[i] = m.fault_probability_pct[i] = 255;
        }
        return m;
    }
    static void send(VehicleFtcStatusFactGroup& group, const mavlink_merivus_ftc_motor_status_t& data)
    {
        mavlink_message_t m{};
        mavlink_msg_merivus_ftc_motor_status_encode(1, 1, &m, &data);
        group.handleMessage(nullptr, m);
    }
    static QVariantMap row(VehicleFtcStatusFactGroup& group)
    { return static_cast<FtcMotorStatusModel*>(group.motors())->get(0); }
private slots:
    void noDataAndInvalidAreDistinctFromStale()
    {
        VehicleFtcStatusFactGroup g;
        QVERIFY(!g.available()); QVERIFY(!g.stale()); QCOMPARE(g.motorCount(), 0);
        QCOMPARE(g.systemStateText(), QString::fromUtf8("消息未收到"));
        auto m = motor(); send(g, m);
        QVERIFY(g.motorAvailable()); QVERIFY(!g.motorStale()); QVERIFY(!g.modelValid());
        QVERIFY(g.systemSeverity() != "normal");
        QCOMPARE(g.modelStateText(), QString::fromUtf8("学习中"));
        QCOMPARE(row(g)["effectiveness"].toDouble(), -1.0);
        m.baseline_learned = 1; m.estimator_state = 4; send(g, m);
        QCOMPARE(g.modelStateText(), QString::fromUtf8("当前不可观测"));
        m.estimator_state = 7; send(g, m);
        QCOMPARE(g.modelStateText(), QString::fromUtf8("模型不可用"));
    }
    void validDegradedFaultAndHistory()
    {
        VehicleFtcStatusFactGroup g; auto m = motor();
        m.flags |= MERIVUS_FTC_MOTOR_FLAGS_MODEL_VALID;
        m.estimator_state = 5; m.baseline_learned = m.current_observable = 1;
        m.health_pct[0] = m.effectiveness_pct[0] = 180; m.estimate_uncertainty[0] = .04f;
        send(g, m); QVERIFY(g.modelValid()); QCOMPARE(row(g)["effectiveness"].toDouble(), 90.0);
        QCOMPARE(g.systemSeverity(), QString("normal"));
        m.degraded_mask = 1; m.diagnosis_state[0] = 4;
        m.system_state = MERIVUS_FTC_SYSTEM_STATE_DEGRADED; send(g, m);
        QCOMPARE(row(g)["dataStateText"].toString(), QString::fromUtf8("退化"));
        m.failed_mask = 1; m.diagnosis_state[0] = 5;
        m.system_state = MERIVUS_FTC_SYSTEM_STATE_FAULT_CONFIRMED; send(g, m);
        QCOMPARE(row(g)["severity"].toString(), QString("critical"));
        m.flags &= ~MERIVUS_FTC_MOTOR_FLAGS_MODEL_VALID;
        m.current_observable = 0; m.estimator_state = 6; m.diagnosis_state[0] = 2; send(g, m);
        QCOMPARE(row(g)["effectiveness"].toDouble(), -1.0);
        QCOMPARE(row(g)["lastEffectiveness"].toDouble(), 90.0);
    }
    void staleAndStreamResumption()
    {
        VehicleFtcStatusFactGroup g; auto m = motor(); send(g, m);
        QSignalSpy changes(g.motors(), &QAbstractItemModel::dataChanged);
        QTest::qWait(3600);
        QVERIFY(g.stale()); QVERIFY(g.motorStale()); QVERIFY(!row(g)["available"].toBool());
        QVERIFY(!changes.isEmpty());
        const auto roles = qvariant_cast<QVector<int>>(changes.last().at(2));
        QVERIFY(roles.isEmpty() || roles.contains(FtcMotorStatusModel::EffectivenessRole));
        QVERIFY(roles.isEmpty() || roles.contains(FtcMotorStatusModel::DataStateTextRole));
        send(g, m); QVERIFY(!g.stale()); QVERIFY(!g.motorStale());
        QVERIFY(row(g)["available"].toBool());
    }
    void incompatibleStreamCanRecoverWithoutVehicleRecreation()
    {
        VehicleFtcStatusFactGroup g; auto m = motor();
        m.protocol_version = 1; send(g, m); QVERIFY(!g.protocolCompatible());
        m.protocol_version = 2; send(g, m);
        QVERIFY(g.protocolCompatible()); QVERIFY(g.motorAvailable());
    }
    void anotherStreamCannotHideVersionMismatch()
    {
        VehicleFtcStatusFactGroup g; auto m = motor();
        m.protocol_version = 1; send(g, m);
        mavlink_merivus_ftc_control_status_t c{}; c.protocol_version = 2;
        mavlink_message_t wire{}; mavlink_msg_merivus_ftc_control_status_encode(1, 1, &wire, &c);
        g.handleMessage(nullptr, wire); QVERIFY(!g.protocolCompatible());
        m.protocol_version = 2; send(g, m); QVERIFY(g.protocolCompatible());
    }
    void controlModes()
    {
        VehicleFtcStatusFactGroup g;
        mavlink_merivus_ftc_control_status_t c{}; c.protocol_version = 2;
        for (int mode = 0; mode <= 4; ++mode) {
            c.control_mode = mode; mavlink_message_t wire{};
            mavlink_msg_merivus_ftc_control_status_encode(1, 1, &wire, &c);
            g.handleMessage(nullptr, wire); QCOMPARE(g.controlMode(), mode);
            QVERIFY(g.controlModeText() != QString::fromUtf8("未知"));
        }
    }
};
int runFtcLiveProbe(int argc, char** argv);
int main(int argc, char** argv)
{
    for (int i = 1; i < argc; ++i) {
        if (QByteArray(argv[i]) == "--live") return runFtcLiveProbe(argc, argv);
    }
    QCoreApplication app(argc, argv);
    FtcBackendTest test;
    return QTest::qExec(&test, argc, argv);
}
#include "FtcBackendTest.moc"
