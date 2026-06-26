#pragma once

#include <chrono>

namespace rune
{
    /// @brief Print compiler build statistics from the BuildPerformanceTracker
    /// @param buildStart The start time of the build phase
    /// @param includeDetail Whether to print nested profile categories.
    void printBuildStatistics(
        std::chrono::steady_clock::time_point buildStart,
        bool includeDetail = false);
}
