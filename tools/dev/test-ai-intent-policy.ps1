[CmdletBinding()]
param(
    [string]$QtRoot = "E:\Qt\5.15.2\msvc2019_64",
    [string]$VisualStudioRoot = "D:\Program Files\Microsoft Visual Studio\2022\Community"
)

$ErrorActionPreference = "Stop"

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
$buildDir = Join-Path $repoRoot "build\ai-intent-policy-tests"
$qmake = Join-Path $QtRoot "bin\qmake.exe"
$vcvars = Join-Path $VisualStudioRoot "VC\Auxiliary\Build\vcvars64.bat"
$project = Join-Path $repoRoot "custom\tests\ai_intent_policy_tests.pro"

foreach ($requiredPath in @($qmake, $vcvars, $project)) {
    if (-not (Test-Path -LiteralPath $requiredPath)) {
        throw "Required path not found: $requiredPath"
    }
}

New-Item -ItemType Directory -Path $buildDir -Force | Out-Null

$commands = @(
    "call `"$vcvars`"",
    "cd /d `"$buildDir`"",
    "`"$qmake`" `"$project`"",
    "nmake /f Makefile.Release"
)

$batchPath = Join-Path $buildDir "run-ai-intent-policy-tests.cmd"
$batchLines = @("@echo off") + $commands + @("exit /b %ERRORLEVEL%")
Set-Content -LiteralPath $batchPath -Value $batchLines -Encoding ASCII

Write-Host "Building AI intent policy C++ tests in $buildDir"
& $env:ComSpec /d /s /c "`"$batchPath`""
if ($LASTEXITCODE -ne 0) {
    throw "AI intent policy test build failed with exit code $LASTEXITCODE"
}

$exeCandidates = @(
    (Join-Path $buildDir "release\ai_intent_policy_tests.exe"),
    (Join-Path $buildDir "debug\ai_intent_policy_tests.exe"),
    (Join-Path $buildDir "ai_intent_policy_tests.exe")
)
$testExe = $exeCandidates | Where-Object { Test-Path -LiteralPath $_ } | Select-Object -First 1
if (-not $testExe) {
    throw "AI intent policy test executable was not produced."
}

$env:Path = (Join-Path $QtRoot "bin") + ";" + $env:Path
& $testExe
if ($LASTEXITCODE -ne 0) {
    throw "AI intent policy tests failed with exit code $LASTEXITCODE"
}