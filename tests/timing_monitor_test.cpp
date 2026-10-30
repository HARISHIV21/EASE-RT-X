#include <cassert>
#include <cstdint>
#include <iostream>

#include "embedded/rtos/timing_monitor.hpp"

using namespace ease_rt_x;

int main() {

    TimingMonitor monitor;

    // Test 1: Execution time, period and jitter.
    {
        const auto result = monitor.measure(
            12000,  // Current task start
            14500,  // Completion
            2000,   // Previous task start
            12000,  // Scheduled release
            10000,  // Expected period
            10000   // Deadline budget
        );

        assert(result.execution_time_us == 2500);
        assert(result.actual_period_us == 10000);
        assert(result.jitter_us == 0);
        assert(result.deadline_missed == false);

        std::cout << "[PASS] Execution time, period and jitter\n";
    }

    // Test 2: Positive jitter.
    {
        const auto result = monitor.measure(
            13000, 16000, 2000, 13000, 10000, 10000
        );

        assert(result.actual_period_us == 11000);
        assert(result.jitter_us == 1000);

        std::cout << "[PASS] Positive period jitter\n";
    }

    // Test 3: Completion exactly at deadline is not a miss.
    {
        const auto result = monitor.measure(
            1000, 11000, 0, 1000, 10000, 10000
        );

        assert(result.deadline_missed == false);

        std::cout << "[PASS] Exact deadline boundary\n";
    }

    // Test 4: Completion after deadline is a miss.
    {
        const auto result = monitor.measure(
            1000, 11001, 0, 1000, 10000, 10000
        );

        assert(result.deadline_missed == true);

        std::cout << "[PASS] Deadline miss detection\n";
    }

    // Test 5: Invalid timestamp ordering.
    {
        const auto result = monitor.measure(
            5000,  // Start
            4000,  // Invalid: completion before start
            1000,
            5000,
            10000,
            10000
        );

        assert(result.execution_time_us == 0);
        assert(result.deadline_missed == false);

        std::cout << "[PASS] Invalid timestamp rejection\n";
    }

    std::cout << "\nAll timing monitor tests passed.\n";

    return 0;
}