#pragma once
#include "../../common/system_types.hpp"
namespace ease_rt_x {
class SafetySupervisor {
public:
    void update(const SystemHealth& health,const FaultEvent* active_fault);
    SystemMode mode() const{return mode_;}
    void request_recovery(){recovery_requested_=true;}
private:
    SystemMode mode_{SystemMode::BOOT}; bool recovery_requested_{false};
};
}
