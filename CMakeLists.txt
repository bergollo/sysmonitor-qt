cmake_minimum_required(VERSION 3.16)

project(QtSysMonitor VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Charts Core Quick QuickWidgets Widgets)

qt_standard_project_setup()

qt_add_executable(QtSysMonitor
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/statsviewmodel.cpp
    src/statsviewmodel.h
    src/systemmonitor.cpp
    src/systemmonitor.h
)

qt_add_resources(QtSysMonitor "qml"
    PREFIX "/qml"
    FILES qml/Dashboard.qml
)

target_link_libraries(QtSysMonitor PRIVATE
    Qt6::Charts
    Qt6::Core
    Qt6::Quick
    Qt6::QuickWidgets
    Qt6::Widgets
)
