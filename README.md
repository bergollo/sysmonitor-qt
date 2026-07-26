# qt-sysmonitor

Qt 6 system monitor for Linux, with Widgets and QML views backed by the same
threaded system-stat worker.

## Project layout

```
qt-sysmonitor/
├── CMakeLists.txt
├── README.md
├── LICENSE
├── .clang-format
├── .github/workflows/build.yml     # CI: build + unit tests (no display needed)
├── cmake/
│   ├── CompilerWarnings.cmake
│   └── toolchains/aarch64-linux-gnu.cmake
├── src/
│   ├── main.cpp                    # wires worker thread + both UIs together
│   ├── core/                       # domain logic, no UI dependency
│   │   ├── systemstats.{h,cpp}         plain data + cpuUsagePercent() math
│   │   └── systemmonitorworker.{h,cpp} QObject polling loop (own QThread)
│   ├── platform/linux/             # OS-specific reads, isolated for portability
│   │   └── procfs.{h,cpp}
│   └── ui/                         # Widgets + QML view-model, no platform code
│       ├── mainwindow.{h,cpp}
│       └── statsviewmodel.{h,cpp}
├── qml/
│   ├── Dashboard.qml
│   └── components/
│       └── StatCard.qml            # extracted reusable stat card
├── tests/
│   └── systemstats_test.cpp        # exercises core/ + platform/, no display
├── docs/architecture.md            # layering, threading model, known limitations
├── scripts/deploy_arm64.sh
└── packaging/qt-sysmonitor.service
```

See `docs/architecture.md` for the reasoning behind the `core` / `platform`
/ `ui` split and the threading model.

## Desktop build

```bash
sudo apt install cmake build-essential qt6-base-dev qt6-charts-dev qt6-declarative-dev
cmake -S . -B build
cmake --build build --parallel
./build/QtSysMonitor
```

Run the unit tests (pure `core`/`platform` logic, no display required):

```bash
ctest --test-dir build --output-on-failure
```

For a headless machine, functional (non-visual) testing only:

```bash
QT_QPA_PLATFORM=offscreen ./build/QtSysMonitor
```

## Viewing the QML tab over a VM / X11 forwarding

If you're building inside a VM and viewing output via `ssh -X` +
XQuartz, be aware of one specific limitation:

**The Widgets tab renders fine over X11 forwarding.** It's plain
`QPainter`/`QChartView` content — no GPU context required.

**The QML tab needs a working GL context to render correctly.**
X11-forwarded GLX frequently can't create one inside a VM
(`glx: failed to create drisw screen`, `QRhiGles2: Failed to create
context`). Forcing the Qt Quick software backend
(`QT_QUICK_BACKEND=software` or `QQuickWindow::setGraphicsApi(Software)`)
avoids the crash, but introduces a separate, known compositing issue:
`QQuickWidget` + the software backend can render washed-out/near-invisible
content even though layout and data binding are entirely correct. If you
see a mostly-white QML tab with faint text, this is that issue — not a bug
in `Dashboard.qml`.

**To actually see the QML tab correctly, use VNC instead of X11 forwarding:**

```bash
# In the VM
sudo apt install tigervnc-standalone-server
vncserver :1 -geometry 1280x800

# On macOS: Finder → Cmd+K → vnc://<vm-ip>:5901
```

VNC lets the VM render with its own (real or software/llvmpipe) GL context
locally and only streams pixels, rather than trying to forward the GL
protocol itself — so the normal (non-software-forced) Qt Quick backend
works as expected.

Use X11 forwarding for day-to-day functional iteration (does it build, do
values update, no crashes) and switch to VNC specifically when you need to
verify how the QML tab actually looks.

## ARM64 cross-compilation

Cross-compile only after the desktop build and runtime behavior are working.
The target needs an ARM64 C++ compiler and a Qt 6 ARM64 development sysroot.

Copy the example toolchain file and set paths for the installed SDK:

```bash
cp cmake/toolchains/aarch64-linux-gnu.cmake /tmp/sysmonitor-aarch64.cmake
${EDITOR:-vi} /tmp/sysmonitor-aarch64.cmake
```

At minimum, set:

- `CMAKE_SYSROOT` to the target SDK sysroot
- `CMAKE_C_COMPILER` / `CMAKE_CXX_COMPILER` to the ARM64 compilers
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

## Deploy to target

```bash
TARGET=pi@192.168.1.50 DEST=/opt/qt-sysmonitor ./scripts/deploy_arm64.sh
```

Prefer installing matching Qt packages on the target rather than copying
host x86_64 libraries. Verify with `ldd` on-device before launching (the
script does this for you).

## Identify the CPU thermal zone

Thermal-zone numbering is board-specific. Before changing the default path,
inspect the target:

```bash
for zone in /sys/class/thermal/thermal_zone*; do
    printf '%s: type=%s temp=%s\n' "$zone" \
        "$(cat "$zone/type" 2>/dev/null || echo unknown)" \
        "$(cat "$zone/temp" 2>/dev/null || echo unknown)"
done
```

Use the zone whose `type` identifies the CPU/package sensor. Temperatures
are in millidegrees Celsius (`55000` = `55.0°C`). `readThermalZone()`
currently returns the first readable zone — revisit that policy once
you've confirmed the target board's actual sensor layout.