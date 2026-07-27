#pragma once

#include "core/systemstats.h"

#include <optional>
#include <string>

[[nodiscard]] std::optional<CpuTimes> readCpuTimes(const std::string &path = "/proc/stat");
[[nodiscard]] std::optional<MemoryInfo> readMemoryInfo(const std::string &path = "/proc/meminfo");
[[nodiscard]] std::optional<double> readThermalZone(const std::string &basePath = "/sys/class/thermal");
[[nodiscard]] std::optional<SystemStats> readSystemStats(const std::optional<CpuTimes> &previous);
