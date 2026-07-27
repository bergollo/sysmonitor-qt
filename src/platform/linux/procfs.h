#pragma once

#include "core/systemstats.h"

#include <optional>
#include <string>

[[nodiscard("CPU counters must be checked before calculating deltas")]]
std::optional<CpuTimes> readCpuTimes(const std::string &path = "/proc/stat");
[[nodiscard("memory parsing can fail and must be handled")]]
std::optional<MemoryInfo> readMemoryInfo(const std::string &path = "/proc/meminfo");
[[nodiscard("a missing thermal sensor is a valid but observable state")]]
std::optional<double> readThermalZone(const std::string &basePath = "/sys/class/thermal");
[[nodiscard("system statistics must be handled before reaching the UI")]]
std::optional<SystemStats> readSystemStats(const std::optional<CpuTimes> &previous);
