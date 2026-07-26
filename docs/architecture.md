# Architecture

The application is split into three layers:

- `src/core/` contains system-stat data types, CPU delta calculation, and the
  Qt worker that polls the platform layer and emits `statsReady`.
- `src/platform/linux/` reads Linux `/proc` and thermal-zone files. A future
  platform implementation can live beside it without changing the UI.
- `src/ui/` contains the Widgets window and QML `StatsViewModel` bridge.

Both UI views receive the same `SystemMonitorWorker::statsReady` signal. This
keeps parsing, threading, and presentation failures independently diagnosable.

## QML Rendering

The QML view is embedded with `QQuickWidget` and uses the software scenegraph
backend to avoid crashes on systems without a usable GLX context. X11
forwarding can still produce washed-out QML compositing even when the view is
functionally correct. Use VNC or a local display for visual QML validation;
use X11 forwarding for startup and data-binding checks.
