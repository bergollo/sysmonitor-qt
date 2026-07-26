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
    const CpuTimes previous{100, 0, 100, 700, 0, 0, 0, 0};
    const CpuTimes current{120, 0, 120, 740, 0, 0, 0, 0};
    QCOMPARE(cpuUsagePercent(previous, current), 50.0);
}

void SystemStatsTest::rejectsMalformedInput()
{
    QTemporaryDir directory;
    const auto path = directory.filePath("stat");
    std::ofstream(path.toStdString()) << "not-cpu 1 2 3\n";
    QVERIFY(!readCpuTimes(path.toStdString()).has_value());
}

QTEST_MAIN(SystemStatsTest)
#include "systemstats_test.moc"
