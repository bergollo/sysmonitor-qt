#include "core/systemmonitorworker.h"

#include <QCoreApplication>
#include <QSignalSpy>
#include <QTest>
#include <QThread>

#include <atomic>

class SystemMonitorWorkerTest final : public QObject {
    Q_OBJECT

private slots:
    void emitsFirstSnapshot();
    void invokesReaderOnEachPoll();
    void emitsFinishedWhenReaderFails();
    void runsInWorkerThreadAndDeliversQueuedStats();
    void shutsDownWorkerThreadAfterReadFailure();
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

void SystemMonitorWorkerTest::runsInWorkerThreadAndDeliversQueuedStats()
{
    const SystemStats expected{
        .cpuPercent = 25.0,
        .memory = MemoryInfo{.totalKb = 200, .availableKb = 100},
        .temperatureCelsius = std::nullopt,
    };
    std::atomic<QThread *> readerThread{nullptr};
    SystemMonitorWorker worker([&readerThread, expected] {
        readerThread.store(QThread::currentThread(), std::memory_order_release);
        return std::optional<SystemStats>{expected};
    });
    QThread thread;
    QSignalSpy statsReady(&worker, &SystemMonitorWorker::statsReady);

    QObject::connect(&thread, &QThread::started, &worker, &SystemMonitorWorker::start);
    QObject::connect(&worker, &SystemMonitorWorker::statsReady, &thread, &QThread::quit,
                     Qt::DirectConnection);
    worker.moveToThread(&thread);
    thread.start();

    QTRY_VERIFY_WITH_TIMEOUT(statsReady.count() == 1, 2000);
    QVERIFY(thread.wait(2000));
    QCOMPARE(readerThread.load(std::memory_order_acquire), &thread);

    const auto received = qvariant_cast<SystemStats>(statsReady.at(0).at(0));
    QCOMPARE(received.cpuPercent, expected.cpuPercent);
    QCOMPARE(received.memory.totalKb, expected.memory.totalKb);
}

void SystemMonitorWorkerTest::shutsDownWorkerThreadAfterReadFailure()
{
    std::atomic<QThread *> readerThread{nullptr};
    SystemMonitorWorker worker([&readerThread] {
        readerThread.store(QThread::currentThread(), std::memory_order_release);
        return std::optional<SystemStats>{};
    });
    QThread thread;
    QSignalSpy finished(&worker, &SystemMonitorWorker::finished);

    QObject::connect(&thread, &QThread::started, &worker, &SystemMonitorWorker::start);
    QObject::connect(&worker, &SystemMonitorWorker::finished, &thread, &QThread::quit,
                     Qt::DirectConnection);
    worker.moveToThread(&thread);
    thread.start();

    QVERIFY(thread.wait(2000));
    QCOMPARE(finished.count(), 1);
    QCOMPARE(readerThread.load(std::memory_order_acquire), &thread);
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    qRegisterMetaType<SystemStats>("SystemStats");
    SystemMonitorWorkerTest test;
    return QTest::qExec(&test, argc, argv);
}

#include "systemmonitorworker_test.moc"
