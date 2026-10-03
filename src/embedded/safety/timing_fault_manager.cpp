#include "timing_fault_manager.hpp"
namespace ease_rt_x {
void TimingFaultManager::update(std::uint32_t total){
    const bool new_fault=total>previous_total_;
    previous_total_=total;
    if(new_fault){
        if(total>=critical_threshold_) { state_=TimingFaultState::CRITICAL; healthy_cycles_=0; return; }
        if(total>=degraded_threshold_) { state_=TimingFaultState::DEGRADED; healthy_cycles_=0; return; }
        if(total>=warning_threshold_) { state_=TimingFaultState::WARNING; healthy_cycles_=0; return; }
        healthy_cycles_=0; return;
    }
    if(state_!=TimingFaultState::OK && ++healthy_cycles_>=recovery_threshold_){state_=TimingFaultState::OK;healthy_cycles_=0;}
}
}
