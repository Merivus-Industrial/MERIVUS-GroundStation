$ErrorActionPreference = "Stop"

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path

$requiredChecks = @(
    @{
        Path = "custom/res/Merivus/GuidedActionsController.qml"
        Text = "_swarm.sendStartCommand(actionData ? actionData : [])"
        Description = "编队按钮调用冻结目标的 SITL 启动入口"
    },
    @{
        Path = "custom/src/Swarm/SwarmController.cc"
        Text = "selectedVehicleIds.count() != 6"
        Description = "编队严格校验六架目标"
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
        Text = "selectedSwarmIds = [vehicleId]"
        Description = "普通选机使用排他单选"
    },
    @{
        Path = "src/PlanView/PlanView.qml"
        Text = "visible:            _planMasterController.managerVehicle.isOfflineEditingVehicle"
        Description = "连接车辆切换时禁止保留上一架方案"
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
Write-Host "SITL 编队与任务隔离静态回归通过。"
