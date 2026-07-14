[CmdletBinding()]
param(
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Release",
    [string]$Python = "python"
)

$ErrorActionPreference = "Stop"

function Resolve-RequiredPath {
    param([string]$Path)
    if (-not (Test-Path -LiteralPath $Path)) {
        throw "Required path not found: $Path"
    }
    return (Resolve-Path -LiteralPath $Path).Path
}

function Assert-PathUnder {
    param(
        [string]$Path,
        [string]$Parent
    )
    $resolvedPath = (Resolve-Path -LiteralPath $Path).Path
    $resolvedParent = (Resolve-Path -LiteralPath $Parent).Path
    if (-not $resolvedPath.StartsWith($resolvedParent, [StringComparison]::OrdinalIgnoreCase)) {
        throw "Refusing to modify path outside expected root: $resolvedPath"
    }
}

$repoRoot = Resolve-RequiredPath (Join-Path $PSScriptRoot "..\..")
$agentRoot = Resolve-RequiredPath (Join-Path $repoRoot "agent")
$specPath = Resolve-RequiredPath (Join-Path $agentRoot "merivus-agent.spec")
$buildDir = Join-Path $agentRoot "build"
$distDir = Join-Path $agentRoot "dist"
$agentDistDir = Join-Path $distDir "merivus-agent"
$agentExe = Join-Path $agentDistDir "merivus-agent.exe"
$qgcBuildDir = Join-Path $repoRoot "build\Desktop_Qt_5_15_2_MSVC2019_64bit-$Configuration"
$stagingRoot = Join-Path $qgcBuildDir "staging"
$stagingAgentDir = Join-Path $stagingRoot "agent"

$pythonVersion = & $Python --version 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "Python was not found or failed to run: $Python"
}
Write-Host "Using $pythonVersion"

$pyInstallerVersion = & $Python -m PyInstaller --version 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "PyInstaller is not installed for '$Python'. Install it with: $Python -m pip install pyinstaller"
}
Write-Host "Using PyInstaller $pyInstallerVersion"

$stagingRoot = Resolve-RequiredPath $stagingRoot

foreach ($path in @($buildDir, $distDir)) {
    if (Test-Path -LiteralPath $path) {
        Assert-PathUnder -Path $path -Parent $agentRoot
        Remove-Item -LiteralPath $path -Recurse -Force
    }
}

if (Test-Path -LiteralPath $stagingAgentDir) {
    Assert-PathUnder -Path $stagingAgentDir -Parent $stagingRoot
    Remove-Item -LiteralPath $stagingAgentDir -Recurse -Force
}

Push-Location $agentRoot
try {
    & $Python -m PyInstaller --noconfirm --clean $specPath
    if ($LASTEXITCODE -ne 0) {
        throw "PyInstaller failed with exit code $LASTEXITCODE"
    }
} finally {
    Pop-Location
}

if (-not (Test-Path -LiteralPath $agentExe)) {
    throw "PyInstaller finished but merivus-agent.exe was not found at: $agentExe"
}

New-Item -ItemType Directory -Path $stagingAgentDir -Force | Out-Null
Copy-Item -Path (Join-Path $agentDistDir "*") -Destination $stagingAgentDir -Recurse -Force

$stagingAgentExe = Join-Path $stagingAgentDir "merivus-agent.exe"
if (-not (Test-Path -LiteralPath $stagingAgentExe)) {
    throw "Staging copy failed; missing: $stagingAgentExe"
}

$forbiddenPatterns = @("*.env", ".env", "*.pdf")
foreach ($pattern in $forbiddenPatterns) {
    $matches = Get-ChildItem -LiteralPath $stagingAgentDir -Recurse -Force -Filter $pattern -ErrorAction SilentlyContinue
    if ($matches) {
        throw "Forbidden file copied to staging agent directory: $($matches[0].FullName)"
    }
}

$venvMatches = Get-ChildItem -LiteralPath $stagingAgentDir -Recurse -Force -Directory -ErrorAction SilentlyContinue |
    Where-Object { $_.Name -in @(".venv", "venv", "env") }
if ($venvMatches) {
    throw "Virtual environment directory copied to staging agent directory: $($venvMatches[0].FullName)"
}

Write-Host "Agent packaged: $agentExe"
Write-Host "Agent staged:   $stagingAgentExe"
