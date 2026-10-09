param(
    [string]$MavlinkSource = "",
    [string]$Python = "python",
    [switch]$Check
)

$ErrorActionPreference = "Stop"
$expectedMavlinkCommit = "18955a04c7c7467e00ea42b704addb4a9c12b53a"
$projectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path

if (-not $MavlinkSource) {
    $MavlinkSource = Join-Path (Split-Path $projectRoot -Parent) "FirmwarePX4\src\modules\mavlink\mavlink"
}

$MavlinkSource = (Resolve-Path $MavlinkSource).Path
$actualCommit = (& git -C $MavlinkSource rev-parse HEAD).Trim()

if ($LASTEXITCODE -ne 0 -or $actualCommit -ne $expectedMavlinkCommit) {
    throw "pymavlink 源码版本不匹配。期望 $expectedMavlinkCommit，实际 $actualCommit"
}

$generator = Join-Path $MavlinkSource "pymavlink\tools\mavgen.py"
$upstreamDefinitions = Join-Path $projectRoot "libs\mavlink\include\mavlink\v2.0\message_definitions"
$schemaRoot = Join-Path $projectRoot "schemas\mavlink"
$headerRoot = Join-Path $projectRoot "libs\mavlink\include\mavlink\v2.0"
$temporaryRoot = Join-Path ([IO.Path]::GetTempPath()) ("merivus-mavlink-" + [guid]::NewGuid().ToString("N"))
$definitionStage = Join-Path $temporaryRoot "definitions"
$generatedRoot = Join-Path $temporaryRoot "generated"

try {
    $previousPythonHashSeed = $env:PYTHONHASHSEED
    $env:PYTHONHASHSEED = "0"
    New-Item -ItemType Directory -Path $definitionStage -Force | Out-Null
    Copy-Item (Join-Path $upstreamDefinitions "*.xml") -Destination $definitionStage -Force
    Copy-Item (Join-Path $schemaRoot "merivus.xml") -Destination $definitionStage -Force
    Copy-Item (Join-Path $schemaRoot "merivus_ftc.xml") -Destination $definitionStage -Force

    & $Python $generator --lang C --wire-protocol 2.0 --strict-units `
        --output $generatedRoot (Join-Path $definitionStage "merivus.xml")

    if ($LASTEXITCODE -ne 0) {
        throw "MAVLink 方言生成失败，退出码 $LASTEXITCODE"
    }

    foreach ($generatedFile in Get-ChildItem (Join-Path $generatedRoot "merivus"), (Join-Path $generatedRoot "merivus_ftc") -File) {
        $generatedText = [IO.File]::ReadAllText($generatedFile.FullName)
        $normalizedText = [regex]::Replace(
            $generatedText,
            "[ `t]+(?=`r?$)",
            "",
            [Text.RegularExpressions.RegexOptions]::Multiline)

        # mavgen writes the current day into version.h even though it is not part of
        # the wire contract. Preserve the tracked value so repeated generation is
        # deterministic and -Check only reports protocol-relevant differences.
        if ($generatedFile.Name -eq "version.h") {
            $trackedVersion = Join-Path $headerRoot "$($generatedFile.Directory.Name)\version.h"

            if (Test-Path $trackedVersion) {
                $trackedBuildDate = [regex]::Match(
                    [IO.File]::ReadAllText($trackedVersion),
                    '#define MAVLINK_BUILD_DATE "[^"]+"').Value

                if ($trackedBuildDate) {
                    $normalizedText = [regex]::Replace(
                        $normalizedText,
                        '#define MAVLINK_BUILD_DATE "[^"]+"',
                        $trackedBuildDate)
                }
            }
        }

        if ($normalizedText -cne $generatedText) {
            [IO.File]::WriteAllText($generatedFile.FullName, $normalizedText, [Text.UTF8Encoding]::new($false))
        }
    }

    foreach ($dialect in @("merivus", "merivus_ftc")) {
        $generatedDialect = Join-Path $generatedRoot $dialect
        $trackedDialect = Join-Path $headerRoot $dialect

        if ($Check) {
            $generatedFiles = Get-ChildItem $generatedDialect -File | Sort-Object Name
            $trackedFiles = Get-ChildItem $trackedDialect -File | Sort-Object Name

            if (($generatedFiles.Name -join "`n") -ne ($trackedFiles.Name -join "`n")) {
                throw "$dialect 生成文件列表与仓库不一致"
            }

            foreach ($generatedFile in $generatedFiles) {
                $trackedFile = Join-Path $trackedDialect $generatedFile.Name
                $generatedText = ([IO.File]::ReadAllText($generatedFile.FullName) -replace "`r`n", "`n")
                $trackedText = ([IO.File]::ReadAllText($trackedFile) -replace "`r`n", "`n")

                if ($generatedText -cne $trackedText) {
                    throw "$dialect/$($generatedFile.Name) 与仓库版本不一致"
                }
            }
        } else {
            New-Item -ItemType Directory -Path $trackedDialect -Force | Out-Null
            Copy-Item (Join-Path $generatedDialect "*") -Destination $trackedDialect -Force
        }
    }

    if ($Check) {
        Write-Host "MERIVUS MAVLink 方言与生成头文件一致。"
    } else {
        Write-Host "MERIVUS MAVLink 头文件已更新。"
    }
} finally {
    $env:PYTHONHASHSEED = $previousPythonHashSeed
    $temporaryPath = [IO.Path]::GetFullPath($temporaryRoot)
    $systemTemporaryPath = [IO.Path]::GetFullPath([IO.Path]::GetTempPath())

    if ($temporaryPath.StartsWith($systemTemporaryPath) -and (Test-Path $temporaryPath)) {
        Remove-Item -LiteralPath $temporaryPath -Recurse -Force
    }
}
