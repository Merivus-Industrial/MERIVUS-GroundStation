[CmdletBinding()]
param(
    [string]$QtRoot = "E:\Qt\5.15.2\msvc2019_64",
    [string]$QtCreatorRoot = "E:\Qt\Tools\QtCreator",
    [string]$VisualStudioRoot = "D:\Program Files\Microsoft Visual Studio\2022\Community"
)

$ErrorActionPreference = "Stop"

$tools = [ordered]@{
    qmake = Join-Path $QtRoot "bin\qmake.exe"
    jom = Join-Path $QtCreatorRoot "bin\jom\jom.exe"
    vcvars64 = Join-Path $VisualStudioRoot "VC\Auxiliary\Build\vcvars64.bat"
    python = (Get-Command python -ErrorAction SilentlyContinue).Source
    git = (Get-Command git -ErrorAction SilentlyContinue).Source
}

$missing = @()
foreach ($entry in $tools.GetEnumerator()) {
    $present = $entry.Value -and (Test-Path -LiteralPath $entry.Value)
    if (-not $present) {
        $missing += $entry.Key
    }
    [pscustomobject]@{
        Tool = $entry.Key
        Present = $present
        Path = $entry.Value
    }
}

if ($missing.Count -gt 0) {
    throw "Missing required tools: $($missing -join ', ')"
}

& $tools.qmake -query QT_VERSION
& $tools.qmake -query QMAKE_SPEC
& $tools.python --version
& $tools.git --version

$clProbe = & $env:ComSpec /d /s /c "call `"$($tools.vcvars64)`" >nul && where cl"
if ($LASTEXITCODE -ne 0 -or -not $clProbe) {
    throw "MSVC compiler cl.exe was not found after vcvars64 initialization. Modify the VisualStudioRoot parameter or install the MSVC x64 C++ build tools."
}
Write-Host "MSVC cl: $($clProbe | Select-Object -First 1)"

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
$releaseBuild = Join-Path $repoRoot "build\Desktop_Qt_5_15_2_MSVC2019_64bit-Release"
Write-Host "Release build directory: $releaseBuild"
Write-Host "Environment check passed."
