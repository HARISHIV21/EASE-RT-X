#pragma once
#include "../common/system_types.hpp"
#include <cstdint>
namespace ease_rt_x {
enum class SensorFault { NONE, GNSS_DROPOUT, LIDAR_INVALID, IMU_BIAS, SENSOR_DELAY };
struct SimSensorFrame { ImuSample imu; GnssSample gnss; WheelOdometrySample wheel; LidarSample lidar; };
class SensorSimulator {
public: SimSensorFrame step(std::uint64_t t_ms,double speed_mps,double heading_rad); void set_fault(SensorFault f){fault_=f;}
    bool healthy() const { return fault_ == SensorFault::NONE; }
    SensorFault fault() const { return fault_; }
private: SensorFault fault_{SensorFault::NONE};
};
}
