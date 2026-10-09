import QtQuick          2.12
import QtQuick.Controls 2.4
import QtQuick.Layouts  1.11
import QGroundControl             1.0
import QGroundControl.Controls    1.0
import QGroundControl.Palette     1.0
import QGroundControl.ScreenTools 1.0

Rectangle {
    id: root

    property var vehicle
    property var statusPalette: localPalette
    readonly property var ftc: vehicle ? vehicle.ftcStatus : null
    readonly property bool fresh: !!ftc && ftc.available && !ftc.stale && ftc.protocolCompatible
    readonly property bool systemFresh: fresh && ((ftc.motorAvailable && !ftc.motorStale) || (ftc.controlAvailable && !ftc.controlStale))
    readonly property bool controlFresh: fresh && ftc.controlAvailable && !ftc.controlStale
    readonly property bool extremeFresh: fresh && ftc.extremeAvailable && !ftc.extremeStale
    readonly property bool locCritical: extremeFresh && ftc.locSeverity === "critical"
    readonly property bool recoveryVisible: (controlFresh || extremeFresh) && ftc.recoveryState > 1
    readonly property bool degradedVisible: !locCritical && ((systemFresh && ftc.systemSeverity !== "normal")
                                                           || (controlFresh && ftc.authoritySeverity !== "normal")
                                                           || (extremeFresh && ftc.locSeverity === "warning"))
    readonly property bool nominal: systemFresh && ftc.systemSeverity === "normal"
                                    && (!controlFresh || ftc.authoritySeverity === "normal")
                                    && (!extremeFresh || ftc.locSeverity === "normal")
                                    && !recoveryVisible
    readonly property string panelSeverity: locCritical
                                             ? "critical"
                                             : (recoveryVisible && ftc.recoverySeverity === "critical"
                                                ? "critical"
                                                : (degradedVisible || recoveryVisible ? "warning"
                                                                                      : (nominal ? "normal" : "unavailable")))

    radius: 5
    color: qgcPal.windowShade
    border.color: statusPalette.colorFor(panelSeverity)
    implicitHeight: compactColumn.implicitHeight + 12

    QGCPalette { id: qgcPal; colorGroupEnabled: true }
    FtcStatusPalette { id: localPalette }

    function percentText(value) {
        return value >= 0 ? Number(value).toFixed(0) + "%" : qsTr("N/A")
    }

    function statusText() {
        if (!vehicle) return qsTr("未选择飞行器")
        if (!ftc) return qsTr("当前版本没有 FTC 后端")
        if (ftc.protocolVersion > 0 && !ftc.protocolCompatible) return qsTr("协议版本不兼容：v%1").arg(ftc.protocolVersion)
        if (!ftc.available) return qsTr("未收到 FTC 遥测")
        if (ftc.stale) return qsTr("FTC 数据已过期")
        if (!systemFresh) return qsTr("FTC 系统状态：N/A")
        return ftc.systemStateText
    }

    function warningText() {
        if (!ftc) return ""

        var warnings = []
        if (systemFresh && ftc.systemSeverity !== "normal") warnings.push(ftc.systemStateText)
        if (controlFresh && ftc.authoritySeverity !== "normal") warnings.push(ftc.authorityStateText)
        if (extremeFresh && ftc.locSeverity === "warning") warnings.push(ftc.locStateText)

        var text = qsTr("注意：%1").arg(warnings.join(qsTr(" · ")))
        if (controlFresh) text += qsTr(" · 最小姿态裕度 %1").arg(percentText(ftc.minimumAttitudeAuthority))
        return text
    }

    ColumnLayout {
        id: compactColumn
        anchors.fill: parent
        anchors.margins: 6
        spacing: 4

        RowLayout {
            Layout.fillWidth: true
            QGCLabel {
                Layout.fillWidth: true
                text: root.nominal ? qsTr("FTC · 正常") : qsTr("FTC · %1").arg(root.statusText())
                color: root.statusPalette.colorFor(root.panelSeverity)
                font.bold: true
                font.pointSize: ScreenTools.smallFontPointSize
            }
            Rectangle {
                Layout.preferredWidth: modeLabel.implicitWidth + 12
                Layout.preferredHeight: 22
                radius: 6
                color: Qt.rgba(statusPalette.colorFor(root.controlFresh ? root.ftc.controlModeSeverity : "unavailable").r,
                               statusPalette.colorFor(root.controlFresh ? root.ftc.controlModeSeverity : "unavailable").g,
                               statusPalette.colorFor(root.controlFresh ? root.ftc.controlModeSeverity : "unavailable").b, 0.16)
                border.color: statusPalette.colorFor(root.controlFresh ? root.ftc.controlModeSeverity : "unavailable")
                QGCLabel {
                    id: modeLabel
                    anchors.centerIn: parent
                    text: root.controlFresh ? root.ftc.controlModeText : qsTr("N/A")
                    color: qgcPal.text
                    font.pointSize: ScreenTools.smallFontPointSize
                }
            }
            QGCButton {
                text: qsTr("详情")
                enabled: !!root.ftc && root.ftc.available && root.ftc.protocolCompatible
                onClicked: detailsPopup.open()
            }
        }

        Rectangle {
            Layout.fillWidth: true
            visible: root.degradedVisible
            implicitHeight: degradedLabel.implicitHeight + 10
            radius: 4
            color: Qt.rgba(root.statusPalette.colorFor("warning").r,
                           root.statusPalette.colorFor("warning").g,
                           root.statusPalette.colorFor("warning").b, 0.14)
            border.color: root.statusPalette.colorFor("warning")
            QGCLabel {
                id: degradedLabel
                anchors.fill: parent
                anchors.margins: 5
                text: root.warningText()
                color: qgcPal.text
                wrapMode: Text.WordWrap
                font.bold: true
            }
        }

        Rectangle {
            Layout.fillWidth: true
            visible: root.locCritical
            implicitHeight: locLabel.implicitHeight + 12
            radius: 4
            color: Qt.rgba(root.statusPalette.colorFor("critical").r,
                           root.statusPalette.colorFor("critical").g,
                           root.statusPalette.colorFor("critical").b, 0.2)
            border.width: 2
            border.color: root.statusPalette.colorFor("critical")
            QGCLabel {
                id: locLabel
                anchors.fill: parent
                anchors.margins: 6
                text: root.extremeFresh
                      ? qsTr("LOC 警告 · %1 · 风险 %2").arg(root.ftc.locStateText).arg(root.percentText(root.ftc.lossOfControlScore))
                      : ""
                color: root.statusPalette.colorFor("critical")
                wrapMode: Text.WordWrap
                font.bold: true
            }
        }

        Rectangle {
            Layout.fillWidth: true
            visible: root.recoveryVisible
            implicitHeight: recoveryLabel.implicitHeight + 10
            radius: 4
            color: Qt.rgba(root.statusPalette.colorFor(root.ftc ? root.ftc.recoverySeverity : "unavailable").r,
                           root.statusPalette.colorFor(root.ftc ? root.ftc.recoverySeverity : "unavailable").g,
                           root.statusPalette.colorFor(root.ftc ? root.ftc.recoverySeverity : "unavailable").b, 0.14)
            border.color: root.statusPalette.colorFor(root.ftc ? root.ftc.recoverySeverity : "unavailable")
            QGCLabel {
                id: recoveryLabel
                anchors.fill: parent
                anchors.margins: 5
                text: root.ftc
                      ? qsTr("Recovery · %1 · 进度 %2").arg(root.ftc.recoveryStateText).arg(root.percentText(root.ftc.recoveryProgress))
                      : ""
                color: qgcPal.text
                wrapMode: Text.WordWrap
                font.bold: true
            }
        }

        QGCLabel {
            Layout.fillWidth: true
            visible: root.controlFresh && root.ftc.controlMode >= 3
            text: root.ftc.controlMode === 4
                  ? qsTr("ACTIVE：执行器命令路径已明确接入。")
                  : qsTr("当前为恢复候选，不向执行器下发 FTC 命令。")
            color: root.statusPalette.colorFor(root.ftc.controlMode === 4 ? "critical" : "warning")
            wrapMode: Text.WordWrap
            font.pointSize: ScreenTools.smallFontPointSize
        }
    }

    Popup {
        id: detailsPopup
        parent: Overlay.overlay
        x: Math.round((parent.width - width) / 2)
        y: Math.round((parent.height - height) / 2)
        width: Math.min(parent.width * 0.82, 760)
        height: Math.min(parent.height * 0.86, 620)
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        background: Rectangle {
            color: qgcPal.window
            radius: 8
            border.color: root.statusPalette.colorFor(root.panelSeverity)
        }

        contentItem: ColumnLayout {
            spacing: 8

            RowLayout {
                Layout.fillWidth: true
                QGCLabel { Layout.fillWidth: true; text: qsTr("FTC 遥测详情"); font.bold: true; font.pointSize: ScreenTools.mediumFontPointSize; color: qgcPal.text }
                QGCLabel { text: root.ftc ? qsTr("协议 v%1").arg(root.ftc.protocolVersion) : qsTr("协议 N/A"); color: qgcPal.colorGrey }
                QGCButton { text: qsTr("关闭"); onClicked: detailsPopup.close() }
            }

            GridLayout {
                Layout.fillWidth: true
                columns: 4
                columnSpacing: 12
                rowSpacing: 4
                QGCLabel { text: qsTr("系统"); color: qgcPal.colorGrey }
                QGCLabel { text: root.systemFresh ? root.ftc.systemStateText : "N/A"; color: root.statusPalette.colorFor(root.systemFresh ? root.ftc.systemSeverity : "unavailable") }
                QGCLabel { text: qsTr("模式"); color: qgcPal.colorGrey }
                QGCLabel { text: root.controlFresh ? root.ftc.controlModeText : "N/A"; color: root.statusPalette.colorFor(root.controlFresh ? root.ftc.controlModeSeverity : "unavailable") }
                QGCLabel { text: qsTr("控制裕度"); color: qgcPal.colorGrey }
                QGCLabel { text: root.controlFresh ? root.ftc.authorityStateText : "N/A"; color: root.statusPalette.colorFor(root.controlFresh ? root.ftc.authoritySeverity : "unavailable") }
                QGCLabel { text: qsTr("恢复状态"); color: qgcPal.colorGrey }
                QGCLabel { text: root.controlFresh || root.extremeFresh ? root.ftc.recoveryStateText : "N/A"; color: root.statusPalette.colorFor(root.controlFresh || root.extremeFresh ? root.ftc.recoverySeverity : "unavailable") }
                QGCLabel { text: qsTr("滚转 / 俯仰"); color: qgcPal.colorGrey }
                QGCLabel { text: root.controlFresh ? root.percentText(root.ftc.rollAuthority) + " / " + root.percentText(root.ftc.pitchAuthority) : "N/A"; color: qgcPal.text }
                QGCLabel { text: qsTr("偏航 / 推力"); color: qgcPal.colorGrey }
                QGCLabel { text: root.controlFresh ? root.percentText(root.ftc.yawAuthority) + " / " + root.percentText(root.ftc.thrustAuthority) : "N/A"; color: qgcPal.text }
                QGCLabel { text: qsTr("撞击 / 失控分数"); color: qgcPal.colorGrey }
                QGCLabel { text: root.extremeFresh ? root.ftc.impactTypeText + " " + root.percentText(root.ftc.impactScore) + " / " + root.percentText(root.ftc.lossOfControlScore) : "N/A"; color: qgcPal.text }
                QGCLabel { text: qsTr("模型质量"); color: qgcPal.colorGrey }
                QGCLabel { text: root.fresh && !root.ftc.motorStale ? root.percentText(root.ftc.modelQuality) : "N/A"; color: qgcPal.text }
            }

            QGCLabel { text: qsTr("电机观测（健康值不是剩余寿命）"); color: qgcPal.text; font.bold: true }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: qgcPal.windowShade
                radius: 5

                ListView {
                    anchors.fill: parent
                    anchors.margins: 6
                    clip: true
                    spacing: 4
                    model: root.ftc ? root.ftc.motors : 0
                    delegate: Rectangle {
                        width: ListView.view.width
                        height: 48
                        radius: 4
                        color: qgcPal.window
                        border.color: root.statusPalette.colorFor(severity)
                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 5
                            QGCLabel { Layout.preferredWidth: 42; text: "M" + (motorIndex + 1); font.bold: true; color: qgcPal.text }
                            QGCLabel { Layout.fillWidth: true; text: available ? qsTr("健康 %1").arg(root.percentText(health)) : qsTr("健康 N/A"); color: qgcPal.text }
                            QGCLabel { Layout.fillWidth: true; text: available ? qsTr("效能 %1").arg(root.percentText(effectiveness)) : qsTr("效能 N/A"); color: qgcPal.text }
                            QGCLabel { Layout.fillWidth: true; text: available ? qsTr("故障概率 %1").arg(root.percentText(faultProbability)) : qsTr("故障概率 N/A"); color: qgcPal.text }
                            QGCLabel { Layout.preferredWidth: 135; text: dataStateText; color: root.statusPalette.colorFor(severity); elide: Text.ElideRight }
                        }
                    }
                }
            }

            QGCLabel {
                Layout.fillWidth: true
                text: root.ftc ? qsTr("估计：%1 · 基线 %2 · 年龄 %3 s").arg(root.ftc.modelStateText)
                      .arg(root.ftc.baselineLearned ? qsTr("已学习") : qsTr("未完成"))
                      .arg(root.ftc.estimateAge >= 0 ? Number(root.ftc.estimateAge).toFixed(2) : "N/A") : "N/A"
                color: qgcPal.text
                wrapMode: Text.WordWrap
            }
            QGCLabel {
                Layout.fillWidth: true
                text: root.ftc && root.ftc.diagnosticsAvailable && !root.ftc.diagnosticsStale
                      ? qsTr("诊断：模型残差 %1 · 分配残差 %2 · 振动 %3 · 原因掩码 0x%4")
                        .arg(Number(root.ftc.modelResidual).toFixed(3))
                        .arg(Number(root.ftc.allocationResidualNorm).toFixed(3))
                        .arg(Number(root.ftc.vibrationScore).toFixed(3))
                        .arg(Number(root.ftc.systemReasonMask).toString(16))
                        + "\n" + qsTr("条件数 %1 · 预测残差 %2 · 更新 %3 · 复位 %4")
                        .arg(Number(root.ftc.details.conditionNumber).toFixed(1))
                        .arg(Number(root.ftc.details.predictionResidual).toFixed(3))
                        .arg(root.ftc.details.updateCount).arg(root.ftc.details.resetCount)
                        + "\n" + qsTr("正向裕度 %1 · 负向裕度 %2 · 恢复权重 %3")
                        .arg(root.ftc.controlAvailable && !root.ftc.controlStale ? root.ftc.details.positiveAuthority : "N/A").arg(root.ftc.controlAvailable && !root.ftc.controlStale ? root.ftc.details.negativeAuthority : "N/A")
                        .arg(root.ftc.controlAvailable && !root.ftc.controlStale ? Number(root.ftc.details.arbitrationWeight).toFixed(2) : "N/A")
                      : qsTr("诊断数据：N/A")
                color: qgcPal.colorGrey
                wrapMode: Text.WordWrap
            }
        }
    }
}
