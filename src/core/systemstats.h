#pragma once

#include <cstdint>
#include <optional>

struct CpuTimes {
    std::uint64_t user = 0;
    std::uint64_t nice = 0;
    std::uint64_t system = 0;
    std::uint64_t idle = 0;
    std::uint64_t iowait = 0;
    std::uint64_t irq = 0;
    std::uint64_t softirq = 0;
    std::uint64_t steal = 0;
};

struct MemoryInfo {
    std::uint64_t totalKb = 0;
    std::uint64_t availableKb = 0;
};

struct SystemStats {
    double cpuPercent = 0.0;
    MemoryInfo memory;
    std::optional<double> temperatureCelsius;
};

double cpuUsagePercent(const CpuTimes &previous, const CpuTimes &current);
