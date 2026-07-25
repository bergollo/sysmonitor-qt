#include "systemmonitor.h"

#include <QCoreApplication>
#include <QTimer>

#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    std::optional<CpuTimes> previous;
    int samples = 0;

    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&]() {
        const auto stats = readSystemStats(previous);
        if (!stats) {
            qWarning() << "Unable to read system statistics";
            app.quit();
            return;
        }

        previous = readCpuTimes();
        qDebug().noquote()
            << QString("CPU: %1% | Memory: %2/%3 MiB | Temperature: %4")
                   .arg(stats->cpuPercent, 0, 'f', 1)
                   .arg((stats->memory.totalKb - stats->memory.availableKb) / 1024)
                   .arg(stats->memory.totalKb / 1024)
                   .arg(stats->temperatureCelsius
                            ? QString::number(*stats->temperatureCelsius, 'f', 1) + " C"
                            : "unavailable");

        if (++samples >= 3) {
            app.quit();
        }
    });

    timer.start(1000);
    return app.exec();
}
