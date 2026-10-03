#pragma once
#include "../../common/system_types.hpp"
namespace ease_rt_x {
class Localization {
public:
    VehicleState update(const ImuSample&, const GnssSample&, const WheelOdometrySample&, double dt_s);
    const VehicleState& state() const { return state_; }
private:
    VehicleState state_{};
};
}
