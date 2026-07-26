# CMake

CMake describes a target dependency graph; it is not primarily a shell script.

- `project()` defines the project and languages.
- `find_package()` discovers Qt development packages.
- `add_library(QtSysMonitorCore ...)` creates the reusable domain/platform
  target.
- `target_link_libraries()` records both link dependencies and usage
  requirements.
- `target_include_directories(... PUBLIC src)` makes layer-qualified includes
  such as `core/systemstats.h` available to consumers.
- `qt_standard_project_setup()` enables Qt's generated-code conventions.
- `qt_add_resources()` embeds QML into the binary.
- `enable_testing()` and `add_test()` register CTest tests.
- `install()` describes the runtime layout used by packaging.

The core library is linked by both the GUI and test executable. This prevents
tests from compiling a subtly different copy of parser code.

Use separate build directories for native and ARM64 builds. A toolchain file
sets the compiler, sysroot, and target Qt package paths; mixing caches can make
CMake appear to find host libraries for a target build.
