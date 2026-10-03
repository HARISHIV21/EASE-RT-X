#pragma once
#include <cstdint>
namespace ease_rt_x {
enum class TimingFaultState:std::uint8_t{OK,WARNING,DEGRADED,CRITICAL};
class TimingFaultManager{
public:void update(std::uint32_t total_deadline_misses);bool timing_ok()const{return state_==TimingFaultState::OK;}TimingFaultState state()const{return state_;}
private:TimingFaultState state_{TimingFaultState::OK};std::uint32_t previous_total_{0};std::uint32_t healthy_cycles_{0};std::uint32_t warning_threshold_{3};std::uint32_t degraded_threshold_{10};std::uint32_t critical_threshold_{50};std::uint32_t recovery_threshold_{100};
};
}
