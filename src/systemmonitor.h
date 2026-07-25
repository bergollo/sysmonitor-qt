#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include <QObject>

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

std::optional<CpuTimes> readCpuTimes(const std::string &path = "/proc/stat");
std::optional<MemoryInfo> readMemoryInfo(const std::string &path = "/proc/meminfo");
std::optional<double> readThermalZone(const std::string &basePath = "/sys/class/thermal");
double cpuUsagePercent(const CpuTimes &previous, const CpuTimes &current);
std::optional<SystemStats> readSystemStats(const std::optional<CpuTimes> &previous);

class SystemMonitorWorker final : public QObject {
    Q_OBJECT

public slots:
    void start();

signals:
    void statsReady(const SystemStats &stats);
    void finished();

private slots:
    void poll();

private:
    std::optional<CpuTimes> previousCpuTimes;
    class QTimer *timer = nullptr;
};
