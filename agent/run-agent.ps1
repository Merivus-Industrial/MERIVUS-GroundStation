param(
    [int]$Port = 8765
)

$ErrorActionPreference = "Stop"

$AgentRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$VenvPython = Join-Path $AgentRoot ".venv\Scripts\python.exe"

if (Test-Path $VenvPython) {
    $Python = $VenvPython
} else {
    $PathPython = Get-Command python -ErrorAction SilentlyContinue
    if (-not $PathPython) {
        Write-Error "Python was not found. Create agent/.venv or install Python and add it to PATH."
    }
    Write-Host "agent/.venv was not found; using Python from PATH."
    $Python = $PathPython.Source
}

& $Python --version *> $null
if ($LASTEXITCODE -ne 0) {
    Write-Error "The selected Python executable failed to run: $Python"
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
