#include "core/systemstats.h"
#include "platform/linux/procfs.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <tuple>

namespace {

class ProcFixture : public ::testing::Test {
protected:
    void SetUp() override
    {
        directory = std::filesystem::temp_directory_path()
            / ("qt-sysmonitor-test-" + std::to_string(++nextId));
        std::filesystem::create_directories(directory);
    }

    void TearDown() override { std::filesystem::remove_all(directory); }

    std::filesystem::path directory;
    inline static std::uint64_t nextId = 0;
};

TEST_F(ProcFixture, ParsesCpuTimes)
{
    const auto path = directory / "stat";
    std::ofstream(path) << "cpu 10 20 30 40 5 6 7 8\n";

    const auto result = readCpuTimes(path.string());

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->user, 10u);
    EXPECT_EQ(result->idle, 40u);
    EXPECT_EQ(result->steal, 8u);
}

TEST_F(ProcFixture, ParsesMemoryInfo)
{
    const auto path = directory / "meminfo";
    std::ofstream(path) << "MemTotal:       4096000 kB\n"
                         << "MemAvailable:   2048000 kB\n";

    const auto result = readMemoryInfo(path.string());

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->totalKb, 4096000u);
    EXPECT_EQ(result->availableKb, 2048000u);
}

TEST_F(ProcFixture, ParsesThermalZone)
{
    const auto zone = directory / "thermal_zone7";
    std::filesystem::create_directories(zone);
    std::ofstream(zone / "temp") << "55000\n";

    const auto result = readThermalZone(directory.string());

    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(*result, 55.0);
}

TEST_F(ProcFixture, RejectsMalformedCpuInput)
{
    const auto path = directory / "stat";
    std::ofstream(path) << "not-cpu 1 2 3\n";

    EXPECT_FALSE(readCpuTimes(path.string()).has_value());
}

TEST(CpuUsagePercentTest, CalculatesBusyPercentage)
{
    const CpuTimes previous{.user = 100, .system = 100, .idle = 700};
    const CpuTimes current{.user = 120, .system = 120, .idle = 740};

    EXPECT_DOUBLE_EQ(cpuUsagePercent(previous, current), 50.0);
}

class CpuUsagePercentParameterizedTest
    : public ::testing::TestWithParam<std::tuple<CpuTimes, CpuTimes, double>> {
};

TEST_P(CpuUsagePercentParameterizedTest, HandlesCounterEdgeCases)
{
    const auto &[previous, current, expected] = GetParam();
    EXPECT_DOUBLE_EQ(cpuUsagePercent(previous, current), expected);
}

INSTANTIATE_TEST_SUITE_P(
    CounterEdgeCases,
    CpuUsagePercentParameterizedTest,
    ::testing::Values(
        std::tuple{CpuTimes{.user = 10}, CpuTimes{.user = 5}, 0.0},
        std::tuple{CpuTimes{.idle = 10}, CpuTimes{.idle = 5}, 0.0},
        std::tuple{CpuTimes{}, CpuTimes{}, 0.0},
        std::tuple{CpuTimes{.user = 10}, CpuTimes{.user = 20}, 100.0}),
    [](const ::testing::TestParamInfo<
           std::tuple<CpuTimes, CpuTimes, double>> &info) {
        return "Case" + std::to_string(info.index);
    });

TEST(CpuTimesTest, ComparesValueSnapshots)
{
    const CpuTimes actual{10, 20, 30, 40, 5, 6, 7, 8};
    const CpuTimes expected{10, 20, 30, 40, 5, 6, 7, 8};

    EXPECT_EQ(actual, expected);
}

} // namespace
