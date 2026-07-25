set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# Replace these example values with paths from the target SDK.
set(CMAKE_SYSROOT "/path/to/aarch64-sysroot")
set(CMAKE_C_COMPILER "/usr/bin/aarch64-linux-gnu-gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/aarch64-linux-gnu-g++")

# Qt host tools run on the development machine; Qt libraries and packages
# below must be built for ARM64.
set(QT_HOST_PATH "/path/to/host/Qt/6.x/gcc_64")
set(CMAKE_PREFIX_PATH "/path/to/aarch64-sysroot/usr/lib/aarch64-linux-gnu/cmake/Qt6")

set(CMAKE_FIND_ROOT_PATH "${CMAKE_SYSROOT}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
