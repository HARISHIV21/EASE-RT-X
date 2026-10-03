#include "safety_supervisor.hpp"
namespace ease_rt_x {
void SafetySupervisor::update(const SystemHealth& h,const FaultEvent* f){
    if(f && f->active){ if(f->severity==FaultSeverity::CRITICAL){mode_=SystemMode::FAIL_SAFE;return;} if(f->severity==FaultSeverity::DEGRADED||f->severity==FaultSeverity::RECOVERABLE){mode_=SystemMode::DEGRADED;recovery_requested_=false;return;} }
    const bool healthy=h.communication_ok&&h.localization_ok&&h.control_ok&&h.timing_ok&&h.watchdog_ok&&h.sensor_ok;
    if(!healthy){ if(mode_!=SystemMode::FAIL_SAFE)mode_=SystemMode::DEGRADED; return; }
    switch(mode_){case SystemMode::BOOT:mode_=SystemMode::SELF_TEST;break;case SystemMode::SELF_TEST:mode_=SystemMode::READY;break;case SystemMode::READY:mode_=SystemMode::NORMAL;break;case SystemMode::DEGRADED:mode_=SystemMode::RECOVERY;break;case SystemMode::RECOVERY:mode_=SystemMode::NORMAL;break;case SystemMode::NORMAL:break;case SystemMode::FAIL_SAFE: if(recovery_requested_) {mode_=SystemMode::RECOVERY;recovery_requested_=false;} break;}
}
}
