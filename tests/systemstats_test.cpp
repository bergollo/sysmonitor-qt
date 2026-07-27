#include "core/systemstats.h"
#include "platform/linux/procfs.h"

#include <QTemporaryDir>
#include <QTest>

#include <fstream>

class SystemStatsTest final : public QObject {
    Q_OBJECT

private slots:
    void parsesCpuTimes();
    void parsesMemoryInfo();
    void parsesThermalZone();
    void calculatesCpuDelta();
    void rejectsMalformedInput();
    void comparesValueSnapshots();
};

void SystemStatsTest::parsesCpuTimes()
{
    QTemporaryDir directory;
    const auto path = directory.filePath("stat");
    std::ofstream(path.toStdString()) << "cpu 10 20 30 40 5 6 7 8\n"
                                      << "cpu0 1 2 3 4 5 6 7 8\n";

    const auto result = readCpuTimes(path.toStdString());
    QVERIFY(result.has_value());
    QCOMPARE(result->user, 10u);
    QCOMPARE(result->idle, 40u);
    QCOMPARE(result->steal, 8u);
}

void SystemStatsTest::parsesMemoryInfo()
{
    QTemporaryDir directory;
    const auto path = directory.filePath("meminfo");
    std::ofstream(path.toStdString()) << "MemTotal:       4096000 kB\n"
                                      << "MemAvailable:   2048000 kB\n";

    const auto result = readMemoryInfo(path.toStdString());
    QVERIFY(result.has_value());
    QCOMPARE(result->totalKb, 4096000u);
    QCOMPARE(result->availableKb, 2048000u);
}

void SystemStatsTest::parsesThermalZone()
{
    QTemporaryDir directory;
    const auto zone = directory.filePath("thermal_zone7");
    QVERIFY(QDir().mkpath(zone));
    std::ofstream(zone.toStdString() + "/temp") << "55000\n";

    const auto result = readThermalZone(directory.path().toStdString());
    QVERIFY(result.has_value());
    QCOMPARE(*result, 55.0);
}

void SystemStatsTest::calculatesCpuDelta()
{
    const CpuTimes previous{
        .user = 100,
        .system = 100,
        .idle = 700,
    };
    const CpuTimes current{
        .user = 120,
        .system = 120,
        .idle = 740,
    };
    QCOMPARE(cpuUsagePercent(previous, current), 50.0);
}

void SystemStatsTest::rejectsMalformedInput()
{
    QTemporaryDir directory;
    const auto path = directory.filePath("stat");
    std::ofstream(path.toStdString()) << "not-cpu 1 2 3\n";
    QVERIFY(!readCpuTimes(path.toStdString()).has_value());
}

void SystemStatsTest::comparesValueSnapshots()
{
    const CpuTimes actual{
        .user = 10,
        .nice = 20,
        .system = 30,
        .idle = 40,
        .iowait = 5,
        .irq = 6,
        .softirq = 7,
        .steal = 8,
    };
    const CpuTimes expected{10, 20, 30, 40, 5, 6, 7, 8};
    QVERIFY(actual == expected);
}

QTEST_MAIN(SystemStatsTest)
#include "systemstats_test.moc"
