#include "timing_monitor.hpp"

namespace ease_rt_x {

TimingSample TimingMonitor::measure(
    std::uint64_t start_us,
    std::uint64_t completion_us,
    std::uint64_t previous_start_us,
    std::uint64_t scheduled_release_us,
    std::uint64_t expected_period_us,
    std::uint64_t deadline_us
) const {

    TimingSample sample{};

    // Reject invalid timestamp ordering.
    if (completion_us < start_us ||
        start_us < previous_start_us ||
        completion_us < scheduled_release_us) {
        return sample;
    }

    // Actual time spent executing the task.
    sample.execution_time_us = completion_us - start_us;

    // Time between consecutive task starts.
    sample.actual_period_us = start_us - previous_start_us;

    // Absolute difference between actual and expected period.
    sample.jitter_us =
        (sample.actual_period_us >= expected_period_us)
        ? sample.actual_period_us - expected_period_us
        : expected_period_us - sample.actual_period_us;

    // Maximum allowed completion time relative to scheduled release.
    sample.deadline_us = deadline_us;

    // A deadline is missed if completion exceeds the deadline.
    sample.deadline_missed =
        (completion_us - scheduled_release_us) > deadline_us;

    return sample;
}

}