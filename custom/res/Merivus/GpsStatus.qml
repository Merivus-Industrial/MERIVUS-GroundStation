import QtQuick 2.12

QtObject {

// 统一工具栏与命令中心的 GNSS 文案。缺失字段保持“不显示”，不把未知遥测推断为 0。
function factNumber(gps, name) {
    if (!gps || !gps[name]) return NaN
    return Number(gps[name].rawValue)
}

function factString(gps, name) {
    if (!gps || !gps[name]) return ""
    var value = gps[name].rawValue
    return value === undefined || value === null ? "" : String(value)
}

function fixTextFromType(fixType) {
    return fixType === 2 ? qsTr("2D 定位")
         : fixType === 3 ? qsTr("3D 定位")
         : fixType === 4 ? qsTr("差分定位")
         : fixType === 5 ? qsTr("RTK 浮点")
         : fixType === 6 ? qsTr("RTK 固定")
         : fixType === 7 ? qsTr("静态固定")
         : fixType === 8 ? qsTr("外推定位")
         : qsTr("无有效定位")
}

function summary(vehicle) {
    if (!vehicle || !vehicle.gps) return qsTr("无数据")
    var gps = vehicle.gps
    var fixType = factNumber(gps, "lock")
    var fixText = fixTextFromType(fixType)
    var count = factNumber(gps, "count")
    return isNaN(count) || count < 0 ? fixText : qsTr("%1 · %2 星").arg(fixText).arg(count)
}

function appendNumber(lines, label, gps, name, decimals, unit) {
    var value = factNumber(gps, name)
    if (isNaN(value)) return
    lines.push(qsTr("%1：%2%3").arg(label).arg(value.toFixed(decimals)).arg(unit ? " " + unit : ""))
}

function receiverDetails(title, gps, includePosition) {
    var lines = [title]
    var fixType = factNumber(gps, "lock")
    var count = factNumber(gps, "count")
    lines.push(qsTr("定位类型：%1").arg(fixTextFromType(fixType)))
    if (!isNaN(count)) lines.push(qsTr("参与解算卫星：%1").arg(count))

    appendNumber(lines, qsTr("水平精度"), gps, "horizontalAccuracy", 3, "m")
    appendNumber(lines, qsTr("垂直精度"), gps, "verticalAccuracy", 3, "m")
    appendNumber(lines, "HDOP", gps, "hdop", 1, "")
    appendNumber(lines, "VDOP", gps, "vdop", 1, "")
    appendNumber(lines, qsTr("速度精度"), gps, "speedAccuracy", 3, "m/s")
    appendNumber(lines, qsTr("地速"), gps, "groundSpeed", 2, "m/s")
    appendNumber(lines, qsTr("航迹角"), gps, "courseOverGround", 1, "°")
    appendNumber(lines, qsTr("GNSS 航向"), gps, "yaw", 2, "°")
    appendNumber(lines, qsTr("航向精度"), gps, "headingAccuracy", 2, "°")
    appendNumber(lines, qsTr("海拔高度"), gps, "altitudeMSL", 2, "m")
    appendNumber(lines, qsTr("椭球高度"), gps, "altitudeEllipsoid", 2, "m")
    appendNumber(lines, qsTr("差分改正龄期"), gps, "differentialAge", 1, "s")

    var differentialCount = factNumber(gps, "differentialCount")
    if (!isNaN(differentialCount) && differentialCount > 0) {
        lines.push(qsTr("差分卫星通道：%1").arg(differentialCount))
    }

    if (includePosition) {
        appendNumber(lines, qsTr("纬度"), gps, "lat", 7, "°")
        appendNumber(lines, qsTr("经度"), gps, "lon", 7, "°")
        var mgrs = factString(gps, "mgrs")
        if (mgrs) lines.push(qsTr("MGRS：%1").arg(mgrs))
    }
    return lines
}

function receiverHasData(gps) {
    if (!gps) return false
    var fixType = factNumber(gps, "lock")
    var lat = factNumber(gps, "lat")
    var lon = factNumber(gps, "lon")
    return fixType > 0 || (!isNaN(lat) && !isNaN(lon) && (lat !== 0 || lon !== 0))
}

function rtkBaseDetails(rtk) {
    if (!rtk || !rtk.connected || !rtk.connected.rawValue) return []

    var lines = [qsTr("RTK 基站 / 改正源")]
    var active = !!rtk.active.rawValue
    var valid = !!rtk.valid.rawValue
    lines.push(qsTr("状态：%1").arg(active ? qsTr("Survey-in 进行中")
                                      : valid ? qsTr("基站位置有效，正在转发改正数据")
                                              : qsTr("已连接，等待有效基站位置")))
    appendNumber(lines, qsTr("观测时长"), rtk, "currentDuration", 0, "s")
    appendNumber(lines, qsTr("当前基站精度"), rtk, "currentAccuracy", 3, "m")
    appendNumber(lines, qsTr("基站卫星"), rtk, "numSatellites", 0, qsTr("星"))
    if (valid) {
        appendNumber(lines, qsTr("基站纬度"), rtk, "currentLatitude", 7, "°")
        appendNumber(lines, qsTr("基站经度"), rtk, "currentLongitude", 7, "°")
        appendNumber(lines, qsTr("基站高度"), rtk, "currentAltitude", 3, "m")
    }
    return lines
}

function details(vehicle, rtkBase) {
    if (!vehicle || !vehicle.gps) return qsTr("尚未连接无人机")
    var lines = receiverDetails(qsTr("主 GNSS"), vehicle.gps, true)
    if (receiverHasData(vehicle.gps2)) {
        lines.push("")
        lines = lines.concat(receiverDetails(qsTr("副 GNSS / 航向基线"), vehicle.gps2, false))
    }
    var baseLines = rtkBaseDetails(rtkBase)
    if (baseLines.length > 0) {
        lines.push("")
        lines = lines.concat(baseLines)
    }
    lines.push("")
    lines.push(qsTr("说明：RTK 改正龄期与差分通道仅在飞控通过 MAVLink 提供时显示；未显示不代表由地面站估算为 0。"))
    return lines.join("\n")
}
}
