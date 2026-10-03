#pragma once
#include "../../common/system_types.hpp"
namespace ease_rt_x {
class PidController {
public: ControlCommand update(const VehicleState&, const ControlCommand&, double dt_s);
private: double integral_{0}; double previous_error_{0};
};
}
