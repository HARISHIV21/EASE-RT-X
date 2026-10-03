#pragma once
#include <cstdint>
namespace ease_rt_x {struct TaskHealth{const char* name;std::uint32_t timeout_ms;std::uint32_t last_heartbeat_tick;bool healthy;};class HealthMonitor{public:void register_task(const char*,std::uint32_t);void heartbeat(const char*,std::uint32_t);void update(std::uint32_t);bool all_healthy()const;private:static constexpr std::uint32_t MAX_TASKS=8;TaskHealth tasks_[MAX_TASKS]{};std::uint32_t count_{0};};}
