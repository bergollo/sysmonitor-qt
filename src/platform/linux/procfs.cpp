#include "platform/linux/procfs.h"

#include <filesystem>
#include <fstream>
#include <sstream>

std::optional<CpuTimes> readCpuTimes(const std::string &path)
{
    // A path parameter makes the parser deterministic in tests while the
    // default remains the real Linux file for production use.
    std::ifstream input(path);
    std::string line;
    if (!input || !std::getline(input, line)) {
        return std::nullopt;
    }

    std::istringstream values(line);
    std::string label;
    CpuTimes times;
    if (!(values >> label >> times.user >> times.nice >> times.system >> times.idle
          >> times.iowait >> times.irq >> times.softirq >> times.steal)
        || label != "cpu") {
        return std::nullopt;
    }
    return times;
}

std::optional<MemoryInfo> readMemoryInfo(const std::string &path)
{
    std::ifstream input(path);
    if (!input) {
        return std::nullopt;
    }

    MemoryInfo memory;
    std::string label;
    std::uint64_t value;
    std::string unit;
    while (input >> label >> value >> unit) {
        if (label == "MemTotal:") {
            memory.totalKb = value;
        } else if (label == "MemAvailable:") {
            memory.availableKb = value;
        }
    }

    if (memory.totalKb == 0 || memory.availableKb > memory.totalKb) {
        return std::nullopt;
    }
    return memory;
}

std::optional<double> readThermalZone(const std::string &basePath)
{
    // std::error_code avoids throwing for a missing board-specific thermal
    // directory; an unavailable sensor is a valid runtime state.
    std::error_code error;
    for (const auto &entry : std::filesystem::directory_iterator(basePath, error)) {
        if (error || !entry.is_directory()) {
            continue;
        }

        std::ifstream input(entry.path() / "temp");
        double millidegrees;
        if (input >> millidegrees) {
            return millidegrees / 1000.0;
        }
    }
    return std::nullopt;
}

std::optional<SystemStats> readSystemStats(const std::optional<CpuTimes> &previous)
{
    const auto current = readCpuTimes();
    const auto memory = readMemoryInfo();
    if (!current || !memory) {
        return std::nullopt;
    }

    SystemStats stats;
    stats.cpuPercent = previous ? cpuUsagePercent(*previous, *current) : 0.0;
    stats.memory = *memory;
    stats.temperatureCelsius = readThermalZone();
    return stats;
}
