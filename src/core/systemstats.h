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

    bool operator==(const CpuTimes &) const = default;
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

/**
 * Calculates utilization from two cumulative /proc/stat snapshots.
 *
 * The kernel counters are totals since boot, so either snapshot alone is not
 * a percentage. We compare deltas and treat backwards or impossible counters
 * as an invalid sample instead of exposing a misleading value to the UI.
 */
[[nodiscard]] double cpuUsagePercent(const CpuTimes &previous, const CpuTimes &current);
