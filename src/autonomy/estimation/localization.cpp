#include "localization.hpp"
#include <cmath>
namespace ease_rt_x {
VehicleState Localization::update(const ImuSample& imu,const GnssSample& gnss,const WheelOdometrySample& wheel,double dt_s) {
    state_.imu=imu.health; state_.gnss=gnss.health; state_.wheel_odometry=wheel.health;
    state_.heading_rad += imu.gyro_z_rad_s * dt_s;
    const bool gnss_ok=gnss.health.valid && gnss.health.fresh && !gnss.health.faulted;
    if(gnss_ok){ state_.position_x_m=0.7*state_.position_x_m+0.3*gnss.position_x_m; state_.position_y_m=0.7*state_.position_y_m+0.3*gnss.position_y_m; }
    else { state_.position_x_m += wheel.velocity_mps*dt_s*std::cos(state_.heading_rad); state_.position_y_m += wheel.velocity_mps*dt_s*std::sin(state_.heading_rad); }
    state_.velocity_mps=wheel.health.valid ? wheel.velocity_mps : (gnss_ok ? gnss.velocity_mps : state_.velocity_mps);
    state_.confidence=(gnss_ok?0.7:0.35)+(wheel.health.valid?0.2:0)+(imu.health.valid?0.1:0);
    return state_;
}
}
