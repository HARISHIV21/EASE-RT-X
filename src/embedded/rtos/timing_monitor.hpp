#pragma once
#include <cstdint>
namespace ease_rt_x { struct TimingSample{std::uint32_t execution_time_ticks{0};std::uint32_t period_ticks{0};std::uint32_t deadline_ticks{0};bool deadline_missed{false};}; class TimingMonitor{public:TimingSample measure(std::uint32_t start,std::uint32_t end,std::uint32_t previous,std::uint32_t deadline)const;}; }
