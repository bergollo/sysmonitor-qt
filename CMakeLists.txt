cmake_minimum_required(VERSION 3.16)

project(QtSysMonitor VERSION 1.0 LANGUAGES CXX)

include(cmake/CompilerWarnings.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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
target_compile_features(QtSysMonitorCore PUBLIC cxx_std_20)
enable_project_warnings(QtSysMonitorCore)

qt_add_executable(QtSysMonitor
    src/main.cpp
    src/ui/mainwindow.cpp
    src/ui/mainwindow.h
    src/ui/statsviewmodel.cpp
    src/ui/statsviewmodel.h
)

target_include_directories(QtSysMonitor PRIVATE src)
target_compile_features(QtSysMonitor PRIVATE cxx_std_20)

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
target_compile_features(SystemStatsTests PRIVATE cxx_std_20)
enable_project_warnings(SystemStatsTests)

add_test(NAME SystemStatsTests COMMAND SystemStatsTests)

qt_add_executable(QmlRenderingTests
    tests/qml_rendering_test.cpp
    src/ui/statsviewmodel.cpp
    src/ui/statsviewmodel.h
)

target_include_directories(QmlRenderingTests PRIVATE src)
target_compile_features(QmlRenderingTests PRIVATE cxx_std_20)

qt_add_resources(QmlRenderingTests "qml_test"
    PREFIX "/qml"
    BASE "qml"
    FILES qml/Dashboard.qml qml/components/StatCard.qml
)

target_link_libraries(QmlRenderingTests PRIVATE
    Qt6::Core
    Qt6::Quick
    Qt6::QuickWidgets
    Qt6::Test
    Qt6::Widgets
    QtSysMonitorCore
)
enable_project_warnings(QmlRenderingTests)

add_test(NAME QmlRenderingTests COMMAND QmlRenderingTests)
set_tests_properties(QmlRenderingTests PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

install(TARGETS QtSysMonitor
    RUNTIME DESTINATION bin
)
install(FILES packaging/qt-sysmonitor.service
    DESTINATION lib/systemd/system
)
