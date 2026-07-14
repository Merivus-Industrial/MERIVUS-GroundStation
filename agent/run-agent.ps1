param(
    [int]$Port = 8765
)

$ErrorActionPreference = "Stop"

if (-not (Get-Command python -ErrorAction SilentlyContinue)) {
    Write-Error "Python was not found in PATH."
}

$AgentRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$VenvPython = Join-Path $AgentRoot ".venv\Scripts\python.exe"

if (Test-Path $VenvPython) {
    $Python = $VenvPython
} else {
    Write-Host "agent/.venv was not found; using Python from PATH."
    $Python = "python"
}

$env:MERIVUS_AGENT_HOST = "127.0.0.1"
if (-not $env:MERIVUS_AGENT_PORT) {
    $env:MERIVUS_AGENT_PORT = "$Port"
}

Push-Location $AgentRoot
try {
    & $Python -m uvicorn app.main:app --host 127.0.0.1 --port $env:MERIVUS_AGENT_PORT
} finally {
    Pop-Location
}
