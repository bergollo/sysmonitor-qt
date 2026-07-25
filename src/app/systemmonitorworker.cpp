#include "systemmonitorworker.h"

#include "../platform/procfs.h"

#include <QTimer>

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
