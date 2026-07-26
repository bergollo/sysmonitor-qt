#pragma once

#include "core/systemstats.h"

#include <optional>
#include <string>

std::optional<CpuTimes> readCpuTimes(const std::string &path = "/proc/stat");
std::optional<MemoryInfo> readMemoryInfo(const std::string &path = "/proc/meminfo");
std::optional<double> readThermalZone(const std::string &basePath = "/sys/class/thermal");
std::optional<SystemStats> readSystemStats(const std::optional<CpuTimes> &previous);
