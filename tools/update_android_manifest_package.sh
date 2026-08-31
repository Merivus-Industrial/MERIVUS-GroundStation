#!/usr/bin/env bash

set -euo pipefail

readonly MANIFEST_FILE="android/AndroidManifest.xml"
readonly QGC_PKG_NAME="org.mavlink.qgroundcontrolbeta"

echo "Adjusting package name for daily build"
sed -i -E "s/package *= *\"[^\"]*\"/package=\"${QGC_PKG_NAME}\"/" "${MANIFEST_FILE}"
