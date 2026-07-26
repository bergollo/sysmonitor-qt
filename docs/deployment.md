# Deployment

Native deployment is the reference path. Build and test it before introducing
ARM64 variables or board-specific runtime assumptions.

For ARM64, the compiler, Qt libraries, Qt host tools, and QML modules must be
consistent. `CMAKE_SYSROOT` describes target headers/libraries while
`QT_HOST_PATH` points to host tools that execute during the build. Never copy
host-architecture Qt libraries to the device.

The install target places the application in `bin/` and the systemd unit in
`lib/systemd/system/`. The deployment script copies the cross-built binary;
the target still needs matching Qt runtime packages, platform plugins, and QML
modules.

Verify target architecture and dependencies:

```bash
file /opt/qt-sysmonitor/QtSysMonitor
ldd /opt/qt-sysmonitor/QtSysMonitor
```

Thermal-zone numbering is board-specific. Inspect each `thermal_zoneN/type`
and `temp` before selecting a CPU sensor. The current reader uses the first
readable zone as a deliberately simple reference policy.
