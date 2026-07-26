# Learning Path

This project assumes a solid software-engineering background and a C++11
foundation, but a long gap since day-to-day C++. Read the documents in this
order:

1. [Modern C++](cpp-modernization.md)
2. [Qt Core and threading](qt-core.md)
3. [Widgets](qt-widgets.md)
4. [QML](qml.md)
5. [CMake](cmake.md)
6. [Testing](testing.md)
7. [Architecture](architecture.md)
8. [Exercises](exercises.md)
9. [Deployment](deployment.md)

Use the source as the worked example. `src/core/` is the best starting point
for plain C++, `src/platform/linux/` shows boundary code, and `src/ui/` shows
Qt ownership and presentation. The build target graph is in `CMakeLists.txt`.

## Feedback Loop

```bash
cmake -S . -B build
cmake --build build --parallel
ctest --test-dir build --output-on-failure
QT_QPA_PLATFORM=offscreen ./build/QtSysMonitor
```

The first three commands validate code and tests. The last command validates
startup and signal flow without requiring a display; use VNC or a local
display for QML visual checks.

## Before Changing Code

- Identify which layer owns the behavior.
- Decide whether the data is a value snapshot or shared mutable state.
- Check which thread owns the QObject being changed.
- Add or update a focused test before moving parsing or calculations.
- Keep platform dependencies out of core math and UI code.
