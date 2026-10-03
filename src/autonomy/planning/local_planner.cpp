#include "local_planner.hpp"
#include <algorithm>
#include <cmath>
namespace ease_rt_x {
ControlCommand LocalPlanner::plan(const VehicleState& s,const PerceptionResult& p,const Waypoint& w) const {
    ControlCommand c{}; const double dx=w.x_m-s.position_x_m, dy=w.y_m-s.position_y_m;
    const double desired=std::atan2(dy,dx); double e=desired-s.heading_rad;
    while (e > 3.141592653589793)
        e -= 6.283185307179586;
    while (e < -3.141592653589793)
        e += 6.283185307179586;
    c.steering_rad=std::clamp(1.2*e,-0.5,0.5); c.target_speed_mps=std::clamp(w.speed_mps,0.0,15.0);
    if(!p.valid || p.obstacle_detected){ c.target_speed_mps=std::min(c.target_speed_mps,2.0); if(p.obstacle_detected&&p.nearest_obstacle_m<2.0)c.safe_stop=true; }
    return c;
}
}
