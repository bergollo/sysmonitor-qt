#include "statsviewmodel.h"

#include "systemmonitor.h"

#include <QVariant>

StatsViewModel::StatsViewModel(QObject *parent)
    : QObject(parent)
{
}

double StatsViewModel::cpuPercent() const
{
    return m_cpuPercent;
}

double StatsViewModel::memoryPercent() const
{
    return m_memoryPercent;
}

int StatsViewModel::memoryUsedMb() const
{
    return m_memoryUsedMb;
}

int StatsViewModel::memoryTotalMb() const
{
    return m_memoryTotalMb;
}

QString StatsViewModel::temperature() const
{
    return m_temperature;
}

QVariantList StatsViewModel::cpuHistory() const
{
    return m_cpuHistory;
}

void StatsViewModel::updateStats(const SystemStats &stats)
{
    const auto usedMemoryKb = stats.memory.totalKb - stats.memory.availableKb;
    m_cpuPercent = stats.cpuPercent;
    m_memoryPercent = 100.0 * static_cast<double>(usedMemoryKb)
        / static_cast<double>(stats.memory.totalKb);
    m_memoryUsedMb = static_cast<int>(usedMemoryKb / 1024);
    m_memoryTotalMb = static_cast<int>(stats.memory.totalKb / 1024);
    m_temperature = stats.temperatureCelsius
        ? QString::number(*stats.temperatureCelsius, 'f', 1) + " C"
        : "unavailable";

    m_cpuHistory.append(m_cpuPercent);
    if (m_cpuHistory.size() > 60) {
        m_cpuHistory.removeFirst();
    }
    emit statsChanged();
}
