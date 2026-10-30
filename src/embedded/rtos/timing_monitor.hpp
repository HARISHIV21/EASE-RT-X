#pragma once

#include <cstdint>

namespace ease_rt_x {

struct TimingSample {
    std::uint64_t execution_time_us{0};
    std::uint64_t actual_period_us{0};
    std::uint64_t jitter_us{0};
    std::uint64_t deadline_us{0};
    bool deadline_missed{false};
};

class TimingMonitor {
public:
    TimingSample measure(
        std::uint64_t start_us,
        std::uint64_t completion_us,
        std::uint64_t previous_start_us,
        std::uint64_t scheduled_release_us,
        std::uint64_t expected_period_us,
        std::uint64_t deadline_us
    ) const;
};

}