#pragma once
#include "../../common/system_types.hpp"
namespace ease_rt_x {
struct PerceptionResult { bool valid{false}; bool obstacle_detected{false}; double nearest_obstacle_m{1000}; double free_path_width_m{10}; };
class LidarPerception { public: PerceptionResult process(const LidarSample&) const; };
}
