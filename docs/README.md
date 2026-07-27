# Learning Path

This project assumes a solid software-engineering background and a C++11
foundation, but a long gap since day-to-day C++. Read the documents in this
order:

1. [C++11, C++17, and C++20 comparison](cpp11-17-20.md)
2. [C++20 attributes and compile-time properties](cpp20-attributes.md)
3. [Modern C++](cpp-modernization.md)
4. [Qt Core and threading](qt-core.md)
5. [Widgets](qt-widgets.md)
6. [QML](qml.md)
7. [CMake](cmake.md)
8. [Testing](testing.md)
9. [Architecture](architecture.md)
10. [Exercises](exercises.md)
11. [Deployment](deployment.md)

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

## C++20 Verification

The project requires a compiler with C++20 support. Verify the selected standard
in the generated compile database:

```bash
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
grep -R -- '-std=c++20' build/compile_commands.json
```

After changing the language standard, a fresh build directory is a useful
diagnostic if a compiler or toolchain appears cached incorrectly:

```bash
cmake -S . -B build
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Format C++ sources with the repository's `.clang-format` configuration:

```bash
clang-format -i \
  src/main.cpp src/core/*.cpp src/core/*.h \
  src/platform/linux/*.cpp src/platform/linux/*.h \
  src/ui/*.cpp src/ui/*.h tests/*.cpp
git diff --check
```

## Before Changing Code

- Identify which layer owns the behavior.
- Decide whether the data is a value snapshot or shared mutable state.
- Check which thread owns the QObject being changed.
- Add or update a focused test before moving parsing or calculations.
- Keep platform dependencies out of core math and UI code.
