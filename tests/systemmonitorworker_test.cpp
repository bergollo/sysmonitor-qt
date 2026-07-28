#include "core/systemmonitorworker.h"

#include <QCoreApplication>
#include <QSignalSpy>
#include <QTest>

class SystemMonitorWorkerTest final : public QObject {
    Q_OBJECT

private slots:
    void emitsFirstSnapshot();
    void invokesReaderOnEachPoll();
    void emitsFinishedWhenReaderFails();
};

void SystemMonitorWorkerTest::emitsFirstSnapshot()
{
    const SystemStats expected{
        .cpuPercent = 12.5,
        .memory = MemoryInfo{.totalKb = 100, .availableKb = 25},
        .temperatureCelsius = 45.0,
    };
    SystemMonitorWorker worker([expected] { return std::optional<SystemStats>{expected}; });
    SystemStats received;
    QObject::connect(&worker, &SystemMonitorWorker::statsReady, &worker,
                     [&received](const SystemStats &stats) { received = stats; });

    worker.start();

    QCOMPARE(received.cpuPercent, expected.cpuPercent);
    QCOMPARE(received.memory.totalKb, expected.memory.totalKb);
    QCOMPARE(received.temperatureCelsius, expected.temperatureCelsius);
}

void SystemMonitorWorkerTest::invokesReaderOnEachPoll()
{
    int calls = 0;
    SystemMonitorWorker worker([&calls] {
        ++calls;
        return std::optional<SystemStats>{SystemStats{}};
    });

    worker.start();
    QMetaObject::invokeMethod(&worker, "poll", Qt::DirectConnection);

    QCOMPARE(calls, 2);
}

void SystemMonitorWorkerTest::emitsFinishedWhenReaderFails()
{
    SystemMonitorWorker worker([] { return std::optional<SystemStats>{}; });
    QSignalSpy finished(&worker, &SystemMonitorWorker::finished);

    worker.start();

    QCOMPARE(finished.count(), 1);
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    SystemMonitorWorkerTest test;
    return QTest::qExec(&test, argc, argv);
}

#include "systemmonitorworker_test.moc"
