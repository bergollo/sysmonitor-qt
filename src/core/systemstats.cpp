#include "systemstats.h"

#include <algorithm>

namespace {

std::uint64_t totalCpuTime(const CpuTimes &times)
{
    return times.user + times.nice + times.system + times.idle + times.iowait
        + times.irq + times.softirq + times.steal;
}

}

double cpuUsagePercent(const CpuTimes &previous, const CpuTimes &current)
{
    const auto previousTotal = totalCpuTime(previous);
    const auto currentTotal = totalCpuTime(current);
    const auto previousIdle = previous.idle + previous.iowait;
    const auto currentIdle = current.idle + current.iowait;

    if (currentTotal < previousTotal || currentIdle < previousIdle) {
        return 0.0;
    }

    const auto totalDelta = currentTotal - previousTotal;
    const auto idleDelta = currentIdle - previousIdle;
    if (totalDelta == 0 || idleDelta > totalDelta) {
        return 0.0;
    }

    return std::clamp(100.0 * static_cast<double>(totalDelta - idleDelta)
                          / static_cast<double>(totalDelta),
                      0.0, 100.0);
}
