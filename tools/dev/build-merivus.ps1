[CmdletBinding()]
param(
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Release",
    [int]$Jobs = [Math]::Max(1, [Environment]::ProcessorCount - 1),
    [switch]$Reconfigure,
    [switch]$Clean,
    [string]$QtRoot = "E:\Qt\5.15.2\msvc2019_64",
    [string]$QtCreatorRoot = "E:\Qt\Tools\QtCreator",
    [string]$VisualStudioRoot = "D:\Program Files\Microsoft Visual Studio\2022\Community"
)

$ErrorActionPreference = "Stop"

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
$buildDir = Join-Path $repoRoot "build\Desktop_Qt_5_15_2_MSVC2019_64bit-$Configuration"
$buildRoot = Join-Path $repoRoot "build"
$qmake = Join-Path $QtRoot "bin\qmake.exe"
$jom = Join-Path $QtCreatorRoot "bin\jom\jom.exe"
$vcvars = Join-Path $VisualStudioRoot "VC\Auxiliary\Build\vcvars64.bat"
$project = Join-Path $repoRoot "qgroundcontrol.pro"

foreach ($path in @($qmake, $jom, $vcvars, $project, $buildRoot)) {
    if (-not (Test-Path -LiteralPath $path)) {
        throw "Required path not found: $path"
    }
}

if ($Clean -and (Test-Path -LiteralPath $buildDir)) {
    $resolvedBuildRoot = (Resolve-Path -LiteralPath $buildRoot).Path
    $resolvedBuild = (Resolve-Path -LiteralPath $buildDir).Path
    if (-not $resolvedBuild.StartsWith($resolvedBuildRoot, [StringComparison]::OrdinalIgnoreCase)) {
        throw "Refusing to clean outside the repository build directory: $resolvedBuild"
    }
    Remove-Item -LiteralPath $resolvedBuild -Recurse -Force
}

New-Item -ItemType Directory -Path $buildDir -Force | Out-Null

$compilerProbe = & $env:ComSpec /d /s /c "call `"$vcvars`" >nul && where cl.exe && where link.exe"
if ($LASTEXITCODE -ne 0 -or -not $compilerProbe) {
    throw "MSVC cl.exe/link.exe were not found after vcvars64 initialization."
}
$cl = $compilerProbe | Where-Object { (Split-Path $_ -Leaf) -ieq "cl.exe" } | Select-Object -First 1
$link = $compilerProbe | Where-Object { (Split-Path $_ -Leaf) -ieq "link.exe" } | Select-Object -First 1
if (-not $cl -or -not $link) {
    throw "MSVC compiler/linker probe did not return cl.exe and link.exe."
}

$commands = @(
    "call `"$vcvars`""
    "cd /d `"$buildDir`""
)

if ($Reconfigure -or -not (Test-Path -LiteralPath (Join-Path $buildDir "Makefile"))) {
    $configName = $Configuration.ToLowerInvariant()
    $commands += "`"$qmake`" `"$project`" CONFIG+=$configName QMAKE_CC=`"$cl`" QMAKE_CXX=`"$cl`" QMAKE_LINK=`"$link`" QMAKE_LINK_C=`"$link`""
}

$commands += "`"$jom`" -j$Jobs"

$batchPath = Join-Path $buildDir "run-build.cmd"
$batchLines = @("@echo off") + $commands + @("exit /b %ERRORLEVEL%")
Set-Content -LiteralPath $batchPath -Value $batchLines -Encoding ASCII

Write-Host "Building $Configuration in $buildDir"
& $env:ComSpec /d /s /c "`"$batchPath`""
if ($LASTEXITCODE -ne 0) {
    throw "Build failed with exit code $LASTEXITCODE"
}

$binary = Join-Path $buildDir "staging\MERIVUS.exe"
if (-not (Test-Path -LiteralPath $binary)) {
    throw "Build command succeeded but MERIVUS.exe was not found at: $binary"
}

Write-Host "Build succeeded: $binary"
