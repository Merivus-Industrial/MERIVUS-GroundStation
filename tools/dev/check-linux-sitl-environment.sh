#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
missing=0

check_cmd() {
    local name="$1"
    local hint="$2"
    if command -v "$name" >/dev/null 2>&1; then
        printf '[OK]   %-16s %s\n' "$name" "$(command -v "$name")"
    else
        printf '[MISS] %-16s %s\n' "$name" "$hint"
        missing=1
    fi
}

check_pkg() {
    local name="$1"
    local hint="$2"
    if pkg-config --exists "$name" 2>/dev/null; then
        printf '[OK]   pkg %-12s %s\n' "$name" "$(pkg-config --modversion "$name")"
    else
        printf '[MISS] pkg %-12s %s\n' "$name" "$hint"
        missing=1
    fi
}

printf 'MERIVUS Linux/SITL environment check\n'
printf 'Repository: %s\n\n' "$repo_root"

check_cmd git 'install git before cloning or bundle import'
check_cmd qmake 'install Qt 5.15.x qmake and ensure it is on PATH'
check_cmd make 'install build-essential or equivalent'
check_cmd gcc 'install build-essential or equivalent'
check_cmd g++ 'install build-essential or equivalent'
check_cmd python3 'install python3 for build and simulator tooling'
check_cmd cmake 'install cmake for PX4/ArduPilot SITL dependencies'
check_cmd ninja 'recommended for simulator builds'
check_cmd pkg-config 'install pkg-config for Linux library discovery'

if command -v qmake >/dev/null 2>&1; then
    qt_version="$(qmake -query QT_VERSION 2>/dev/null || true)"
    qt_spec="$(qmake -query QMAKE_SPEC 2>/dev/null || true)"
    printf '\nQt version: %s\n' "${qt_version:-unknown}"
    printf 'Qt spec:    %s\n' "${qt_spec:-unknown}"
    case "$qt_version" in
        5.15.*) printf '[OK]   Qt baseline matches MERIVUS Qt 5.15.x\n' ;;
        *) printf '[WARN] Qt baseline should be 5.15.x for this source snapshot\n' ;;
    esac
fi

if command -v pkg-config >/dev/null 2>&1; then
    printf '\nNative library checks\n'
    check_pkg sdl2 'install libsdl2-dev'
    check_pkg gstreamer-1.0 'install libgstreamer1.0-dev'
    check_pkg gstreamer-video-1.0 'install libgstreamer-plugins-base1.0-dev'
fi

printf '\nGit state\n'
if git -C "$repo_root" rev-parse --is-inside-work-tree >/dev/null 2>&1; then
    git -C "$repo_root" status --short
    tracked_count="$(git -C "$repo_root" ls-files | wc -l | tr -d ' ')"
    printf 'Tracked files: %s\n' "$tracked_count"
    if [ "$tracked_count" = "0" ]; then
        printf '[WARN] Git repository exists but no source files are tracked. Create a baseline commit before moving machines.\n'
    fi
else
    printf '[MISS] This directory is not a Git work tree.\n'
    missing=1
fi

printf '\nSITL reminder\n'
printf 'Use PX4 or ArduPilot SITL only; never point automated command tests at real aircraft.\n'
printf 'Start the simulator before MERIVUS and connect over the normal MAVLink UDP/TCP path.\n'

exit "$missing"
