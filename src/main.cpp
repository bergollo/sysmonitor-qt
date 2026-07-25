#include "systemmonitor.h"

#include <QCoreApplication>
#include <QDebug>
#include <QThread>

namespace {

void printStats(const SystemStats &stats)
{
    qDebug().noquote()
        << QString("CPU: %1% | Memory: %2/%3 MiB | Temperature: %4")
               .arg(stats.cpuPercent, 0, 'f', 1)
               .arg((stats.memory.totalKb - stats.memory.availableKb) / 1024)
               .arg(stats.memory.totalKb / 1024)
               .arg(stats.temperatureCelsius
                        ? QString::number(*stats.temperatureCelsius, 'f', 1) + " C"
                        : "unavailable");
}

}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QThread workerThread;
    SystemMonitorWorker worker;
    worker.moveToThread(&workerThread);

    QObject::connect(&workerThread, &QThread::started, &worker, &SystemMonitorWorker::start);
    QObject::connect(&worker, &SystemMonitorWorker::statsReady, &printStats);
    QObject::connect(&worker, &SystemMonitorWorker::finished, &app, &QCoreApplication::quit);
    QObject::connect(&app, &QCoreApplication::aboutToQuit, &workerThread, &QThread::quit);

    workerThread.start();
    const int result = app.exec();
    workerThread.quit();
    workerThread.wait();
    return result;
}
