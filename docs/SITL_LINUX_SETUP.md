# MERIVUS Linux SITL Preparation

This source snapshot is still based on Qt 5.15.2 and qmake. Keep the Linux
validation environment on Qt 5.15.x unless the project baseline is deliberately
upgraded.

## Before copying from Windows

1. Confirm the working tree state:

   ```powershell
   git status --short
   git ls-files | Measure-Object
   ```

2. If `git ls-files` reports `0`, create a local baseline commit before moving
   machines. The current repository has a `.git` directory, but the source tree
   may still be untracked.

   ```powershell
   git add .
   git commit -m "Baseline MERIVUS QGC source snapshot"
   ```

3. Prefer a Git bundle or a normal remote over ad hoc folder copy:

   ```powershell
   git bundle create merivus-baseline.bundle --all
   ```

   On Linux:

   ```bash
   git clone merivus-baseline.bundle MERIVUS
   cd MERIVUS
   ```

4. If you must copy a folder, exclude generated artifacts such as `build/`,
   `.qmake.stash`, Qt Creator `.user` files, and simulator logs. The repository
   `.gitignore` covers these paths; copying them wastes time and can confuse the
   Linux qmake cache.

## Linux host prerequisites

Install the equivalent of the following packages on Ubuntu/Debian-like systems:

```bash
sudo apt update
sudo apt install build-essential git cmake ninja-build pkg-config python3 \
    libsdl2-dev libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev
```

Install Qt 5.15.x with qmake. The exact package source depends on the target
Linux distribution; verify with:

```bash
qmake -query QT_VERSION
qmake -query QMAKE_SPEC
```

Expected baseline:

```text
QT_VERSION: 5.15.x
QMAKE_SPEC: linux-g++ or linux-clang
```

## Repository check on Linux

Run:

```bash
chmod +x tools/dev/check-linux-sitl-environment.sh
./tools/dev/check-linux-sitl-environment.sh
```

The script checks core build tools, qmake, SDL2/GStreamer development packages,
and whether the Git work tree has tracked files.

## Shadow build on Linux

Use a fresh Linux shadow build. Do not reuse the Windows `build/` directory.

```bash
mkdir -p build/linux-sitl-release
cd build/linux-sitl-release
qmake ../../qgroundcontrol.pro CONFIG+=release
make -j"$(nproc)"
```

If project files, resources, or Qt paths change, delete only this Linux shadow
build directory and rerun qmake.

## SITL validation boundary

Use Mock Link, PX4 SITL, or ArduPilot SITL. Do not run automated command-path
verification against real aircraft.

Recommended verification order:

1. Launch MERIVUS without a vehicle and confirm Fly View loads.
2. Connect one SITL vehicle and verify telemetry, guided confirmation, and map
   target display.
3. Connect multiple SITL vehicles with distinct system IDs and verify selection
   visibility before any command test.
4. Test partial failures by stopping one simulator instance or blocking one
   link, then confirm per-vehicle state remains visible.

Keep command scope, selected count, and system IDs visible during every SITL
command-path test.
