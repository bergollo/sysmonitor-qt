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

## C++20 Configuration

The project sets the language level explicitly:

```cmake
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
```

Targets also declare `cxx_std_20` with `target_compile_features`. The global
setting keeps the project readable; the target requirement makes the dependency
explicit for consumers and future target restructuring. `CMAKE_CXX_EXTENSIONS OFF`
requests portable ISO C++ rather than compiler-specific GNU extensions.

After changing the standard, reconfigure the build directory. If a compiler or
toolchain appears cached incorrectly, configure a fresh directory such as
`build-cxx20`; never edit generated build files.
