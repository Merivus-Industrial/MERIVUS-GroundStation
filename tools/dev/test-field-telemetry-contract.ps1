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

$plugin = Read-RepositoryFile 'src/FirmwarePlugin/PX4/PX4FirmwarePlugin.cc'
Assert-Contains $plugin 'MAVLINK_MSG_ID_ESC_STATUS' 'PX4 must request ESC_STATUS measurements'
Assert-Contains $plugin 'MAVLINK_MSG_ID_ESC_INFO' 'PX4 must request ESC_INFO health data'
Assert-Contains $plugin 'escStatusIntervalPending' 'ESC interval requests must be serialized by command id'

$escFacts = Read-RepositoryFile 'src/Vehicle/VehicleEscStatusFactGroup.cc'
Assert-Contains $escFacts 'message.msgid == MAVLINK_MSG_ID_ESC_INFO' 'ESC_INFO must be decoded'
Assert-Contains $escFacts 'message.msgid != MAVLINK_MSG_ID_ESC_STATUS' 'ESC_STATUS must remain the measurement source'
Assert-Contains $escFacts 'if (content.index != 0)' 'the four-motor FactGroup must reject later ESC blocks'

$gpsPrimary = Read-RepositoryFile 'src/Vehicle/VehicleGPSFactGroup.cc'
$gpsSecondary = Read-RepositoryFile 'src/Vehicle/VehicleGPS2FactGroup.cc'
foreach ($field in @(
    'altitudeMSL',
    'altitudeEllipsoid',
    'groundSpeed',
    'horizontalAccuracy',
    'verticalAccuracy',
    'speedAccuracy',
    'headingAccuracy',
    'yaw'
)) {
    Assert-Contains $gpsPrimary "$field()->setRawValue" "primary GNSS must populate $field"
    Assert-Contains $gpsSecondary "$field()->setRawValue" "secondary GNSS must populate $field"
}
Assert-Contains $gpsSecondary 'differentialAge()->setRawValue' 'secondary GNSS must expose correction age'
Assert-Contains $gpsSecondary 'differentialCount()->setRawValue' 'secondary GNSS must expose differential channel count'

$gpsMetadata = (Read-RepositoryFile 'src/Vehicle/GPSFact.json') | ConvertFrom-Json
$gpsNames = @($gpsMetadata.'QGC.MetaData.Facts'.name)
foreach ($name in @(
    'altitudeMSL',
    'altitudeEllipsoid',
    'groundSpeed',
    'horizontalAccuracy',
    'verticalAccuracy',
    'speedAccuracy',
    'headingAccuracy',
    'yaw',
    'differentialAge',
    'differentialCount'
)) {
    if ($name -notin $gpsNames) {
        throw "Contract failed: GPS metadata is missing $name"
    }
}

$resource = Read-RepositoryFile 'custom/qgroundcontrol.qrc'
$qmldir = Read-RepositoryFile 'src/QmlControls/QGroundControl/Controls/qmldir'
$toolbar = Read-RepositoryFile 'custom/res/Merivus/MainToolBar.qml'
$commandCenter = Read-RepositoryFile 'custom/res/Merivus/CommandCenterOverlay.qml'
Assert-Contains $resource 'QGroundControl/Controls/GpsStatus.qml' 'GpsStatus must be embedded in the QML resource'
Assert-Contains $qmldir 'GpsStatus' 'GpsStatus must be exported by the Controls module'
Assert-Contains $toolbar 'GpsStatus { id: gpsStatus }' 'the toolbar must use the shared GNSS presenter'
Assert-Contains $commandCenter 'GpsStatus { id: gpsStatus }' 'the command center must use the shared GNSS presenter'

Write-Host 'Field telemetry contract passed.'
