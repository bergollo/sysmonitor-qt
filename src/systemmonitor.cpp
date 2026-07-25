#include "systemmonitor.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>

#include <QTimer>

namespace {

std::uint64_t totalCpuTime(const CpuTimes &times)
{
    return times.user + times.nice + times.system + times.idle + times.iowait
        + times.irq + times.softirq + times.steal;
}

}

std::optional<CpuTimes> readCpuTimes(const std::string &path)
{
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
    std::error_code error;
    for (const auto &entry : std::filesystem::directory_iterator(basePath, error)) {
        if (error || !entry.is_directory()) {
            continue;
        }

        const auto temperaturePath = entry.path() / "temp";
        std::ifstream input(temperaturePath);
        double millidegrees;
        if (input >> millidegrees) {
            return millidegrees / 1000.0;
        }
    }
    return std::nullopt;
}

double cpuUsagePercent(const CpuTimes &previous, const CpuTimes &current)
{
    const auto previousTotal = totalCpuTime(previous);
    const auto currentTotal = totalCpuTime(current);
    const auto previousIdle = previous.idle + previous.iowait;
    const auto currentIdle = current.idle + current.iowait;
    const auto totalDelta = currentTotal - previousTotal;
    const auto idleDelta = currentIdle - previousIdle;

    if (totalDelta == 0 || currentTotal < previousTotal || currentIdle < previousIdle) {
        return 0.0;
    }
    return std::clamp(100.0 * static_cast<double>(totalDelta - idleDelta)
                          / static_cast<double>(totalDelta),
                      0.0, 100.0);
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

void SystemMonitorWorker::start()
{
    timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, &QTimer::timeout, this, &SystemMonitorWorker::poll);
    timer->start();
    poll();
}

void SystemMonitorWorker::poll()
{
    const auto stats = readSystemStats(previousCpuTimes);
    if (!stats) {
        emit finished();
        return;
    }

    previousCpuTimes = readCpuTimes();
    emit statsReady(*stats);
}
