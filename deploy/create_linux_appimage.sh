#!/bin/bash
set -Eeuo pipefail

if [[ $# -lt 2 || $# -gt 3 ]]; then
  echo 'Usage: create_linux_appimage.sh QGC_SRC_DIR QGC_RELEASE_DIR [OUTPUT_DIR]' >&2
  exit 1
fi

QGC_SRC=$(readlink -f "$1")
QGC_RELEASE_DIR=$(readlink -f "$2")
OUTPUT_DIR=$(readlink -m "${3:-$PWD}")
QGC_CUSTOM_APP_NAME="${QGC_CUSTOM_APP_NAME:-QGroundControl}"
QGC_CUSTOM_GENERIC_NAME="${QGC_CUSTOM_GENERIC_NAME:-Ground Control Station}"
QGC_CUSTOM_BINARY_NAME="${QGC_CUSTOM_BINARY_NAME:-QGroundControl}"
QGC_CUSTOM_LINUX_START_SH="${QGC_CUSTOM_LINUX_START_SH:-${QGC_SRC}/deploy/qgroundcontrol-start.sh}"
QGC_CUSTOM_APP_ICON="${QGC_CUSTOM_APP_ICON:-${QGC_SRC}/resources/icons/qgroundcontrol.png}"
QGC_CUSTOM_APP_ICON_NAME="${QGC_CUSTOM_APP_ICON_NAME:-QGroundControl}"

for required_file in "$QGC_SRC/qgroundcontrol.pro" "$QGC_RELEASE_DIR/$QGC_CUSTOM_BINARY_NAME" "$QGC_CUSTOM_LINUX_START_SH" "$QGC_CUSTOM_APP_ICON"; do
  if [[ ! -f "$required_file" ]]; then
    echo "Missing packaging input: $required_file" >&2
    exit 1
  fi
done

packaging_dir=$(mktemp -d)
trap 'rm -rf -- "$packaging_dir"' EXIT
APPDIR="$packaging_dir/$QGC_CUSTOM_BINARY_NAME.AppDir"
mkdir -p "$APPDIR/usr/lib/x86_64-linux-gnu"

# Bundle the runtime from the build system; mixing an obsolete Debian SDL/DirectFB
# ABI with the compiler's Ubuntu sysroot produces a different dependency contract.
sdl_library=$(ldconfig -p | awk '$1 == "libSDL2-2.0.so.0" && /x86-64/ && !found { print $NF; found=1 }')
if [[ -z "$sdl_library" || ! -f "$sdl_library" ]]; then
  echo 'The x86_64 SDL2 runtime used by the build is required.' >&2
  exit 1
fi
cp -L "$sdl_library" "$APPDIR/usr/lib/x86_64-linux-gnu/libSDL2-2.0.so.0"
rsync -a --exclude='*.cpp' --exclude='*.h' --exclude='*.o' --exclude='CMake*' --exclude='*.cmake' --exclude='/package/' "$QGC_RELEASE_DIR/" "$APPDIR/"
cp "$QGC_CUSTOM_LINUX_START_SH" "$APPDIR/AppRun"
chmod +x "$APPDIR/AppRun"
cp "$QGC_CUSTOM_APP_ICON" "$APPDIR/$QGC_CUSTOM_APP_ICON_NAME.png"
cat > "$APPDIR/$QGC_CUSTOM_BINARY_NAME.desktop" <<EOF
[Desktop Entry]
Type=Application
Name=${QGC_CUSTOM_APP_NAME}
GenericName=${QGC_CUSTOM_GENERIC_NAME}
Comment=UAS ground control station
Icon=${QGC_CUSTOM_APP_ICON_NAME}
Exec=AppRun
Terminal=false
Categories=Utility;
Keywords=computer;
EOF

library_report=$(LD_LIBRARY_PATH="$APPDIR/Qt/libs:$APPDIR/usr/lib/x86_64-linux-gnu${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}" ldd "$APPDIR/$QGC_CUSTOM_BINARY_NAME")
if grep -q 'not found' <<< "$library_report"; then
  printf '%s\n' "$library_report" >&2
  exit 1
fi

appimagetool_sha256=d918b4df547b388ef253f3c9e7f6529ca81a885395c31f619d9aaf7030499a13
curl --fail --location --retry 3 --output "$packaging_dir/appimagetool.AppImage" \
  'https://github.com/AppImage/AppImageKit/releases/download/12/appimagetool-x86_64.AppImage'
printf '%s  %s\n' "$appimagetool_sha256" "$packaging_dir/appimagetool.AppImage" | sha256sum --check --status
chmod +x "$packaging_dir/appimagetool.AppImage"
cd "$packaging_dir"
# Extraction works in hosted runners without a FUSE device.
./appimagetool.AppImage --appimage-extract >/dev/null
version=$(git -C "$QGC_SRC" describe --always --tags --match 'v[0-9]*')
ARCH=x86_64 VERSION="$version" ./squashfs-root/AppRun "$APPDIR" "$packaging_dir/$QGC_CUSTOM_BINARY_NAME.AppImage"

mkdir -p "$OUTPUT_DIR"
cp "$packaging_dir/$QGC_CUSTOM_BINARY_NAME.AppImage" "$OUTPUT_DIR/"
cd "$OUTPUT_DIR"
sha256sum "$QGC_CUSTOM_BINARY_NAME.AppImage" > "$QGC_CUSTOM_BINARY_NAME.AppImage.sha256"
{
  printf 'source_commit=%s\n' "$(git -C "$QGC_SRC" rev-parse HEAD)"
  printf 'source_version=%s\n' "$version"
  printf 'build_type=%s\n' "${BUILD_TYPE:-unspecified}"
  printf 'appimagetool_sha256=%s\n' "$appimagetool_sha256"
  . /etc/os-release
  printf 'build_os=%s\n' "$PRETTY_NAME"
  printf 'qt_version=%s\n' "$(qmake -query QT_VERSION)"
  g++ --version | head -n 1
  dpkg-query -W -f='${binary:Package}=${Version}\n' 'libsdl2-2.0-0*'
} > "$QGC_CUSTOM_BINARY_NAME.AppImage.build-info"
printf 'Created %s/%s.AppImage\n' "$OUTPUT_DIR" "$QGC_CUSTOM_BINARY_NAME"
