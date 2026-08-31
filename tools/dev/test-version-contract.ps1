$ErrorActionPreference = 'Stop'

function ConvertTo-AndroidBuildVersion {
    param([Parameter(Mandatory)][string]$Describe)

    if ($Describe -match '^v(?<major>\d+)\.(?<minor>\d+)\.(?<patch>\d+)-dev\.(?<prerelease>\d+)(?:-(?<distance>\d+)-g[0-9a-f]+)?$') {
        $prerelease = [int]$Matches.prerelease
        $distance = if ($Matches.distance) { [int]$Matches.distance } else { 0 }
        if ($prerelease -gt 9) { throw "Prerelease version exceeds one digit: $prerelease" }
        if ($distance -gt 99) { throw "Commits since prerelease tag exceed two digits: $distance" }
        return '{0}.{1}.{2}.{3}{4:D2}' -f $Matches.major, $Matches.minor, $Matches.patch, $prerelease, $distance
    }

    if ($Describe -match '^v(?<major>\d+)\.(?<minor>\d+)\.(?<patch>\d+)(?:-(?<distance>\d+)-g[0-9a-f]+)?$') {
        $distance = if ($Matches.distance) { [int]$Matches.distance } else { 0 }
        return '{0}.{1}.{2}.{3}' -f $Matches.major, $Matches.minor, $Matches.patch, $distance
    }

    throw "Unsupported Git version description: $Describe"
}

$cases = @(
    @{ Describe = 'v0.1.0-dev.1'; Expected = '0.1.0.100' }
    @{ Describe = 'v0.1.0-dev.1-28-gabcdef0'; Expected = '0.1.0.128' }
    @{ Describe = 'v0.1.0-28-gabcdef0'; Expected = '0.1.0.28' }
)

foreach ($case in $cases) {
    $actual = ConvertTo-AndroidBuildVersion -Describe $case.Describe
    if ($actual -ne $case.Expected) {
        throw "Version contract failed for $($case.Describe): expected $($case.Expected), got $actual"
    }
}

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot '..\..')
$currentDescribe = (& git -C $repoRoot describe --always --tags).Trim()
$currentVersion = ConvertTo-AndroidBuildVersion -Describe $currentDescribe

Write-Output "Version contract passed: $currentDescribe -> $currentVersion"
