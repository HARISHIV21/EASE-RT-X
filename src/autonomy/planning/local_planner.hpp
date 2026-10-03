#pragma once
#include "../../common/system_types.hpp"
#include "../perception/lidar_perception.hpp"
namespace ease_rt_x {
class LocalPlanner {
public: ControlCommand plan(const VehicleState&, const PerceptionResult&, const Waypoint&) const;
};
}
