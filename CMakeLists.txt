cmake_minimum_required(VERSION 3.16)

project(QtSysMonitor VERSION 1.0 LANGUAGES CXX)

include(cmake/CompilerWarnings.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# CMake describes target relationships. The core target is reusable by both
# the GUI and tests, so parser code cannot accidentally depend on UI modules.

find_package(Qt6 REQUIRED COMPONENTS Charts Core Quick QuickWidgets Test Widgets)

qt_standard_project_setup()

add_library(QtSysMonitorCore STATIC
    src/core/systemstats.cpp
    src/core/systemstats.h
    src/core/systemmonitorworker.cpp
    src/core/systemmonitorworker.h
    src/platform/linux/procfs.cpp
    src/platform/linux/procfs.h
)

target_include_directories(QtSysMonitorCore PUBLIC src)
target_link_libraries(QtSysMonitorCore PUBLIC Qt6::Core)
enable_project_warnings(QtSysMonitorCore)

qt_add_executable(QtSysMonitor
    src/main.cpp
    src/ui/mainwindow.cpp
    src/ui/mainwindow.h
    src/ui/statsviewmodel.cpp
    src/ui/statsviewmodel.h
)

target_include_directories(QtSysMonitor PRIVATE src)

qt_add_resources(QtSysMonitor "qml"
    PREFIX "/qml"
    BASE "qml"
    FILES qml/Dashboard.qml qml/components/StatCard.qml
)

target_link_libraries(QtSysMonitor PRIVATE
    Qt6::Charts
    Qt6::Core
    Qt6::Quick
    Qt6::QuickWidgets
    Qt6::Widgets
    QtSysMonitorCore
)
enable_project_warnings(QtSysMonitor)

enable_testing()

qt_add_executable(SystemStatsTests
    tests/systemstats_test.cpp
)

target_link_libraries(SystemStatsTests PRIVATE Qt6::Core Qt6::Test QtSysMonitorCore)
target_include_directories(SystemStatsTests PRIVATE src)
enable_project_warnings(SystemStatsTests)

add_test(NAME SystemStatsTests COMMAND SystemStatsTests)

install(TARGETS QtSysMonitor
    RUNTIME DESTINATION bin
)
install(FILES packaging/qt-sysmonitor.service
    DESTINATION lib/systemd/system
)
