$ErrorActionPreference = "Stop"

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path

$requiredChecks = @(
    @{
        Path = "custom/src/Swarm/SwarmController.h"
        Text = "kFormationFeatureEnabled = true"
        Description = "六机编队功能默认启用"
    },
    @{
        Path = "custom/src/Swarm/SwarmController.h"
        Text = "kAutoStartMissionFeatureEnabled = true"
        Description = "AUTO_START_MISSION 默认启用"
    },
    @{
        Path = "custom/res/Merivus/GuidedActionsController.qml"
        Text = "_swarm.sendStartCommand(actionData ? actionData : [])"
        Description = "编队按钮调用冻结六机目标的启动入口"
    },
    @{
        Path = "custom/src/Swarm/SwarmController.cc"
        Text = "selectedVehicleIds.count() != 6"
        Description = "编队严格校验六架目标"
    },
    @{
        Path = "custom/src/Swarm/SwarmController.cc"
        Text = "_sendFormationCommand(MAV_CMD_USER_1"
        Description = "编队启动使用带 ACK 的版本化 MAV_CMD"
    },
    @{
        Path = "custom/src/Swarm/SwarmController.cc"
        Text = "_sendFormationCommand(MAV_CMD_USER_2"
        Description = "编队结束和失联保护下发停止 MAV_CMD"
    },
    @{
        Path = "custom/src/Swarm/SwarmController.cc"
        Text = "mavlink_msg_follow_target_encode_chan"
        Description = "主机位置使用 FOLLOW_TARGET 转发"
    },
    @{
        Path = "custom/src/Swarm/SwarmController.cc"
        Text = "QVariantMap SwarmController::executeLand"
        Description = "框选目标具备批量降落入口"
    },
    @{
        Path = "custom/src/Swarm/SwarmController.cc"
        Text = "QVariantMap SwarmController::executeRTL"
        Description = "框选目标具备批量标准返航入口"
    },
    @{
        Path = "custom/src/Swarm/SwarmController.cc"
        Text = "vehicle->id() == 1 || !_formationVehicleIds.contains(vehicle->id())"
        Description = "主机位置只转发给冻结从机"
    },
    @{
        Path = "custom/res/Merivus/FlyViewMap.qml"
        Text = "shiftFrozenVehicleIds = ids"
        Description = "Shift 首点冻结目标"
    },
    @{
        Path = "custom/res/Merivus/FlyViewMap.qml"
        Text = "guidedController.actionQueuedMission"
        Description = "Shift 队列使用统一滑动确认入口"
    },
    @{
        Path = "custom/res/Merivus/FlyViewMap.qml"
        Text = "selectedSwarmIds = [vehicleId]"
        Description = "普通选机使用排他单选"
    },
    @{
        Path = "src/PlanView/PlanView.qml"
        Text = "放弃未保存修改并加载当前无人机方案"
        Description = "连接车辆切换使用统一中文脏草稿确认"
    }
)

foreach ($check in $requiredChecks) {
    $fullPath = Join-Path $repoRoot $check.Path
    if (-not (Test-Path -LiteralPath $fullPath)) {
        throw "缺少文件：$($check.Path)"
    }
    $content = Get-Content -Raw -LiteralPath $fullPath
    if (-not $content.Contains($check.Text)) {
        throw "静态回归失败：$($check.Description)"
    }
    Write-Host "[通过] $($check.Description)"
}

$guidedControllerPath = Join-Path $repoRoot "custom/res/Merivus/GuidedActionsController.qml"
$guidedController = Get-Content -Raw -LiteralPath $guidedControllerPath
if ($guidedController.Contains("executeStartMissions")) {
    throw "静态回归失败：编队入口仍引用各机旧 Mission 启动逻辑"
}

Write-Host "[通过] 编队入口未引用 executeStartMissions"

if ($guidedController.Contains("confirmAction(actionContinueMission)")) {
    throw "静态回归失败：切换车辆仍可能自动弹出继续任务确认"
}
Write-Host "[通过] 继续任务只保留手动入口"

$swarmControllerPath = Join-Path $repoRoot "custom/src/Swarm/SwarmController.cc"
$swarmController = Get-Content -Raw -LiteralPath $swarmControllerPath
if ($swarmController.Contains("MERIVUS_DEV_ENABLE_SWARM")) {
    throw "静态回归失败：编队或临时任务仍依赖旧环境变量"
}
Write-Host "[通过] 编队与临时任务默认启用且不依赖旧环境变量"

if ($swarmController.Contains("mavlink_msg_gps_raw_int_pack_chan")) {
    throw "静态回归失败：仍在用 GPS_RAW_INT 字段伪装编队控制命令"
}
Write-Host "[通过] 编队控制不再伪装 GPS_RAW_INT"

$flyViewMapPath = Join-Path $repoRoot "custom/res/Merivus/FlyViewMap.qml"
$flyViewMap = Get-Content -Raw -LiteralPath $flyViewMapPath
if ($flyViewMap.Contains("temporaryMissionConfirmDialog") -or $flyViewMap.Contains("MessageDialog {")) {
    throw "静态回归失败：Shift 队列仍使用平台原生确认框"
}
Write-Host "[通过] Shift 队列不再使用平台原生确认框"

Write-Host "六机编队、多机动作与任务隔离静态回归通过。"
