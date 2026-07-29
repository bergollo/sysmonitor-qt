cmake_minimum_required(VERSION 3.16)

project(QtSysMonitor VERSION 1.0 LANGUAGES CXX)

include(cmake/CompilerWarnings.cmake)
include(cmake/StaticAnalysis.cmake)
include(cmake/Sanitizers.cmake)

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
    src/core/monitorconstants.h
    src/core/systemmonitorworker.cpp
    src/core/systemmonitorworker.h
    src/platform/linux/procfs.cpp
    src/platform/linux/procfs.h
)

target_include_directories(QtSysMonitorCore PUBLIC src)
target_link_libraries(QtSysMonitorCore PUBLIC Qt6::Core)
target_compile_features(QtSysMonitorCore PUBLIC cxx_std_20)
enable_project_warnings(QtSysMonitorCore)
enable_project_clang_tidy(QtSysMonitorCore)
enable_project_sanitizer(QtSysMonitorCore)

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
enable_project_clang_tidy(QtSysMonitor)
enable_project_sanitizer(QtSysMonitor)

enable_testing()

include(FetchContent)

option(SYSMONITOR_FETCH_TEST_DEPS "Download pinned test dependencies when unavailable" ON)

find_package(GTest CONFIG QUIET)

if(NOT GTest_FOUND AND SYSMONITOR_FETCH_TEST_DEPS)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG 52eb8108c5bdec04579160ae17225d66034bd723 # v1.17.0
    )
    FetchContent_MakeAvailable(googletest)
elseif(NOT GTest_FOUND)
    find_package(GTest CONFIG REQUIRED)
endif()

add_executable(SystemStatsGTests
    tests/systemstats_gtest.cpp
)

target_link_libraries(SystemStatsGTests PRIVATE GTest::gtest_main QtSysMonitorCore)
target_include_directories(SystemStatsGTests PRIVATE src)
target_compile_features(SystemStatsGTests PRIVATE cxx_std_20)
enable_project_warnings(SystemStatsGTests)
enable_project_clang_tidy(SystemStatsGTests)
enable_project_sanitizer(SystemStatsGTests)

include(GoogleTest)
gtest_discover_tests(SystemStatsGTests
    PROPERTIES LABELS "unit"
    TEST_LIST SystemStatsGTestCases
)
set_tests_properties(${SystemStatsGTestCases} PROPERTIES LABELS "unit;sanitizer")
if(NOT SYSMONITOR_SANITIZER STREQUAL "none")
    # GoogleTest discovery happens at build/test time, so provide one stable
    # CTest entry for sanitizer runners in addition to individual test cases.
    add_test(NAME SystemStatsGTestsSanitized COMMAND SystemStatsGTests)
    set_tests_properties(SystemStatsGTestsSanitized PROPERTIES LABELS "sanitizer")
endif()

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
enable_project_clang_tidy(QmlRenderingTests)

add_test(NAME QmlRenderingTests COMMAND QmlRenderingTests)
set_tests_properties(QmlRenderingTests PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
    LABELS "integration;qml;headless;sanitizer"
)

qt_add_executable(SystemMonitorWorkerTests
    tests/systemmonitorworker_test.cpp
)

target_include_directories(SystemMonitorWorkerTests PRIVATE src)
target_compile_features(SystemMonitorWorkerTests PRIVATE cxx_std_20)
target_link_libraries(SystemMonitorWorkerTests PRIVATE Qt6::Core Qt6::Test QtSysMonitorCore)
enable_project_warnings(SystemMonitorWorkerTests)
enable_project_clang_tidy(SystemMonitorWorkerTests)
enable_project_sanitizer(SystemMonitorWorkerTests)

add_test(NAME SystemMonitorWorkerTests COMMAND SystemMonitorWorkerTests)
set_tests_properties(SystemMonitorWorkerTests PROPERTIES LABELS "integration;headless;sanitizer")

install(TARGETS QtSysMonitor
    RUNTIME DESTINATION bin
)
install(FILES packaging/qt-sysmonitor.service
    DESTINATION lib/systemd/system
)
