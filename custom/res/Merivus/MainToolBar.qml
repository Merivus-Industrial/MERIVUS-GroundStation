import QtQuick          2.12
import QtQuick.Controls 2.4
import QtQuick.Layouts  1.11
import QGroundControl             1.0
import QGroundControl.Controls    1.0
import QGroundControl.FlightDisplay 1.0
import QGroundControl.Palette     1.0
import Merivus                    1.0

Rectangle {
    id: root

    GpsStatus { id: gpsStatus }
    color: qgcPal.toolbarBackground

    property int currentToolbar: flyViewToolbar
    readonly property int flyViewToolbar: 0
    readonly property int planViewToolbar: 1
    readonly property int simpleToolbar: 2
    readonly property var activeVehicle: QGroundControl.multiVehicleManager.activeVehicle
    readonly property bool communicationLost: activeVehicle ? activeVehicle.vehicleLinkManager.communicationLost : true
    readonly property bool vehicleConnected: activeVehicle && !communicationLost
    readonly property int messageCount: activeVehicle ? activeVehicle.messageCount : 0
    readonly property color nominalColor: qgcPal.colorGreen
    readonly property color warningColor: qgcPal.colorOrange
    readonly property color criticalColor: qgcPal.colorRed
    readonly property color offlineColor: qgcPal.colorGrey
    readonly property bool compact: width < 1500

    MerivusLinkDiagnostics {
        id: linkDiagnostics
    }

    function dropMessageIndicatorTool() {
        if (messageIndicatorLoader.item) messageIndicatorLoader.item.dropMessageIndicator()
    }

    function openApplicationLog() {
        mainWindow.showTool(qsTr("应用日志"),
                            "qrc:/qml/QGroundControl/Controls/AppMessages.qml",
                            "/qmlimages/MavlinkConsoleIcon")
    }

    function gpsStatusText() {
        return gpsStatus.summary(activeVehicle)
    }

    function gpsStatusColor() {
        if (!vehicleConnected || !activeVehicle || !activeVehicle.gps) return offlineColor
        var fixType = Number(activeVehicle.gps.lock.rawValue)
        if (isNaN(fixType) || fixType < 2) return criticalColor
        return fixType >= 3 ? nominalColor : warningColor
    }

    function gpsStatusDetails() {
        return gpsStatus.details(activeVehicle, QGroundControl.gpsRtk)
    }

    function triggerStatusAction(action) {
        if (action === "gps") {
            mainWindow.showMessageDialog(qsTr("GPS / RTK 状态"),
                                         gpsStatusDetails())
        } else if (action === "alerts") {
            dropMessageIndicatorTool()
        } else if (action === "logs") {
            openApplicationLog()
        } else if (action === "network") {
            QGroundControl.saveGlobalSetting("Merivus/AppSettingsPage", "qrc:/qml/LinkSettings.qml")
            mainWindow.showSettingsTool()
        }
    }

    function tcpStatusColor() {
        if (linkDiagnostics.summaryState === "connected") return root.nominalColor
        if (linkDiagnostics.summaryState === "connecting" || linkDiagnostics.summaryState === "configured_disconnected") return root.warningColor
        if (linkDiagnostics.summaryState === "error") return root.criticalColor
        return root.offlineColor
    }

    QGCPalette { id: qgcPal; colorGroupEnabled: true }

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 1
        color: qgcPal.windowShade
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 14
        anchors.rightMargin: 14
        spacing: 10

        QGCToolBarButton {
            Layout.preferredWidth: root.compact ? 62 : 78
            Layout.fillHeight: true
            icon.source: "/qmlimages/mrws2.png"
            icon.width: root.compact ? 46 : 56
            icon.height: root.compact ? 46 : 56
            logo: true
            onClicked: mainWindow.showToolSelectDialog()
            MerivusToolTip {
                visible: parent.hovered
                text: qsTr("工具与设置")
                anchors.left: parent.left
                anchors.top: parent.bottom
                anchors.topMargin: 6
            }
        }

        RowLayout {
            visible: root.currentToolbar === root.flyViewToolbar
            spacing: root.compact ? 3 : 7
            Repeater {
                model: [
                    { icon: "/qmlimages/Link.svg", title: qsTr("飞控"), value: root.vehicleConnected ? qsTr("已连接") : (root.activeVehicle ? qsTr("异常") : qsTr("未连接")), color: root.vehicleConnected ? root.nominalColor : (root.activeVehicle ? root.criticalColor : root.offlineColor), action: "", tip: qsTr("飞控链路状态") },
                    { icon: "/qmlimages/wifi.svg", title: "TCP", value: linkDiagnostics.summaryText, color: root.tcpStatusColor(), action: "network", tip: linkDiagnostics.detailText },
                    { icon: "/qmlimages/Signal100.svg", title: qsTr("心跳"), value: root.vehicleConnected ? qsTr("正常") : qsTr("未连接"), color: root.vehicleConnected ? root.nominalColor : root.offlineColor, action: "", tip: qsTr("MAVLink 心跳状态") },
                    { icon: "/qmlimages/Gps.svg", title: "GPS/RTK", value: root.gpsStatusText(), color: root.gpsStatusColor(), action: "gps", tip: qsTr("查看定位摘要") },
                    { icon: root.messageCount > 0 ? "/qmlimages/Yield.svg" : "/qmlimages/Megaphone.svg", title: qsTr("消息"), value: root.messageCount > 0 ? qsTr("%1 条").arg(root.messageCount) : qsTr("正常"), color: root.messageCount > 0 ? root.warningColor : root.nominalColor, action: "alerts", tip: qsTr("打开飞行器消息") },
                    { icon: "/qmlimages/MavlinkConsoleIcon", title: qsTr("诊断"), value: qsTr("日志"), color: root.offlineColor, action: "logs", tip: qsTr("打开应用日志") }
                ]

                Rectangle {
                    Layout.preferredWidth: root.compact ? 34 : Math.max(66, statusRow.implicitWidth + 10)
                    Layout.preferredHeight: root.compact ? 38 : 42
                    radius: 6
                    color: statusMouse.containsMouse ? qgcPal.windowShade : "transparent"

                    RowLayout {
                        id: statusRow
                        anchors.fill: parent
                        anchors.leftMargin: 6
                        anchors.rightMargin: 6
                        spacing: 6
                        Item {
                            Layout.preferredWidth: 20
                            Layout.preferredHeight: 24
                            QGCColoredImage {
                                anchors.centerIn: parent
                                width: 18
                                height: 18
                                source: modelData.icon
                                color: modelData.color
                            }
                            Rectangle {
                                anchors.right: parent.right
                                anchors.bottom: parent.bottom
                                width: 6
                                height: 6
                                radius: 3
                                color: modelData.color
                                border.color: qgcPal.toolbarBackground
                            }
                        }
                        ColumnLayout {
                            visible: !root.compact
                            spacing: -1
                            QGCLabel { text: modelData.title; color: qgcPal.colorGrey; font.pixelSize: 9 }
                            QGCLabel { text: modelData.value; color: modelData.color; font.pixelSize: 11; font.bold: true }
                        }
                    }
                    MouseArea { id: statusMouse; anchors.fill: parent; hoverEnabled: true; onClicked: root.triggerStatusAction(modelData.action) }
                    MerivusToolTip {
                        visible: statusMouse.containsMouse
                        text: modelData.tip
                        anchors.left: parent.left
                        anchors.top: parent.bottom
                        anchors.topMargin: 6
                    }
                }
            }
        }

        Loader {
            visible: root.currentToolbar === root.planViewToolbar
            source: visible ? "qrc:/qml/PlanToolBarIndicators.qml" : ""
        }

        Item { Layout.fillWidth: true }
        Item { Layout.preferredWidth: root.compact ? 136 : 168 }
    }

    QGCLabel {
        anchors.centerIn: parent
        text: qsTr("无人机多机调度系统指挥中心")
        color: qgcPal.buttonHighlight
        font.bold: true
        font.pixelSize: root.compact ? 18 : 24
        font.letterSpacing: root.compact ? 0.8 : 1.2
    }

    Image {
        anchors.right: parent.right
        anchors.rightMargin: 18
        anchors.verticalCenter: parent.verticalCenter
        visible: true
        source: "/qmlimages/mrws4.png"
        width: root.compact ? 132 : 164
        height: parent.height - 6
        fillMode: Image.PreserveAspectFit
        mipmap: true
    }

    Loader { id: messageIndicatorLoader; visible: false; source: "qrc:/toolbar/MessageIndicator.qml" }
    Rectangle {
        anchors.bottom: parent.bottom
        height: Math.max(2, root.height * 0.05)
        width: root.activeVehicle ? root.activeVehicle.loadProgress * parent.width : 0
        color: qgcPal.colorGreen
        visible: width > 0
    }
}
