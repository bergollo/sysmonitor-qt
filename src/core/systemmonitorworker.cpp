#include "core/systemmonitorworker.h"

#include "platform/linux/procfs.h"

#include <QTimer>

void SystemMonitorWorker::start()
{
    // This slot runs after QThread::started, so the timer is constructed in
    // the worker thread and its timeout events are delivered there as well.
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

    // Signals carry a value snapshot. The GUI never reads mutable worker
    // state directly, so queued delivery does not require a shared mutex.
    previousCpuTimes = readCpuTimes();
    emit statsReady(*stats);
}
