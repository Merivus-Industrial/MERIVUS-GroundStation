Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$repositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path

function Read-RepositoryFile([string]$relativePath) {
    return [System.IO.File]::ReadAllText((Join-Path $repositoryRoot $relativePath))
}

function Assert-Contains([string]$content, [string]$expected, [string]$description) {
    if (-not $content.Contains($expected)) {
        throw "Contract failed: $description"
    }
}

function Assert-NotContains([string]$content, [string]$unexpected, [string]$description) {
    if ($content.Contains($unexpected)) {
        throw "Contract failed: $description"
    }
}

function Assert-Matches([string]$content, [string]$pattern, [string]$description) {
    if (-not [regex]::IsMatch($content, $pattern)) {
        throw "Contract failed: $description"
    }
}

$plugin = Read-RepositoryFile 'src/FirmwarePlugin/PX4/PX4FirmwarePlugin.cc'
$report = Read-RepositoryFile 'src/Vehicle/HealthAndArmingCheckReport.cc'
$vehicle = Read-RepositoryFile 'src/Vehicle/Vehicle.cc'
$swarm = Read-RepositoryFile 'custom/src/Swarm/SwarmController.cc'

Assert-Contains $plugin 'PX4TakeoffHealthCheckStage::RefreshBeforeTakeoff' 'takeoff must start with a preflight refresh'
Assert-Contains $plugin 'MAV_CMD_RUN_PREARM_CHECKS' 'the refresh must be requested from the flight controller'
Assert-Contains $plugin 'updateSequence() <= instanceData->takeoffHealthReportSequence' 'stale health reports must be rejected'
Assert-Contains $plugin 'PX4TakeoffHealthCheckStage::WaitingForTakeoffAcceptance' 'the takeoff command must have a distinct ACK stage'
Assert-Contains $plugin 'PX4TakeoffHealthCheckStage::RefreshBeforeArm' 'automatic arming must use a second health refresh'
Assert-Contains $plugin 'PX4TakeoffHealthCheckStage::RefreshAfterTakeoffRejection' 'takeoff rejection must refresh the failure reason'
Assert-Contains $plugin 'PX4TakeoffHealthCheckStage::RefreshAfterArmRejection' 'arming rejection must refresh the failure reason'

Assert-Contains $report 'if (healthResultsUpdated)' 'the sequence must distinguish reports from local recalculation'
Assert-Contains $report '++_updateSequence;' 'a flight-controller report must advance the sequence'
Assert-Matches $vehicle 'healthAndArmingChecksUpdated[\s\S]*?_healthAndArmingCheckReport\.update\(compid,[\s\S]*?\s+true\);' 'event updates must be marked as new reports'
Assert-Matches $vehicle 'flightModeChanged[\s\S]*?_healthAndArmingCheckReport\.update\(compid,[\s\S]*?\s+false\);' 'mode changes must not advance the report sequence'

$takeoffReadyMatch = [regex]::Match(
    $swarm,
    'bool SwarmController::_vehicleTakeoffReady[\s\S]*?\n}\r?\n\r?\nbool SwarmController::_vehicleLandReady')
if (-not $takeoffReadyMatch.Success) {
    throw 'Contract failed: unable to locate the swarm takeoff readiness function.'
}
Assert-NotContains $takeoffReadyMatch.Value 'HealthAndArmingCheckReport' 'the swarm entry point must not duplicate the PX4 health gate'
Assert-NotContains $takeoffReadyMatch.Value 'canTakeoff()' 'the swarm entry point must not use a potentially stale takeoff decision'

Write-Host 'PX4 takeoff health contract passed.'
