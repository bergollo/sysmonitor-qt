# AGENT.md

## Project Overview

QtSysMonitor is a Qt 6 (C++17) reference/teaching app that shows the same live CPU/memory/temperature
data in two UIs — QtWidgets and QML — driven by one `SystemMonitorWorker` on a background `QThread`.
Requires CMake ≥ 3.16 and Qt 6 (Core, Widgets, Charts, Quick, QuickWidgets, Test); targets Ubuntu/Linux
natively, with an optional ARM64 cross-build via `cmake/toolchains/aarch64-linux-gnu.cmake`.

## Tech Stack

- **Language:** C++17, QML
- **Framework:** Qt 6 — Core, Widgets, Charts, Quick, QuickWidgets, Test
- **Build:** CMake ≥ 3.16 (three targets: `QtSysMonitorCore` static lib, `QtSysMonitor` executable, `SystemStatsTests`)
- **Platform:** Linux (`/proc`, `/sys/class/thermal`); ARM64 cross-compile supported, not primary

## Commands

```bash
# Install deps (Ubuntu)
sudo apt install cmake build-essential qt6-base-dev qt6-charts-dev qt6-declarative-dev

# Configure + build
cmake -S . -B build
cmake --build build --parallel

# Test
ctest --test-dir build --output-on-failure

# Run (headless smoke test — no display required)
QT_QPA_PLATFORM=offscreen ./build/QtSysMonitor

# Format (before finishing any change)
clang-format -i $(git diff --name-only -- '*.cpp' '*.h')
git diff --check
```

There is no separate lint step beyond compiler warnings (`cmake/CompilerWarnings.cmake`, applied to
every target via `enable_project_warnings(...)`) and `.clang-format`. A warning is a build failure to
fix, not something to suppress.

## Directory Orientation

| Path | What lives here |
|---|---|
| `src/core/` | Plain C++ + `QObject` worker — `systemstats.*` (math), `systemmonitorworker.*` (polling loop). No Widgets/QML includes allowed. |
| `src/platform/linux/` | `procfs.*` — the *only* code allowed to read `/proc` or `/sys/class/thermal`. |
| `src/ui/` | `mainwindow.*` (Widgets), `statsviewmodel.*` (QML view-model). Both consume `core/`, never `platform/` directly. |
| `qml/` | `Dashboard.qml` + `components/` (e.g. `StatCard.qml`). |
| `tests/` | `systemstats_test.cpp` — exercises `QtSysMonitorCore` only, no display needed. |
| `cmake/` | `CompilerWarnings.cmake`, `toolchains/aarch64-linux-gnu.cmake`. |
| `docs/` | Deeper docs — see Pointers below. |

## Coding Conventions

**Forward-declare Qt types in headers; include full headers only in `.cpp` files.**
```cpp
// Right — mainwindow.h
class QLabel;
class QProgressBar;
class MainWindow final : public QMainWindow { ... };

// Wrong — pulls QLabel's full definition into every file that includes mainwindow.h
#include <QLabel>
#include <QProgressBar>
class MainWindow final : public QMainWindow { ... };
```

**Plain data crossing thread boundaries is a `struct` of values, not a `QObject` or shared pointer.**
```cpp
// Right — SystemStats is a value type, copied by the queued signal/slot connection
struct SystemStats { double cpuPercent = 0.0; MemoryInfo memory; std::optional<double> temperatureCelsius; };
signals: void statsReady(const SystemStats &stats);

// Wrong — sharing a mutable object across threads invites a race regardless of signals/slots
signals: void statsReady(SystemStats *stats);
```

**Include paths are rooted at `src/`, not relative.**
```cpp
// Right
#include "core/systemstats.h"

// Wrong
#include "../core/systemstats.h"
```

**Classes that shouldn't be subclassed further are `final`; members get inline default values instead of constructor boilerplate where possible** (see `mainwindow.h`, `systemstats.h` for the pattern already in use — match it rather than introducing a new style).

**QML components under `qml/components/` are imported explicitly**, not assumed to be globally visible:
```qml
// Right — Dashboard.qml
import "components"
StatCard { ... }
```

## Hard Constraints / Gotchas

- **`src/platform/linux/` is the only layer permitted to touch `/proc` or `/sys` files.** Don't add file I/O to `core/` or `ui/` — route it through `procfs.*` even for a "quick" addition.
- **`SystemMonitorWorker`'s `QTimer` must be constructed inside `start()`, never the constructor.** It has to be created after the object lands on its `QThread` (i.e. after `QThread::started`), or its affinity is wrong.
- **Thread shutdown is `quit()` then `wait()` — never `terminate()`.**
- **`QQuickWindow::setGraphicsApi(QSGRendererInterface::Software)` in `main.cpp` is intentional**, not a leftover debug flag — it's what keeps the app usable headless/over X11 forwarding. Don't remove it without checking `docs/qml.md` and `docs/architecture.md` first. Note X11-forwarded `QQuickWidget` content can still look washed out even with this set — that's a known display-path limitation (use VNC/local display to visually verify QML), not a code bug to "fix."
- **Don't add `QtQuick.Controls` types (`Label`, etc.) to QML** without also adding the Controls style plugin package — this breaks the build with `<Type> is not a type` on machines that only have the base Quick modules. `Dashboard.qml` deliberately uses plain `Text`/`Rectangle` for this reason.
- **Native and ARM64 builds must use separate build directories** (`build/` vs `build-aarch64/` or similar) — never reconfigure the native cache with the cross-toolchain file.
- **`SystemStatsTests` links `QtSysMonitorCore`** — do not add duplicate copies of `core/`/`platform/` sources directly to the test target.
- **Generated files are off-limits:** anything under `build*/`, `moc_*.cpp`, `moc_*.h`, `qrc_*.cpp`, `ui_*.h`, `CMakeCache.txt`, `compile_commands.json` (see `.gitignore`). If one needs to change, the fix belongs in the source or CMake, not the generated artifact.
- **`cmake/toolchains/aarch64-linux-gnu.cmake` ships with placeholder paths on purpose** — don't commit real device-specific sysroot/compiler paths into this file; set them via a local copy or `-D` overrides instead.
- **`scripts/deploy_arm64.sh` takes `TARGET`/`DEST` as environment variables** — don't hardcode a device hostname, IP, or credential into the script or into any tracked file.

## Deeper Docs

Start at `docs/README.md` for the full learning-path order. Most relevant per task:
- `docs/architecture.md` — layering rationale and threading model
- `docs/qt-core.md` — threading/signal patterns used throughout `core/`
- `docs/qt-widgets.md`, `docs/qml.md` — UI-layer specifics for `ui/` and `qml/`
- `docs/cmake.md` — target graph and build-system decisions
- `docs/testing.md` — what `tests/` does and doesn't cover
- `docs/deployment.md` — ARM64 cross-compile and on-device deployment