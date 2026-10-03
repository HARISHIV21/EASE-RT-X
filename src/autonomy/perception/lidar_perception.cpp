#include "lidar_perception.hpp"
namespace ease_rt_x { PerceptionResult LidarPerception::process(const LidarSample& s) const { PerceptionResult r; r.valid=s.valid&&!s.faulted; r.nearest_obstacle_m=s.nearest_obstacle_m; r.free_path_width_m=s.free_path_width_m; r.obstacle_detected=r.valid && s.nearest_obstacle_m<8.0; return r; } }
