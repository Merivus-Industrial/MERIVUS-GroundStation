import QtQuick          2.12
import QtQuick.Controls 2.4
import QtQuick.Layouts  1.11

import QGroundControl               1.0
import QGroundControl.Controls      1.0
import QGroundControl.FlightDisplay 1.0
import QGroundControl.FlightMap     1.0
import QGroundControl.Palette       1.0
import QGroundControl.ScreenTools   1.0

Item {
    id: root

    property var selectedIds: []
    property var vehicles: QGroundControl.multiVehicleManager.vehicles
    property var activeVehicle: QGroundControl.multiVehicleManager.activeVehicle
    property var toolInsets

    signal vehicleSelectionRequested(int vehicleId, bool selected)
    signal vehicleFocusRequested(int vehicleId)
    signal clearSelectionRequested()

    readonly property real margin: Math.max(8, ScreenTools.defaultFontPixelWidth * 0.75)
    readonly property real topInset: toolInsets ? Math.max(margin, toolInsets.topEdgeLeftInset + margin) : margin
    readonly property real panelRadius: 8
    readonly property real collapsedPanelWidth: 52
    readonly property real resizeHandleThickness: 10
    readonly property real sidePanelMinHeight: 420
    readonly property real panelBottomLiftMin: 78
    readonly property real leftPanelMinWidth: 300
    readonly property real rightPanelMinWidth: 300
    readonly property real leftPanelMaxWidth: Math.max(leftPanelMinWidth, Math.min(480, width * 0.40))
    readonly property real rightPanelMaxWidth: Math.max(rightPanelMinWidth, Math.min(480, width * 0.40))
    readonly property color panelColor: Qt.rgba(qgcPal.window.r, qgcPal.window.g, qgcPal.window.b, 0.94)
    readonly property color raisedColor: Qt.rgba(qgcPal.windowShade.r, qgcPal.windowShade.g, qgcPal.windowShade.b, 0.96)
    readonly property color panelLine: Qt.rgba(qgcPal.text.r, qgcPal.text.g, qgcPal.text.b, 0.20)
    readonly property color mutedLine: Qt.rgba(qgcPal.text.r, qgcPal.text.g, qgcPal.text.b, 0.11)
    readonly property color accent: qgcPal.buttonHighlight
    readonly property color nominal: qgcPal.colorGreen
    readonly property color muted: qgcPal.colorGrey
    readonly property var focusVehicle: resolveFocusVehicle()
    readonly property var rightVehicle: resolveRightVehicle()
    readonly property var guidedController: globals.guidedControllerFlyView

    property real leftPanelTopExtra: 3
    property real rightPanelTopExtra: 3
    property real leftPanelBottomLift: 78
    property real rightPanelBottomLift: 78
    property real leftPanelWidth: Math.min(390, width * 0.27)
    property real rightPanelWidth: Math.min(390, width * 0.27)
    property bool leftExpanded: true
    property bool rightExpanded: width >= 1280
    property bool leftPanelResizing: false
    property bool rightPanelResizing: false
    property int rightPanelVehicleId: -1
    property real altitudeCommandMeters: 10
    property real speedCommandMetersSecond: 5
    property date now: new Date()

    visible: width > 900 && height > 560

    QGCPalette { id: qgcPal; colorGroupEnabled: true }
    Timer { interval: 1000; running: root.visible; repeat: true; onTriggered: root.now = new Date() }

    function tr(text) { return qsTr(text) }
    function clamp(value, minValue, maxValue) { return Math.max(minValue, Math.min(maxValue, value)) }

    function vehicleById(vehicleId) {
        if (!vehicles) return null
        for (var i = 0; i < vehicles.count; i++) {
            var vehicle = vehicles.get(i)
            if (vehicle && vehicle.id === vehicleId) return vehicle
        }
        return null
    }

    function resolveFocusVehicle() {
        if (selectedIds && selectedIds.length > 0) {
            var selectedVehicle = vehicleById(selectedIds[0])
            if (selectedVehicle) return selectedVehicle
        }
        return activeVehicle
    }

    function resolveRightVehicle() {
        if (selectedIds && selectedIds.length > 0) {
            if (rightPanelVehicleId > 0 && selectedIds.indexOf(rightPanelVehicleId) !== -1) {
                var requestedVehicle = vehicleById(rightPanelVehicleId)
                if (requestedVehicle) return requestedVehicle
            }
            var firstSelectedVehicle = vehicleById(selectedIds[0])
            if (firstSelectedVehicle) return firstSelectedVehicle
        }
        return focusVehicle
    }

    function rightVehicleSelected(vehicleId) {
        return rightVehicle && rightVehicle.id === vehicleId
    }

    function selectedCount() { return selectedIds ? selectedIds.length : 0 }

    function selectedSummary() {
        if (selectedCount() === 0) {
            return activeVehicle ? tr("\u5730\u56fe\u76ee\u6807\uff1a\u5f53\u524d\u7126\u70b9 UAV-%1").arg(activeVehicle.id) : tr("\u5730\u56fe\u76ee\u6807\uff1a\u672a\u9009\u62e9")
        }
        if (selectedCount() === 1) return tr("\u5730\u56fe\u76ee\u6807\uff1aUAV-%1").arg(selectedIds[0])
        return tr("\u5730\u56fe\u76ee\u6807\uff1a%1 \u67b6 - ID %2").arg(selectedCount()).arg(selectedIds.join(", "))
    }

    function isSelected(vehicleId) { return selectedIds && selectedIds.indexOf(vehicleId) !== -1 }
    function factHasValue(fact) { return fact && !isNaN(Number(fact.rawValue)) }
    function numberText(fact, decimals, suffix) { return factHasValue(fact) ? Number(fact.rawValue).toFixed(decimals) + (suffix ? " " + suffix : "") : "--" }

    function metricFactFor(vehicle, kind) {
        if (!vehicle) return null
        if (kind === "altitude") return vehicle.altitudeRelative
        if (kind === "speed") return (vehicle.fixedWing || vehicle.vtolInFwdFlight) ? vehicle.airSpeed : vehicle.groundSpeed
        if (kind === "climb") return vehicle.climbRate
        if (kind === "heading") return vehicle.heading
        if (kind === "distance") return vehicle.flightDistance
        if (kind === "battery") return vehicle.batteries && vehicle.batteries.count > 0 ? vehicle.batteries.get(0).percentRemaining : null
        if (kind === "gps") return vehicle.gps ? vehicle.gps.count : null
        return null
    }

    function metricFact(kind) { return metricFactFor(focusVehicle, kind) }

    function metricTextFor(vehicle, kind, decimals, suffix) {
        if (kind === "time") return vehicle ? elapsedText(vehicle.flightTime) : "--"
        return numberText(metricFactFor(vehicle, kind), decimals, suffix)
    }

    function metricText(kind, decimals, suffix) { return metricTextFor(focusVehicle, kind, decimals, suffix) }

function elapsedText(fact) {
        if (!factHasValue(fact)) return "--"
        var seconds = Number(fact.rawValue)
        if (seconds < 0) return "--"
        var hours = Math.floor(seconds / 3600)
        var minutes = Math.floor((seconds % 3600) / 60)
        var secs = Math.floor(seconds % 60)
        return (hours < 10 ? "0" : "") + hours + ":" + (minutes < 10 ? "0" : "") + minutes + ":" + (secs < 10 ? "0" : "") + secs
    }

    function batteryPercent(vehicle) {
        if (!vehicle || !vehicle.batteries || vehicle.batteries.count === 0) return "--"
        return numberText(vehicle.batteries.get(0).percentRemaining, 0, "%")
    }

    function temperatureText(vehicle) {
        if (!vehicle) return "--"
        if (vehicle.hygrometer && factHasValue(vehicle.hygrometer.hygroTemp)) {
            return numberText(vehicle.hygrometer.hygroTemp, 1, "°C")
        }
        return numberText(vehicle.temperature ? vehicle.temperature.temperature1 : null, 1, "°C")
    }

    function humidityText(vehicle) {
        return vehicle && vehicle.hygrometer ? numberText(vehicle.hygrometer.hygroHumi, 1, "%") : "--"
    }

    function windSpeedText(vehicle) {
        return vehicle && vehicle.wind ? numberText(vehicle.wind.speed, 1, "m/s") : "--"
    }

    function windDirectionText(vehicle) {
        if (!vehicle || !vehicle.wind || !factHasValue(vehicle.wind.direction)) return "--"
        var degrees = Number(vehicle.wind.direction.rawValue)
        var names = [tr("北"), tr("东北"), tr("东"), tr("东南"), tr("南"), tr("西南"), tr("西"), tr("西北")]
        var index = Math.round(((degrees % 360) + 360) % 360 / 45) % 8
        return names[index] + " " + degrees.toFixed(0) + "°"
    }

    function attitudeTextFor(vehicle, kind) {
        if (!vehicle || !vehicle[kind]) return "--"
        return numberText(vehicle[kind], 0, "°")
    }

    function attitudeText(kind) { return attitudeTextFor(focusVehicle, kind) }

    function vibrationAxisTextFor(vehicle, axis) {
        return vehicle && vehicle.vibration ? root.numberText(vehicle.vibration[axis], 2, "") : "--"
    }

    function vibrationAxisText(axis) { return vibrationAxisTextFor(root.focusVehicle, axis) }

function escFact(vehicle, prefix, motorIndex) {
        if (!vehicle || !vehicle.escStatus) return null
        var suffixes = ["First", "Second", "Third", "Fourth"]
        if (motorIndex < 0 || motorIndex >= suffixes.length) return null
        return vehicle.escStatus[prefix + suffixes[motorIndex]]
    }

    function escNumber(vehicle, prefix, motorIndex, decimals, suffix) {
        return escHasData(vehicle, motorIndex) ? numberText(escFact(vehicle, prefix, motorIndex), decimals, suffix) : "--"
    }

    function escFactNonZero(vehicle, prefix, motorIndex) {
        var fact = escFact(vehicle, prefix, motorIndex)
        return factHasValue(fact) && Math.abs(Number(fact.rawValue)) > 0.001
    }

    function escHasData(vehicle, motorIndex) {
        return escFactNonZero(vehicle, "rpm", motorIndex)
            || escFactNonZero(vehicle, "current", motorIndex)
            || escFactNonZero(vehicle, "voltage", motorIndex)
    }

    function escTipText(motorIndex) {
        var label = tr("电机 M%1").arg(motorIndex + 1)
        if (!focusVehicle) return label + "\n" + tr("等待飞行器接入后显示 ESC 遥测。")
        if (!escHasData(focusVehicle, motorIndex)) return label + "\n" + tr("等待 ESC_STATUS 遥测；收到后显示转速、电流、电压。")
        return label + "\n" +
               tr("转速：%1").arg(escNumber(focusVehicle, "rpm", motorIndex, 0, "rpm")) + "\n" +
               tr("电流：%1").arg(escNumber(focusVehicle, "current", motorIndex, 1, "A")) + "\n" +
               tr("电压：%1").arg(escNumber(focusVehicle, "voltage", motorIndex, 1, "V"))
    }

    function linkStateText(vehicle) {
        if (!vehicle) return tr("\u672a\u8fde\u63a5")
        if (vehicle.vehicleLinkManager && vehicle.vehicleLinkManager.communicationLost) return tr("\u94fe\u8def\u5f02\u5e38")
        return tr("\u5df2\u8fde\u63a5")
    }

    function runGuidedAction(actionId) {
        if (!guidedController || actionId < 0) return
        guidedController.closeAll()
        guidedController.confirmAction(actionId)
    }

    function showFloatingToolTip(sourceItem, text, align) {
        if (!sourceItem || !text) return
        var anchor = sourceItem.mapToItem(root, sourceItem.width, sourceItem.height + 6)
        floatingToolTip.text = text
        floatingToolTip.align = align ? align : "right"
        floatingToolTip.anchorX = anchor.x
        floatingToolTip.anchorY = anchor.y
    }

    function hideFloatingToolTip() { floatingToolTip.text = "" }

    function numericInput(value, fallback, minValue, maxValue) {
        var parsed = parseFloat(String(value).replace(",", "."))
        if (isNaN(parsed)) parsed = fallback
        return clamp(parsed, minValue, maxValue)
    }

    function controlValueText(kind) {
        return (kind === "altitude" ? altitudeCommandMeters : speedCommandMetersSecond).toFixed(1)
    }

    function setControlValue(kind, value) {
        if (kind === "altitude") altitudeCommandMeters = numericInput(value, altitudeCommandMeters, -100, 100)
        else if (kind === "speed") speedCommandMetersSecond = numericInput(value, speedCommandMetersSecond, 0.1, 40)
    }

    function adjustControlValue(kind, delta) {
        if (kind === "altitude") altitudeCommandMeters = numericInput(altitudeCommandMeters + delta, altitudeCommandMeters, -100, 100)
        else if (kind === "speed") speedCommandMetersSecond = numericInput(speedCommandMetersSecond + delta, speedCommandMetersSecond, 0.1, 40)
    }

    function canSendFocusCommand() {
        return focusVehicle && !(focusVehicle.vehicleLinkManager && focusVehicle.vehicleLinkManager.communicationLost)
    }

    function applyControlCommand(kind) {
        if (!canSendFocusCommand()) {
            mainWindow.showMessageDialog(tr("指令未发送"), tr("当前没有可用的焦点飞行器，或链路已断开。"))
            return
        }
        if (kind === "altitude") {
            focusVehicle.guidedModeChangeAltitude(altitudeCommandMeters, false)
        } else if (kind === "speed") {
            if (focusVehicle.fixedWing || focusVehicle.vtolInFwdFlight) focusVehicle.guidedModeChangeEquivalentAirspeedMetersSecond(speedCommandMetersSecond)
            else focusVehicle.guidedModeChangeGroundSpeedMetersSecond(speedCommandMetersSecond)
        }
    }

    Rectangle {
        id: fleetPanel
        z: 200
        anchors.left: parent.left
        anchors.leftMargin: root.margin
        anchors.top: parent.top
        anchors.topMargin: root.topInset + root.leftPanelTopExtra
        anchors.bottom: parent.bottom
        anchors.bottomMargin: root.margin + root.leftPanelBottomLift
        width: root.leftExpanded ? root.clamp(root.leftPanelWidth, root.leftPanelMinWidth, root.leftPanelMaxWidth) : root.collapsedPanelWidth
        color: root.panelColor
        border.color: root.panelLine
        radius: root.panelRadius
        clip: root.leftExpanded

        Behavior on width {
            enabled: !root.leftPanelResizing
            NumberAnimation { duration: 120; easing.type: Easing.OutCubic }
        }

        DeadMouseArea { anchors.fill: parent }

        MouseArea {
            id: fleetCollapsedMouse
            anchors.fill: parent
            visible: !root.leftExpanded
            hoverEnabled: true
            onClicked: root.leftExpanded = true

            QGCColoredImage {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 16
                width: 32
                height: 32
                source: "/qmlimages/compassInstrumentArrow.svg"
                color: root.accent
            }

            MerivusToolTip {
                visible: fleetCollapsedMouse.containsMouse
                text: tr("\u5c55\u5f00\u673a\u7fa4\u4e0e\u98de\u884c\u63a7\u5236")
                anchors.left: parent.right
                anchors.leftMargin: 8
                anchors.top: parent.top
                anchors.topMargin: 12
            }
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 7
            visible: root.leftExpanded

            RowLayout {
                Layout.fillWidth: true
                QGCLabel {
                    Layout.fillWidth: true
                    text: tr("\u98de\u884c\u63a7\u5236")
                    color: qgcPal.text
                    font.bold: true
                    font.pixelSize: 18
                }
                QGCLabel {
                    text: root.vehicles ? tr("%1 \u67b6\u5728\u7ebf\u5217\u8868").arg(root.vehicles.count) : tr("0 \u67b6")
                    color: qgcPal.colorGrey
                    font.pixelSize: 11
                }
                QGCButton {
                    id: leftCollapseButton
                    text: "\u2039"
                    Layout.preferredWidth: 28
                    Layout.preferredHeight: 26
                    onHoveredChanged: {
                        if (hovered) root.showFloatingToolTip(leftCollapseButton, tr("\u6536\u8d77\u9762\u677f"), "right")
                        else root.hideFloatingToolTip()
                    }
                    onClicked: {
                        root.hideFloatingToolTip()
                        root.leftExpanded = false
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 38
                radius: 4
                color: qgcPal.windowShade
                border.color: root.selectedCount() > 0 ? root.accent : root.mutedLine
                QGCLabel {
                    anchors.fill: parent
                    anchors.leftMargin: 9
                    anchors.rightMargin: 9
                    verticalAlignment: Text.AlignVCenter
                    text: root.selectedSummary()
                    color: root.selectedCount() > 0 ? root.accent : qgcPal.colorGrey
                    font.pixelSize: 12
                    font.bold: root.selectedCount() > 0
                    elide: Text.ElideRight
                }
            }

            QGCLabel {
                Layout.fillWidth: true
                text: root.focusVehicle ? tr("\u5f53\u524d\u7126\u70b9 UAV-%1").arg(root.focusVehicle.id) : tr("\u5f53\u524d\u65e0\u98de\u884c\u5668\u7126\u70b9")
                color: qgcPal.colorGrey
                font.pixelSize: 11
                elide: Text.ElideRight
            }

            GridLayout {
                Layout.fillWidth: true
                columns: 5
                columnSpacing: 7
                rowSpacing: 7
                Repeater {
                    model: [
                        { title: tr("\u4efb\u52a1\u89c4\u5212"), icon: "/qmlimages/Plan.svg", enabled: true, type: "plan", action: -1 },
                        { title: tr("\u8d77\u98de"), icon: "/res/takeoff.svg", enabled: root.guidedController && root.guidedController.showTakeoff, type: "guided", action: root.guidedController ? root.guidedController.actionTakeoff : -1 },
                        { title: tr("\u964d\u843d"), icon: "/res/land.svg", enabled: root.guidedController && root.guidedController.showLand, type: "guided", action: root.guidedController ? root.guidedController.actionLand : -1 },
                        { title: tr("\u8fd4\u822a"), icon: "/res/rtl.svg", enabled: root.guidedController && root.guidedController.showRTL, type: "guided", action: root.guidedController ? root.guidedController.actionRTL : -1 },
                        { title: tr("\u7f16\u961f"), icon: "/qmlimages/swarm.svg", enabled: root.guidedController && root.vehicleById(1), type: "guided", action: root.guidedController ? root.guidedController.actionSwarm : -1 }
                    ]
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 52
                        radius: 6
                        color: commandMouse.pressed ? qgcPal.windowShadeLight : (commandMouse.containsMouse ? root.raisedColor : qgcPal.windowShade)
                        border.color: modelData.enabled && (commandMouse.containsMouse || commandMouse.pressed) ? root.accent : root.mutedLine
                        opacity: modelData.enabled ? 1.0 : 0.45
                        Column {
                            anchors.centerIn: parent
                            spacing: 4
                            QGCColoredImage {
                                anchors.horizontalCenter: parent.horizontalCenter
                                width: 20
                                height: 20
                                source: modelData.icon
                                color: modelData.enabled ? (commandMouse.containsMouse ? root.accent : qgcPal.text) : root.muted
                            }
                            QGCLabel {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: modelData.title
                                color: qgcPal.text
                                font.bold: true
                                font.pixelSize: 11
                            }
                        }
                        MouseArea {
                            id: commandMouse
                            anchors.fill: parent
                            enabled: modelData.enabled
                            hoverEnabled: true
                            onClicked: modelData.type === "plan" ? mainWindow.showPlanView() : root.runGuidedAction(modelData.action)
                        }
                        MerivusToolTip {
                            visible: commandMouse.containsMouse
                            text: modelData.type === "plan" ? tr("\u6253\u5f00\u4efb\u52a1\u89c4\u5212") : tr("\u8fdb\u5165\u539f\u6709\u786e\u8ba4\u6d41\u7a0b")
                            anchors.left: parent.left
                            anchors.bottom: parent.top
                            anchors.bottomMargin: 6
                        }
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                QGCLabel {
                    Layout.fillWidth: true
                    text: root.focusVehicle ? "UAV-" + root.focusVehicle.id : tr("\u65e0\u4eba\u673a\u53c2\u6570")
                    color: qgcPal.text
                    font.bold: true
                    font.pixelSize: 14
                }
                QGCLabel {
                    text: "\u25cb " + root.linkStateText(root.focusVehicle)
                    color: root.focusVehicle && !(root.focusVehicle.vehicleLinkManager && root.focusVehicle.vehicleLinkManager.communicationLost) ? root.nominal : root.muted
                    font.pixelSize: 11
                }
            }

            QGCLabel {
                Layout.fillWidth: true
                text: root.focusVehicle ? ((root.focusVehicle.armed ? tr("\u5df2\u89e3\u9501") : tr("\u672a\u89e3\u9501")) + " / " + root.focusVehicle.flightMode) : tr("\u7b49\u5f85\u98de\u884c\u5668\u63a5\u5165")
                color: qgcPal.colorGrey
                font.pixelSize: 11
                elide: Text.ElideRight
            }

            GridLayout {
                Layout.fillWidth: true
                columns: 3
                columnSpacing: 5
                rowSpacing: 5
                Repeater {
                    model: [
                        { label: tr("\u9ad8\u5ea6"), kind: "altitude", adjustable: true, value: root.metricText("altitude", 1, "m"), step: 1.0, unit: "m", tip: tr("高度增量命令：正值上升、负值下降，默认 10 m；点击下发后通过当前焦点飞行器发送。") },
                        { label: tr("\u5730\u901f"), kind: "speed", adjustable: true, value: root.metricText("speed", 1, "m/s"), step: 0.5, unit: "m/s", tip: tr("目标速度命令：默认 5 m/s；多旋翼下发地速，固定翼/前飞 VTOL 下发等效空速。") },
                        { label: tr("\u722c\u5347"), kind: "climb", adjustable: false, value: root.metricText("climb", 1, "m/s"), tip: tr("垂直速度（正值上升、负值下降；默认单位 m/s）") },
                        { label: tr("\u822a\u5411"), kind: "heading", adjustable: false, value: root.metricText("heading", 0, "\u00b0"), tip: tr("机头朝向（0/360 为北向）") },
                        { label: tr("\u7535\u6c60"), kind: "battery", adjustable: false, value: root.metricText("battery", 0, "%"), tip: tr("主电池剩余百分比") },
                        { label: "GPS", kind: "gps", adjustable: false, value: root.metricText("gps", 0, tr("\u661f")), tip: tr("GPS 可用卫星数量") }
                    ]
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 76
                        radius: 5
                        color: paramTipMouse.containsMouse ? root.raisedColor : qgcPal.windowShade
                        border.color: modelData.adjustable ? (root.canSendFocusCommand() ? root.accent : root.mutedLine) : (paramTipMouse.containsMouse ? root.accent : root.mutedLine)
                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 5
                            spacing: 2
                            QGCLabel {
                                Layout.fillWidth: true
                                horizontalAlignment: Text.AlignHCenter
                                text: modelData.label
                                color: qgcPal.colorGrey
                                font.pixelSize: 10
                            }
                            RowLayout {
                                Layout.fillWidth: true
                                visible: modelData.adjustable
                                spacing: 3
                                QGCButton {
                                    text: "-"
                                    Layout.preferredWidth: 24
                                    Layout.preferredHeight: 24
                                    onClicked: {
                                        root.adjustControlValue(modelData.kind, -modelData.step)
                                        controlValueField.text = root.controlValueText(modelData.kind)
                                    }
                                }
                                QGCTextField {
                                    id: controlValueField
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 24
                                    text: root.controlValueText(modelData.kind)
                                    horizontalAlignment: Text.AlignHCenter
                                    font.pixelSize: 11
                                    validator: DoubleValidator { bottom: modelData.kind === "altitude" ? -100 : 0.1; top: modelData.kind === "altitude" ? 100 : 40; decimals: 1 }
                                    onEditingFinished: root.setControlValue(modelData.kind, text)
                                }
                                QGCButton {
                                    text: "+"
                                    Layout.preferredWidth: 24
                                    Layout.preferredHeight: 24
                                    onClicked: {
                                        root.adjustControlValue(modelData.kind, modelData.step)
                                        controlValueField.text = root.controlValueText(modelData.kind)
                                    }
                                }
                            }
                            QGCButton {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 22
                                visible: modelData.adjustable
                                enabled: root.canSendFocusCommand()
                                text: tr("下发") + " " + modelData.unit
                                onClicked: {
                                    root.setControlValue(modelData.kind, controlValueField.text)
                                    controlValueField.text = root.controlValueText(modelData.kind)
                                    root.applyControlCommand(modelData.kind)
                                }
                            }
                            QGCLabel {
                                Layout.fillWidth: true
                                visible: !modelData.adjustable
                                horizontalAlignment: Text.AlignHCenter
                                text: modelData.value
                                color: qgcPal.text
                                font.bold: true
                                font.pixelSize: 12
                            }
                        }
                        MouseArea {
                            id: paramTipMouse
                            anchors.fill: parent
                            hoverEnabled: true
                            acceptedButtons: Qt.NoButton
                        }
                        MerivusToolTip {
                            visible: paramTipMouse.containsMouse
                            text: modelData.tip
                            anchors.left: parent.left
                            anchors.bottom: parent.top
                            anchors.bottomMargin: 6
                            maximumWidth: 280
                        }
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                QGCLabel {
                    Layout.fillWidth: true
                    text: tr("\u7535\u673a / ESC")
                    color: qgcPal.text
                    font.bold: true
                    font.pixelSize: 12
                }
                QGCLabel {
                    text: root.focusVehicle ? tr("\u5df2\u63a5\u5165") : tr("\u65e0\u6570\u636e\u6e90")
                    color: root.focusVehicle ? root.nominal : root.muted
                    font.pixelSize: 11
                }
            }

            GridLayout {
                Layout.fillWidth: true
                columns: 4
                columnSpacing: 5
                rowSpacing: 5
                Repeater {
                    model: [
                        { label: "M1", index: 0, direction: "CW" },
                        { label: "M2", index: 1, direction: "CCW" },
                        { label: "M3", index: 2, direction: "CW" },
                        { label: "M4", index: 3, direction: "CCW" }
                    ]
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 56
                        radius: 5
                        color: escMouse.containsMouse ? root.raisedColor : qgcPal.windowShade
                        border.color: root.escHasData(root.focusVehicle, modelData.index) ? root.nominal : (escMouse.containsMouse ? root.accent : root.mutedLine)
                        Column {
                            anchors.fill: parent
                            anchors.margins: 5
                            spacing: 1
                            Row {
                                anchors.horizontalCenter: parent.horizontalCenter
                                spacing: 3
                                QGCLabel { text: modelData.label; color: qgcPal.text; font.bold: true; font.pixelSize: 11 }
                                QGCLabel { text: root.escHasData(root.focusVehicle, modelData.index) ? "\u25cf" : "\u25cb"; color: root.escHasData(root.focusVehicle, modelData.index) ? root.nominal : root.muted; font.pixelSize: 9 }
                            }
                            Row {
                                anchors.horizontalCenter: parent.horizontalCenter
                                spacing: 3
                                QGCLabel {
                                    text: root.escNumber(root.focusVehicle, "rpm", modelData.index, 0, "")
                                    color: qgcPal.text
                                    font.bold: true
                                    font.pixelSize: 12
                                }
                                QGCLabel {
                                    text: tr("rpm")
                                    color: qgcPal.colorGrey
                                    font.pixelSize: 9
                                }
                            }
                            QGCLabel {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: modelData.direction
                                color: root.accent
                                font.bold: true
                                font.pixelSize: 10
                            }
                        }
                        MouseArea {
                            id: escMouse
                            anchors.fill: parent
                            hoverEnabled: true
                            acceptedButtons: Qt.NoButton
                        }
                        MerivusToolTip {
                            visible: escMouse.containsMouse
                            text: root.escTipText(modelData.index)
                            anchors.left: parent.left
                            anchors.bottom: parent.top
                            anchors.bottomMargin: 6
                            maximumWidth: 240
                        }
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                QGCLabel {
                    Layout.fillWidth: true
                    text: tr("\u65e0\u4eba\u673a\u5217\u8868")
                    color: qgcPal.text
                    font.bold: true
                    font.pixelSize: 13
                }
                QGCButton {
                    text: tr("\u6e05\u9664")
                    visible: root.selectedCount() > 0
                    onClicked: root.clearSelectionRequested()
                }
            }

            QGCFlickable {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                contentWidth: width
                contentHeight: vehicleColumn.implicitHeight

                ColumnLayout {
                    id: vehicleColumn
                    width: parent.width
                    spacing: 6

                    Repeater {
                        model: root.vehicles ? root.vehicles : 0
                        delegate: Rectangle {
                            id: vehicleRow
                            property var vehicle: object
                            Layout.fillWidth: true
                            Layout.preferredHeight: 44
                            radius: 5
                            color: root.isSelected(vehicle.id) ? Qt.rgba(root.accent.r, root.accent.g, root.accent.b, 0.18) : qgcPal.windowShade
                            border.color: root.isSelected(vehicle.id) ? root.accent : root.mutedLine
                            border.width: root.isSelected(vehicle.id) ? 2 : 1

                            Rectangle {
                                visible: root.isSelected(vehicle.id)
                                anchors.fill: parent
                                anchors.margins: 2
                                radius: 4
                                color: "transparent"
                                border.color: Qt.rgba(root.accent.r, root.accent.g, root.accent.b, 0.48)
                                border.width: 1
                            }
                            Rectangle {
                                visible: root.isSelected(vehicle.id)
                                anchors.left: parent.left
                                anchors.top: parent.top
                                anchors.bottom: parent.bottom
                                width: 4
                                radius: 2
                                color: root.accent
                            }

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 8
                                anchors.rightMargin: 8
                                spacing: 6
                                QGCLabel {
                                    Layout.preferredWidth: 62
                                    text: vehicle ? "UAV-" + vehicle.id : "UAV--"
                                    color: root.isSelected(vehicle.id) ? root.accent : qgcPal.text
                                    font.bold: true
                                    font.pixelSize: 12
                                }
                                QGCLabel {
                                    Layout.fillWidth: true
                                    text: vehicle ? ((vehicle.armed ? tr("\u5df2\u89e3\u9501") : tr("\u672a\u89e3\u9501")) + " / " + vehicle.flightMode) : "--"
                                    color: qgcPal.colorGrey
                                    font.pixelSize: 11
                                    elide: Text.ElideRight
                                }
                                QGCLabel {
                                    text: root.batteryPercent(vehicle)
                                    color: qgcPal.text
                                    font.pixelSize: 11
                                }
                            }

                            MouseArea {
                                anchors.fill: parent
                                acceptedButtons: Qt.LeftButton
                                onClicked: {
                                    root.vehicleFocusRequested(vehicle.id)
                                    root.vehicleSelectionRequested(vehicle.id, !root.isSelected(vehicle.id))
                                }
                            }
                        }
                    }

                    QGCLabel {
                        width: parent.width
                        visible: !root.vehicles || root.vehicles.count === 0
                        text: tr("\u6682\u65e0\u63a5\u5165\u98de\u884c\u5668")
                        color: qgcPal.colorGrey
                        font.pixelSize: 12
                    }
                }
            }
        }

        MouseArea {
            id: leftWidthResizeArea
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: root.resizeHandleThickness
            visible: root.leftExpanded
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton
            preventStealing: true
            propagateComposedEvents: false
            cursorShape: Qt.SizeHorCursor
            property real startWidth: 0
            property real startMouseX: 0
            onPressed: {
                mouse.accepted = true
                root.leftPanelResizing = true
                startWidth = root.leftPanelWidth
                startMouseX = mapToItem(root, mouse.x, mouse.y).x
            }
            onPositionChanged: {
                if (pressed) {
                    var currentMouseX = mapToItem(root, mouse.x, mouse.y).x
                    root.leftPanelWidth = root.clamp(startWidth + currentMouseX - startMouseX, root.leftPanelMinWidth, root.leftPanelMaxWidth)
                }
            }
            onReleased: root.leftPanelResizing = false
            onCanceled: root.leftPanelResizing = false
            Rectangle {
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                width: 2
                height: Math.min(120, parent.height * 0.28)
                radius: 1
                color: leftWidthResizeArea.containsMouse || leftWidthResizeArea.pressed ? root.accent : root.mutedLine
            }
        }

        MouseArea {
            id: leftTopResizeArea
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            height: root.resizeHandleThickness
            visible: root.leftExpanded
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton
            preventStealing: true
            propagateComposedEvents: false
            cursorShape: Qt.SizeVerCursor
            property real startTopExtra: 0
            property real startMouseY: 0
            onPressed: {
                mouse.accepted = true
                root.leftPanelResizing = true
                startTopExtra = root.leftPanelTopExtra
                startMouseY = mapToItem(root, mouse.x, mouse.y).y
            }
            onPositionChanged: {
                if (pressed) {
                    var currentMouseY = mapToItem(root, mouse.x, mouse.y).y
                    var maxTop = Math.max(0, root.height - root.topInset - root.margin - root.leftPanelBottomLift - root.sidePanelMinHeight)
                    root.leftPanelTopExtra = root.clamp(startTopExtra + currentMouseY - startMouseY, 0, maxTop)
                }
            }
            onReleased: root.leftPanelResizing = false
            onCanceled: root.leftPanelResizing = false
            Rectangle {
                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter
                width: Math.min(92, parent.width * 0.32)
                height: 2
                radius: 1
                color: leftTopResizeArea.containsMouse || leftTopResizeArea.pressed ? root.accent : root.mutedLine
            }
        }

        MouseArea {
            id: leftBottomResizeArea
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: root.resizeHandleThickness
            visible: root.leftExpanded
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton
            preventStealing: true
            propagateComposedEvents: false
            cursorShape: Qt.SizeVerCursor
            property real startBottomLift: 0
            property real startMouseY: 0
            onPressed: {
                mouse.accepted = true
                root.leftPanelResizing = true
                startBottomLift = root.leftPanelBottomLift
                startMouseY = mapToItem(root, mouse.x, mouse.y).y
            }
            onPositionChanged: {
                if (pressed) {
                    var currentMouseY = mapToItem(root, mouse.x, mouse.y).y
                    var maxBottom = Math.max(root.panelBottomLiftMin, root.height - root.topInset - root.leftPanelTopExtra - root.margin - root.sidePanelMinHeight)
                    root.leftPanelBottomLift = root.clamp(startBottomLift - (currentMouseY - startMouseY), root.panelBottomLiftMin, maxBottom)
                }
            }
            onReleased: root.leftPanelResizing = false
            onCanceled: root.leftPanelResizing = false
            Rectangle {
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                width: Math.min(92, parent.width * 0.32)
                height: 2
                radius: 1
                color: leftBottomResizeArea.containsMouse || leftBottomResizeArea.pressed ? root.accent : root.mutedLine
            }
        }
    }

    Rectangle {
        id: rightPanel
        z: 200
        anchors.right: parent.right
        anchors.rightMargin: root.margin
        anchors.top: parent.top
        anchors.topMargin: root.topInset + root.rightPanelTopExtra
        anchors.bottom: parent.bottom
        anchors.bottomMargin: root.margin + root.rightPanelBottomLift
        width: root.rightExpanded ? root.clamp(root.rightPanelWidth, root.rightPanelMinWidth, root.rightPanelMaxWidth) : root.collapsedPanelWidth
        color: root.panelColor
        border.color: root.panelLine
        radius: root.panelRadius
        clip: root.rightExpanded

        Behavior on width {
            enabled: !root.rightPanelResizing
            NumberAnimation { duration: 120; easing.type: Easing.OutCubic }
        }

        DeadMouseArea { anchors.fill: parent }

        MouseArea {
            id: environmentCollapsedMouse
            anchors.fill: parent
            visible: !root.rightExpanded
            hoverEnabled: true
            onClicked: root.rightExpanded = true

            QGCColoredImage {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 16
                width: 32
                height: 32
                source: "/qmlimages/compassInstrumentArrow.svg"
                color: root.accent
            }

            MerivusToolTip {
                visible: environmentCollapsedMouse.containsMouse
                text: tr("\u5c55\u5f00\u73af\u5883\u3001\u59ff\u6001\u4e0e\u89c6\u9891")
                anchors.right: parent.left
                anchors.rightMargin: 8
                anchors.top: parent.top
                anchors.topMargin: 12
            }
        }

        QGCFlickable {
            anchors.fill: parent
            anchors.margins: 12
            visible: root.rightExpanded
            clip: true
            contentWidth: width
            contentHeight: rightContent.implicitHeight

            ColumnLayout {
                id: rightContent
                width: parent.width
                spacing: 10

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 6
                    QGCLabel {
                        Layout.fillWidth: true
                        text: tr("环境 / 姿态 / 视频")
                        color: qgcPal.text
                        font.bold: true
                        font.pixelSize: 18
                    }
                    RowLayout {
                        visible: root.selectedCount() > 1
                        spacing: 4
                        Repeater {
                            model: root.selectedIds
                            Rectangle {
                                width: 58
                                height: 26
                                radius: 5
                                color: root.rightVehicleSelected(modelData) ? Qt.rgba(root.accent.r, root.accent.g, root.accent.b, 0.22) : qgcPal.windowShade
                                border.color: root.rightVehicleSelected(modelData) ? root.accent : root.mutedLine
                                Row {
                                    anchors.centerIn: parent
                                    spacing: 3
                                    Rectangle {
                                        width: 10
                                        height: 10
                                        radius: 2
                                        color: root.rightVehicleSelected(modelData) ? root.accent : "transparent"
                                        border.color: root.rightVehicleSelected(modelData) ? root.accent : qgcPal.colorGrey
                                    }
                                    QGCLabel { text: "UAV-" + modelData; color: root.rightVehicleSelected(modelData) ? root.accent : qgcPal.text; font.bold: true; font.pixelSize: 10 }
                                }
                                MouseArea { anchors.fill: parent; onClicked: root.rightPanelVehicleId = modelData }
                            }
                        }
                    }
                    Rectangle {
                        visible: root.selectedCount() <= 1
                        Layout.preferredWidth: 70
                        Layout.preferredHeight: 26
                        radius: 5
                        color: root.rightVehicle ? Qt.rgba(root.accent.r, root.accent.g, root.accent.b, 0.18) : qgcPal.windowShade
                        border.color: root.rightVehicle ? root.accent : root.mutedLine
                        QGCLabel {
                            anchors.centerIn: parent
                            text: root.rightVehicle ? "UAV-" + root.rightVehicle.id : tr("无数据")
                            color: root.rightVehicle ? root.accent : root.muted
                            font.bold: true
                            font.pixelSize: 11
                        }
                    }
                    QGCButton {
                        id: rightCollapseButton
                        text: "›"
                        Layout.preferredWidth: 32
                        Layout.preferredHeight: 30
                        onHoveredChanged: {
                            if (hovered) root.showFloatingToolTip(rightCollapseButton, tr("收起面板"), "right")
                            else root.hideFloatingToolTip()
                        }
                        onClicked: {
                            root.hideFloatingToolTip()
                            root.rightExpanded = false
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 150
                    radius: 7
                    color: qgcPal.windowShadeDark
                    border.color: root.mutedLine
                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        spacing: 7
                        RowLayout {
                            Layout.fillWidth: true
                            QGCLabel { Layout.fillWidth: true; text: tr("环境参数"); color: qgcPal.text; font.bold: true; font.pixelSize: 14 }
                        }
                        GridLayout {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            columns: 3
                            columnSpacing: 7
                            rowSpacing: 7
                            Repeater {
                                model: [
                                    { label: tr("日期"), value: Qt.formatDate(root.now, "yyyy-MM-dd")},
                                    { label: tr("时间"), value: Qt.formatTime(root.now, "HH:mm:ss")},
                                    { label: tr("风向"), value: root.windDirectionText(root.rightVehicle)},
                                    { label: tr("温度"), value: root.temperatureText(root.rightVehicle)},
                                    { label: tr("湿度"), value: root.humidityText(root.rightVehicle)},
                                    { label: tr("风速"), value: root.windSpeedText(root.rightVehicle)}
                                ]
                                Rectangle {
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    radius: 5
                                    color: qgcPal.windowShade
                                    border.color: root.mutedLine
                                    Column {
                                        anchors.centerIn: parent
                                        spacing: 1
                                        QGCLabel { anchors.horizontalCenter: parent.horizontalCenter; text: modelData.label; color: qgcPal.colorGrey; font.pixelSize: 10 }
                                        QGCLabel { anchors.horizontalCenter: parent.horizontalCenter; text: modelData.value; color: qgcPal.text; font.bold: true; font.pixelSize: 13; elide: Text.ElideRight }
                                        QGCLabel { anchors.horizontalCenter: parent.horizontalCenter; text: modelData.detail; color: qgcPal.colorGrey; font.pixelSize: 9; elide: Text.ElideRight }
                                    }
                                }
                            }
                        }
                    }
                }

                Rectangle {
                    id: sensorBlock
                    property real instrumentSize: Math.min(104, Math.max(82, (rightPanel.width - 72) * 0.28))

                    Layout.fillWidth: true
                    Layout.preferredHeight: 304
                    radius: 8
                    color: qgcPal.windowShadeDark
                    border.color: root.mutedLine
                    clip: true

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        spacing: 8

                        RowLayout {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 20
                            QGCLabel {
                                Layout.fillWidth: true
                                text: tr("姿态与传感器")
                                color: qgcPal.text
                                font.bold: true
                                font.pixelSize: 14
                            }
                            QGCLabel {
                                text: tr("R %1 / P %2 / H %3").arg(root.attitudeTextFor(root.rightVehicle, "roll")).arg(root.attitudeTextFor(root.rightVehicle, "pitch")).arg(root.metricTextFor(root.rightVehicle, "heading", 0, "°"))
                                color: qgcPal.colorGrey
                                font.pixelSize: 10
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 146
                            spacing: 8

                            Rectangle {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                radius: 7
                                color: qgcPal.windowShade
                                border.color: root.mutedLine
                                clip: true
                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 7
                                    spacing: 3
                                    RowLayout {
                                        Layout.fillWidth: true
                                        QGCLabel { Layout.fillWidth: true; text: tr("水平仪"); color: qgcPal.text; font.bold: true; font.pixelSize: 13 }
                                    }
                                    QGCAttitudeWidget {
                                        Layout.alignment: Qt.AlignHCenter
                                        Layout.preferredWidth: sensorBlock.instrumentSize
                                        Layout.preferredHeight: sensorBlock.instrumentSize
                                        vehicle: root.rightVehicle
                                        size: sensorBlock.instrumentSize
                                        showHeading: false
                                        showPitch: true
                                    }
                                }
                                MouseArea {
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    acceptedButtons: Qt.NoButton
                                    onContainsMouseChanged: containsMouse ? root.showFloatingToolTip(this, tr("Attitude indicator: displays roll and pitch from the selected vehicle attitude telemetry."), "left") : root.hideFloatingToolTip()
                                }
                            }

                            Rectangle {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                radius: 7
                                color: qgcPal.windowShade
                                border.color: root.mutedLine
                                clip: true
                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 7
                                    spacing: 3
                                    RowLayout {
                                        Layout.fillWidth: true
                                        QGCLabel { Layout.fillWidth: true; text: tr("指南针"); color: qgcPal.text; font.bold: true; font.pixelSize: 13 }
                                        QGCLabel { text: root.metricTextFor(root.rightVehicle, "heading", 0, "°"); color: root.accent; font.bold: true; font.pixelSize: 12 }
                                    }
                                    Rectangle {
                                        id: compassFace
                                        Layout.alignment: Qt.AlignHCenter
                                        Layout.preferredWidth: sensorBlock.instrumentSize
                                        Layout.preferredHeight: sensorBlock.instrumentSize
                                        radius: width / 2
                                        color: Qt.rgba(qgcPal.window.r, qgcPal.window.g, qgcPal.window.b, 0.36)
                                        border.color: root.panelLine
                                        border.width: 1

                                        Repeater {
                                            model: [
                                                { label: "N", x: 0.47, y: 0.04, c: root.accent },
                                                { label: "E", x: 0.84, y: 0.45, c: qgcPal.colorGrey },
                                                { label: "S", x: 0.47, y: 0.84, c: qgcPal.colorGrey },
                                                { label: "W", x: 0.08, y: 0.45, c: qgcPal.colorGrey }
                                            ]
                                            QGCLabel {
                                                x: compassFace.width * modelData.x
                                                y: compassFace.height * modelData.y
                                                text: modelData.label
                                                color: modelData.c
                                                font.bold: modelData.label === "N"
                                                font.pixelSize: 10
                                            }
                                        }
                                        QGCColoredImage {
                                            anchors.centerIn: parent
                                            width: compassFace.width * 0.46
                                            height: width
                                            source: "/qmlimages/compassInstrumentArrow.svg"
                                            color: root.rightVehicle ? root.accent : root.muted
                                            rotation: root.rightVehicle && root.rightVehicle.heading ? Number(root.rightVehicle.heading.rawValue) : 0
                                            transformOrigin: Item.Center
                                        }
                                    }
                                }
                                MouseArea {
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    acceptedButtons: Qt.NoButton
                                    onContainsMouseChanged: containsMouse ? root.showFloatingToolTip(this, tr("Compass: displays the selected vehicle heading; 0/360 degrees points north."), "left") : root.hideFloatingToolTip()
                                }
                            }
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 112
                            radius: 7
                            color: qgcPal.windowShade
                            border.color: root.mutedLine
                            clip: true
                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 8
                                spacing: 6
                                RowLayout {
                                    Layout.fillWidth: true
                                    QGCLabel { Layout.fillWidth: true; text: tr("IMU / 振动"); color: qgcPal.text; font.bold: true; font.pixelSize: 13 }
                                    QGCLabel {
                                        text: root.rightVehicle && root.rightVehicle.vibration ? tr("实时三轴") : tr("等待数据")
                                        color: root.rightVehicle && root.rightVehicle.vibration ? root.nominal : root.muted
                                        font.pixelSize: 10
                                    }
                                }
                                RowLayout {
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    spacing: 6
                                    Repeater {
                                        model: [
                                            { label: "X", axis: "xAxis", tip: tr("X-axis vibration: lateral vibration level for spotting IMU or frame anomalies.") },
                                            { label: "Y", axis: "yAxis", tip: tr("Y-axis vibration: longitudinal vibration level for spotting IMU or frame anomalies.") },
                                            { label: "Z", axis: "zAxis", tip: tr("Z-axis vibration: vertical vibration level for spotting propeller, motor, or mounting resonance.") }
                                        ]
                                        Rectangle {
                                            Layout.fillWidth: true
                                            Layout.fillHeight: true
                                            radius: 6
                                            color: qgcPal.windowShadeDark
                                            border.color: root.rightVehicle && root.rightVehicle.vibration ? root.accent : root.mutedLine
                                            Column {
                                                anchors.centerIn: parent
                                                spacing: 3
                                                QGCLabel { anchors.horizontalCenter: parent.horizontalCenter; text: modelData.label; color: root.accent; font.bold: true; font.pixelSize: 13 }
                                                QGCLabel { anchors.horizontalCenter: parent.horizontalCenter; text: root.vibrationAxisTextFor(root.rightVehicle, modelData.axis); color: qgcPal.text; font.bold: true; font.pixelSize: 14 }
                                                Rectangle {
                                                    anchors.horizontalCenter: parent.horizontalCenter
                                                    width: Math.max(34, parent.width * 0.45)
                                                    height: 3
                                                    radius: 2
                                                    color: root.rightVehicle && root.rightVehicle.vibration ? root.accent : root.mutedLine
                                                }
                                                QGCLabel { anchors.horizontalCenter: parent.horizontalCenter; text: tr("vibe"); color: qgcPal.colorGrey; font.pixelSize: 9 }
                                            }
                                            MouseArea {
                                                anchors.fill: parent
                                                hoverEnabled: true
                                                acceptedButtons: Qt.NoButton
                                                onContainsMouseChanged: containsMouse ? root.showFloatingToolTip(this, modelData.tip, "left") : root.hideFloatingToolTip()
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                RowLayout {
                    Layout.fillWidth: true
                    QGCLabel { Layout.fillWidth: true; text: tr("视频监控预留区"); color: qgcPal.text; font.bold: true; font.pixelSize: 15 }
                    QGCLabel { text: "16:9"; color: root.accent; font.bold: true; font.pixelSize: 12 }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: Math.min(240, Math.max(150, (rightPanel.width - 24) * 9 / 16))
                    radius: 8
                    color: qgcPal.windowShadeDark
                    border.color: root.accent
                    border.width: 1
                    Rectangle { anchors.fill: parent; anchors.margins: 6; radius: 6; color: "transparent"; border.color: root.mutedLine }
                    Column {
                        anchors.centerIn: parent
                        spacing: 8
                        QGCColoredImage { anchors.horizontalCenter: parent.horizontalCenter; width: 44; height: 44; source: "/qmlimages/camera_video.svg"; color: root.muted }
                        QGCLabel { anchors.horizontalCenter: parent.horizontalCenter; text: tr("16:9 视频画面区域"); color: qgcPal.text; font.bold: true; font.pixelSize: 15 }
                        QGCLabel { anchors.horizontalCenter: parent.horizontalCenter; text: tr("后续接入 4G 链路视频流 / 吊舱画面"); color: qgcPal.colorGrey; font.pixelSize: 11 }
                    }
                    QGCLabel { anchors.left: parent.left; anchors.leftMargin: 12; anchors.bottom: parent.bottom; anchors.bottomMargin: 8; text: tr("VIDEO · STANDBY"); color: root.muted; font.pixelSize: 10 }
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8
                    QGCButton {
                        id: videoRecordButton
                        Layout.fillWidth: true
                        Layout.preferredHeight: 30
                        text: (QGroundControl.videoManager && QGroundControl.videoManager.recording ? tr("停止录制") : tr("● 录制"))
                        onHoveredChanged: {
                            if (hovered) root.showFloatingToolTip(videoRecordButton, tr("开始或停止视频流录制"), "right")
                            else root.hideFloatingToolTip()
                        }
                        onClicked: {
                            root.hideFloatingToolTip()
                            if (QGroundControl.videoManager) {
                                if (QGroundControl.videoManager.recording) QGroundControl.videoManager.stopRecording()
                                else QGroundControl.videoManager.startRecording()
                            }
                        }
                    }
                    QGCButton {
                        id: videoSettingsButton
                        Layout.fillWidth: true
                        Layout.preferredHeight: 30
                        text: tr("视频设置")
                        onHoveredChanged: {
                            if (hovered) root.showFloatingToolTip(videoSettingsButton, tr("打开应用设置中的视频配置"), "right")
                            else root.hideFloatingToolTip()
                        }
                        onClicked: {
                            root.hideFloatingToolTip()
                            QGroundControl.saveGlobalSetting("Merivus/AppSettingsPage", "qrc:/qml/GeneralSettings.qml")
                            mainWindow.showSettingsTool()
                        }
                    }
                }
            }
        }

        MouseArea {
            id: rightWidthResizeArea
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: root.resizeHandleThickness
            visible: root.rightExpanded
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton
            preventStealing: true
            propagateComposedEvents: false
            cursorShape: Qt.SizeHorCursor
            property real startWidth: 0
            property real startMouseX: 0
            onPressed: {
                mouse.accepted = true
                root.rightPanelResizing = true
                startWidth = root.rightPanelWidth
                startMouseX = mapToItem(root, mouse.x, mouse.y).x
            }
            onPositionChanged: {
                if (pressed) {
                    var currentMouseX = mapToItem(root, mouse.x, mouse.y).x
                    root.rightPanelWidth = root.clamp(startWidth - (currentMouseX - startMouseX), root.rightPanelMinWidth, root.rightPanelMaxWidth)
                }
            }
            onReleased: root.rightPanelResizing = false
            onCanceled: root.rightPanelResizing = false
            Rectangle {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                width: 2
                height: Math.min(120, parent.height * 0.28)
                radius: 1
                color: rightWidthResizeArea.containsMouse || rightWidthResizeArea.pressed ? root.accent : root.mutedLine
            }
        }

        MouseArea {
            id: rightTopResizeArea
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            height: root.resizeHandleThickness
            visible: root.rightExpanded
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton
            preventStealing: true
            propagateComposedEvents: false
            cursorShape: Qt.SizeVerCursor
            property real startTopExtra: 0
            property real startMouseY: 0
            onPressed: {
                mouse.accepted = true
                root.rightPanelResizing = true
                startTopExtra = root.rightPanelTopExtra
                startMouseY = mapToItem(root, mouse.x, mouse.y).y
            }
            onPositionChanged: {
                if (pressed) {
                    var currentMouseY = mapToItem(root, mouse.x, mouse.y).y
                    var maxTop = Math.max(0, root.height - root.topInset - root.margin - root.rightPanelBottomLift - root.sidePanelMinHeight)
                    root.rightPanelTopExtra = root.clamp(startTopExtra + currentMouseY - startMouseY, 0, maxTop)
                }
            }
            onReleased: root.rightPanelResizing = false
            onCanceled: root.rightPanelResizing = false
            Rectangle {
                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter
                width: Math.min(92, parent.width * 0.32)
                height: 2
                radius: 1
                color: rightTopResizeArea.containsMouse || rightTopResizeArea.pressed ? root.accent : root.mutedLine
            }
        }

        MouseArea {
            id: rightBottomResizeArea
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: root.resizeHandleThickness
            visible: root.rightExpanded
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton
            preventStealing: true
            propagateComposedEvents: false
            cursorShape: Qt.SizeVerCursor
            property real startBottomLift: 0
            property real startMouseY: 0
            onPressed: {
                mouse.accepted = true
                root.rightPanelResizing = true
                startBottomLift = root.rightPanelBottomLift
                startMouseY = mapToItem(root, mouse.x, mouse.y).y
            }
            onPositionChanged: {
                if (pressed) {
                    var currentMouseY = mapToItem(root, mouse.x, mouse.y).y
                    var maxBottom = Math.max(root.panelBottomLiftMin, root.height - root.topInset - root.rightPanelTopExtra - root.margin - root.sidePanelMinHeight)
                    root.rightPanelBottomLift = root.clamp(startBottomLift - (currentMouseY - startMouseY), root.panelBottomLiftMin, maxBottom)
                }
            }
            onReleased: root.rightPanelResizing = false
            onCanceled: root.rightPanelResizing = false
            Rectangle {
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                width: Math.min(92, parent.width * 0.32)
                height: 2
                radius: 1
                color: rightBottomResizeArea.containsMouse || rightBottomResizeArea.pressed ? root.accent : root.mutedLine
            }
        }
    }

    MerivusToolTip {
        id: floatingToolTip
        property real anchorX: 0
        property real anchorY: 0
        property string align: "right"

        z: 1000000
        x: root.clamp(align === "right" ? anchorX - width : anchorX, 0, Math.max(0, root.width - width))
        y: root.clamp(anchorY, 0, Math.max(0, root.height - height))
    }
}
