# qt-sysmonitor

Qt 6 system monitor for Linux, with Widgets and QML views backed by the same
threaded system-stat worker.

## Desktop Build

Install the Qt 6 development packages required by the application:

```bash
sudo apt install cmake build-essential qt6-base-dev qt6-charts-dev qt6-declarative-dev
```

Configure and build:

```bash
cmake -S . -B build
cmake --build build --parallel
./build/QtSysMonitor
```

For a headless development machine, use the software renderer:

```bash
QT_QPA_PLATFORM=offscreen ./build/QtSysMonitor
```

## ARM64 Cross-Compilation

Cross-compile only after the desktop build and runtime behavior are working.
The target needs an ARM64 C++ compiler and a Qt 6 ARM64 development sysroot.

Copy the example toolchain file and set paths for the installed SDK:

```bash
cp cmake/toolchains/aarch64-linux-gnu.cmake /tmp/sysmonitor-aarch64.cmake
${EDITOR:-vi} /tmp/sysmonitor-aarch64.cmake
```

At minimum, set:

- `CMAKE_SYSROOT` to the target SDK sysroot
- `CMAKE_C_COMPILER` to the ARM64 C compiler
- `CMAKE_CXX_COMPILER` to the ARM64 C++ compiler
- `QT_HOST_PATH` to the host-side Qt tools used by CMake
- `CMAKE_PREFIX_PATH` to the Qt 6 installation inside the ARM64 sysroot

Configure in a separate build directory so the native build cache is never
mixed with the cross build:

```bash
cmake -S . -B build-aarch64 \
  -DCMAKE_TOOLCHAIN_FILE=/tmp/sysmonitor-aarch64.cmake \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build-aarch64 --parallel
```

The resulting binary is `build-aarch64/QtSysMonitor`. Qt runtime libraries,
the platform plugin, and QML modules must also be installed for the target
architecture. The exact deployment set depends on the target image and Qt
configuration; do not copy host x86_64 libraries to an ARM64 device.

## Deploy To Target

For a target reachable over SSH, copy the executable and its Qt runtime
deployment directory to a staging directory first:

```bash
export TARGET=pi@192.168.1.50
export DEST=/opt/qt-sysmonitor
ssh "$TARGET" "sudo mkdir -p $DEST && sudo chown $USER:$USER $DEST"
scp build-aarch64/QtSysMonitor "$TARGET:$DEST/"
```

Prefer installing matching Qt packages on the target. If the target image
does not provide them, use a Qt deployment tool or copy the matching ARM64
libraries, plugins, and QML modules from the same SDK. Verify dependencies on
the device before launching:

```bash
ssh "$TARGET" "file $DEST/QtSysMonitor; ldd $DEST/QtSysMonitor"
ssh -X "$TARGET" "$DEST/QtSysMonitor"
```

For an embedded display session, set the platform appropriate to the image,
for example `-platform eglfs` or `-platform wayland`. Do not use `offscreen`
when you need to inspect the actual display output.

## Identify The CPU Thermal Zone

Thermal-zone numbering is board-specific. Before changing the default path or
adding a board-specific configuration, inspect the target:

```bash
for zone in /sys/class/thermal/thermal_zone*; do
    printf '%s: ' "$zone"
    printf 'type='; cat "$zone/type" 2>/dev/null || printf 'unknown'
    printf 'temp='; cat "$zone/temp" 2>/dev/null || printf 'unknown'
    printf '\n'
done
```

Use the zone whose `type` identifies the CPU or package sensor. Temperatures
are normally reported in millidegrees Celsius, so `55000` means `55.0 C`.
The reader currently scans available zones and uses the first readable
temperature; update that policy only after confirming the target board's
sensor layout.
