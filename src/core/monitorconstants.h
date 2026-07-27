#pragma once

#include <cstddef>

namespace monitor {

inline constexpr int PollIntervalMs = 1000;
inline constexpr std::size_t MaxCpuHistorySamples = 60;

static_assert(PollIntervalMs > 0);
static_assert(MaxCpuHistorySamples > 0);

}
