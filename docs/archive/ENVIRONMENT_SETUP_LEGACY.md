# Windows Development Environment

## Detected baseline

- Source: `E:\MERIVUS`
- Qt: `E:\Qt\5.15.2\msvc2019_64`
- qmake: `E:\Qt\5.15.2\msvc2019_64\bin\qmake.exe`
- jom: `E:\Qt\Tools\QtCreator\bin\jom\jom.exe`
- Compiler environment: Visual Studio 2022 Community x64
- Build system: qmake shadow build
- Existing release build:
  `build\Desktop_Qt_5_15_2_MSVC2019_64bit-Release`

The Qt kit is named MSVC2019 because that is the Qt binary ABI. The installed
MSVC 2022 toolset is binary-compatible with that kit and is already represented
in the existing `.qmake.stash`.

## First check

```powershell
Set-ExecutionPolicy -Scope Process Bypass
.\tools\dev\check-windows-environment.ps1
```

The scripts use explicit paths, so qmake, jom, and MSVC do not need to be added
globally to `PATH`.

## Build

```powershell
.\tools\dev\build-windows.ps1 -Configuration Release
```

For a Debug build:

```powershell
.\tools\dev\build-windows.ps1 -Configuration Debug
```

Force qmake regeneration after project/resource changes:

```powershell
.\tools\dev\build-windows.ps1 -Configuration Release -Reconfigure
```

## Codex

Open this directory as the workspace. The repository contains:

- `AGENTS.md` for durable project rules.
- `.codex/config.toml` for workspace-write/on-request behavior.
- `.agents/skills/` for project workflows.

Start a new Codex thread after adding or changing Skills so metadata is
rediscovered.

## Git provenance

This source snapshot arrived without `.git` history. A local Git repository can
track MERIVUS changes, but it does not recreate QGroundControl upstream history.
Before configuring an upstream remote, identify the exact QGC release/commit
that produced this snapshot.

## Linux SITL transfer

Before moving this workspace to a Linux simulator host, read
`docs/SITL_LINUX_SETUP.md` and run the Linux-side check:

```bash
chmod +x tools/dev/check-linux-sitl-environment.sh
./tools/dev/check-linux-sitl-environment.sh
```

Create a real baseline commit or Git bundle before copying. A `.git` directory
alone is not enough if `git ls-files` is empty or the source tree is still
untracked.
