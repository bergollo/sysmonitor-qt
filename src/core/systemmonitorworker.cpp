#include "core/systemmonitorworker.h"
#include "core/monitorconstants.h"

#include <QTimer>

#include <utility>

SystemMonitorWorker::SystemMonitorWorker(
    std::function<std::optional<SystemStats>()> reader)
    : reader(std::move(reader))
{
}

void SystemMonitorWorker::start()
{
    // This slot runs after QThread::started, so the timer is constructed in
    // the worker thread and its timeout events are delivered there as well.
    timer = new QTimer(this);
    timer->setInterval(monitor::PollIntervalMs);
    connect(timer, &QTimer::timeout, this, &SystemMonitorWorker::poll);
    timer->start();
    poll();
}

void SystemMonitorWorker::poll()
{
    const auto stats = reader();
    if (!stats) {
        emit finished();
        return;
    }

    // Signals carry a value snapshot. The GUI never reads mutable worker
    // state directly, so queued delivery does not require a shared mutex.
    emit statsReady(*stats);
}
