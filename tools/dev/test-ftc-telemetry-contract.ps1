$ErrorActionPreference = "Stop"

$projectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
$firmwareRoot = Join-Path (Split-Path $projectRoot -Parent) "FirmwarePX4"
$coreSchema = Join-Path $projectRoot "schemas\mavlink\merivus_ftc.xml"
$backendHeader = Join-Path $projectRoot "src\Vehicle\VehicleFtcStatusFactGroup.h"
$backendSource = Join-Path $projectRoot "src\Vehicle\VehicleFtcStatusFactGroup.cc"
$panel = Join-Path $projectRoot "custom\res\Merivus\FtcStatusPanel.qml"
$overlay = Join-Path $projectRoot "custom\res\Merivus\CommandCenterOverlay.qml"
$qmakeProject = Join-Path $projectRoot "qgroundcontrol.pro"
$vehicleCMake = Join-Path $projectRoot "src\Vehicle\CMakeLists.txt"
$resource = Join-Path $projectRoot "custom\qgroundcontrol.qrc"
$flightDisplayModule = Join-Path $projectRoot "custom\res\Merivus\qmldir"
$controlsModule = Join-Path $projectRoot "src\QmlControls\QGroundControl\Controls\qmldir"

function Assert-Contains([string]$Path, [string]$Pattern, [string]$Message) {
    if (-not (Select-String -LiteralPath $Path -Pattern $Pattern -Quiet)) {
        throw $Message
    }
}

$xml = [xml](Get-Content -LiteralPath $coreSchema -Raw)
$expectedMessages = @{
    MERIVUS_FTC_MOTOR_STATUS = 60000
    MERIVUS_FTC_CONTROL_STATUS = 60001
    MERIVUS_FTC_EXTREME_STATUS = 60002
    MERIVUS_FTC_DIAGNOSTICS = 60003
}

foreach ($message in $xml.mavlink.messages.message) {
    if (-not $expectedMessages.ContainsKey([string]$message.name) -or
        [int]$message.id -ne $expectedMessages[[string]$message.name]) {
        throw "MAVLink 消息 ID 不符合契约：$($message.name)=$($message.id)"
    }

    if (-not ($message.field | Where-Object { $_.name -eq "protocol_version" })) {
        throw "$($message.name) 缺少 protocol_version"
    }
}

if ($xml.mavlink.messages.message.Count -ne $expectedMessages.Count) {
    throw "FTC 消息数量不符合契约"
}

$generatedExpectations = @{
    "mavlink_msg_merivus_ftc_motor_status.h" = @("LEN 153", "MIN_LEN 78", "CRC 29")
    "mavlink_msg_merivus_ftc_control_status.h" = @("LEN 78", "MIN_LEN 24", "CRC 153")
    "mavlink_msg_merivus_ftc_extreme_status.h" = @("LEN 38", "CRC 136")
    "mavlink_msg_merivus_ftc_diagnostics.h" = @("LEN 117", "MIN_LEN 65", "CRC 5")
}

foreach ($entry in $generatedExpectations.GetEnumerator()) {
    $header = Join-Path $projectRoot "libs\mavlink\include\mavlink\v2.0\merivus_ftc\$($entry.Key)"
    foreach ($expectation in $entry.Value) {
        Assert-Contains $header ([regex]::Escape($expectation)) "$($entry.Key) 缺少 $expectation"
    }
}

foreach ($property in @(
    "enabled", "available", "lastUpdate", "stale", "systemState", "motorCount",
    "modelQuality", "rollAuthority", "pitchAuthority", "yawAuthority", "thrustAuthority",
    "minimumAttitudeAuthority", "actuatorHeadroom", "impactType", "locState", "recoveryState"
)) {
    Assert-Contains $backendHeader ("Q_PROPERTY\([^\r\n]*\b" + $property + "\b") "后端缺少属性 $property"
}

foreach ($stateText in @(
    "关闭", "监测", "检测到强扰动", "角速度抑制", "推力方向恢复", "姿态恢复",
    "高度稳定", "恢复正常控制", "紧急降落", "已中止", "恢复失败"
)) {
    Assert-Contains $backendSource ([regex]::Escape($stateText)) "恢复状态缺少中文映射：$stateText"
}

Assert-Contains $panel "vehicle\.ftcStatus" "FTC 面板没有使用 Vehicle 类型化后端"
Assert-Contains $overlay "ftcStatus\.motors\.get" "电机卡片没有接入 FTC 电机模型"
Assert-Contains $qmakeProject "src/Vehicle/VehicleFtcStatusFactGroup\.cc" "qmake 没有编译 FTC 后端源文件"
Assert-Contains $qmakeProject "src/Vehicle/VehicleFtcStatusFactGroup\.h" "qmake 没有把 FTC 后端头文件交给 MOC"
Assert-Contains $vehicleCMake "VehicleFtcStatusFactGroup\.cc" "CMake 没有编译 FTC 后端源文件"
Assert-Contains $vehicleCMake "VehicleFtcStatusFactGroup\.h" "CMake 没有把 FTC 后端头文件交给 AUTOMOC"
Assert-Contains $resource "QGroundControl/FlightDisplay/FtcStatusPanel\.qml" "FTC 面板没有进入 custom qrc"
Assert-Contains $resource "QGroundControl/FlightDisplay/FtcStatusPalette\.qml" "FTC 调色板没有进入 custom qrc"
Assert-Contains $flightDisplayModule "FtcStatusPanel\s+1\.0\s+FtcStatusPanel\.qml" "FTC 面板没有导出到 FlightDisplay 模块"
Assert-Contains $flightDisplayModule "FtcStatusPalette\s+1\.0\s+FtcStatusPalette\.qml" "FTC 调色板没有导出到 FlightDisplay 模块"
Assert-Contains $controlsModule "QGCButton\s+1\.0\s+QGCButton\.qml" "Controls 模块没有导出 QGCButton"
Assert-Contains $controlsModule "QGCLabel\s+1\.0\s+QGCLabel\.qml" "Controls 模块没有导出 QGCLabel"
Assert-Contains $panel "import QGroundControl\.Controls\s+1\.0" "FTC 面板缺少 QGroundControl.Controls import"

if (Select-String -LiteralPath $panel, $overlay -Pattern "mavlink_msg_|MAVLINK_MSG_ID_" -Quiet) {
    throw "QML 不得解析原始 MAVLink"
}

if (Test-Path $firmwareRoot) {
    $firmwareCore = Join-Path $firmwareRoot "src\modules\mavlink\message_definitions\v1.0\merivus_ftc.xml"
    if ((Get-FileHash $firmwareCore -Algorithm SHA256).Hash -ne (Get-FileHash $coreSchema -Algorithm SHA256).Hash) {
        throw "固件与地面站 FTC XML 不一致"
    }

    $controlStream = Join-Path $firmwareRoot "src\modules\mavlink\streams\MERIVUS_FTC_CONTROL_STATUS.hpp"
    Assert-Contains $controlStream 'if \(_system\.intervention_enabled\)' "ACTIVE 必须由实际仲裁反馈驱动"
    Assert-Contains $backendHeader 'ProtocolVersion = 2' "后端未切换至 FTC v2"
    Assert-Contains $backendSource '历史估计过期' "缺少估计年龄过期状态"
    Assert-Contains $backendSource '消息已过期' "缺少消息超时状态"
    Assert-Contains $backendSource '模型不可用' "缺少模型无效状态"
    Assert-Contains $panel 'dataStateText' "电机行没有使用类型化数据状态"
}

Write-Host "FTC 遥测字段、后端属性、QML 消费与安全语义检查通过。"
