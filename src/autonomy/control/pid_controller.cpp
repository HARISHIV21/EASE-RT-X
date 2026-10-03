#include "pid_controller.hpp"
#include <algorithm>
#include <cmath>
namespace ease_rt_x {
ControlCommand PidController::update(const VehicleState& s,const ControlCommand& target,double dt_s){
    ControlCommand out=target; const double e=target.target_speed_mps-s.velocity_mps;
    integral_=std::clamp(integral_+e*dt_s,-10.0,10.0); const double d=(e-previous_error_)/dt_s; previous_error_=e;
    out.target_speed_mps=std::clamp(s.velocity_mps+0.8*e+0.08*integral_+0.02*d,0.0,15.0);
    if (target.safe_stop)
        out.target_speed_mps = 0;

    return out;
}
}
