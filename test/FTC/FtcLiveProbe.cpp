#include "VehicleFtcStatusFactGroup.h"
#include <QCoreApplication>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUdpSocket>
#include <QElapsedTimer>

int runFtcLiveProbe(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    const auto arguments = app.arguments();
    const int outputIndex = arguments.indexOf("--output");
    if (outputIndex < 0 || outputIndex+1 >= arguments.size()) return 2;
    QFile output(arguments[outputIndex+1]);
    if (output.exists() || !output.open(QIODevice::WriteOnly)) return 3;
    VehicleFtcStatusFactGroup group;
    QUdpSocket udp;
    if (!udp.bind(QHostAddress::LocalHost, 14655)) return 4;
    QElapsedTimer elapsed; elapsed.start();
    quint64 heartbeat = 0, packets = 0, crcErrors = 0;
    mavlink_status_t parser{};
    mavlink_message_t incoming{};
    QObject::connect(&udp, &QUdpSocket::readyRead, [&] {
        while (udp.hasPendingDatagrams()) {
            QByteArray data; data.resize(int(udp.pendingDatagramSize())); udp.readDatagram(data.data(), data.size());
            for (char byte : data) {
                mavlink_message_t decoded{}; mavlink_status_t status{};
                const auto framing = mavlink_frame_char_buffer(&incoming, &parser, uint8_t(byte), &decoded, &status);
                if (framing == MAVLINK_FRAMING_BAD_CRC) ++crcErrors;
                if (framing == MAVLINK_FRAMING_OK) {
                    ++packets;
                    if (decoded.msgid == MAVLINK_MSG_ID_HEARTBEAT) ++heartbeat;
                    group.handleMessage(nullptr, decoded);
                }
            }
        }
    });
    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&] {
        QJsonObject state{{"elapsed_ms", elapsed.elapsed()}, {"packets", qint64(packets)},
            {"heartbeat", qint64(heartbeat)}, {"crc_errors", qint64(crcErrors)},
            {"available", group.available()}, {"stale", group.stale()}, {"motor_stale", group.motorStale()},
            {"compatible", group.protocolCompatible()}, {"model_valid", group.modelValid()},
            {"system_state", group.systemStateText()}, {"model_state", group.modelStateText()},
            {"control_mode", group.controlModeText()}};
        QJsonArray motors;
        for (int i = 0; i < group.motorCount(); ++i)
            motors.append(QJsonObject::fromVariantMap(static_cast<FtcMotorStatusModel*>(group.motors())->get(i)));
        state["motors"] = motors;
        output.write(QJsonDocument(state).toJson(QJsonDocument::Compact)+"\n"); output.flush();
    });
    timer.start(500);
    QTimer::singleShot(300000, &app, &QCoreApplication::quit);
    return app.exec();
}
