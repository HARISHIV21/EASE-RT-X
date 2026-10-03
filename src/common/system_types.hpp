#pragma once
#include <cstdint>

namespace ease_rt_x {

enum class SystemMode : std::uint8_t { BOOT, SELF_TEST, READY, NORMAL, DEGRADED, RECOVERY, FAIL_SAFE };
enum class FaultSeverity : std::uint8_t { INFORMATIONAL, WARNING, RECOVERABLE, DEGRADED, CRITICAL };
enum class FaultType : std::uint8_t { NONE, SENSOR, COMMUNICATION, TIMING, WATCHDOG, ESTIMATION, CONTROL };

struct SensorHealth { bool valid{false}; bool fresh{false}; bool faulted{false}; std::uint32_t age_ms{0}; };
struct SensorSample { std::uint64_t timestamp_ms{0}; double x{0}; double y{0}; double z{0}; SensorHealth health{}; };
struct ImuSample : SensorSample { double gyro_z_rad_s{0}; };
struct GnssSample : SensorSample { double position_x_m{0}; double position_y_m{0}; double velocity_mps{0}; };
struct WheelOdometrySample : SensorSample { double distance_m{0}; double velocity_mps{0}; };
struct LidarSample { std::uint64_t timestamp_ms{0}; bool valid{false}; bool faulted{false}; double nearest_obstacle_m{1000.0}; double free_path_width_m{10.0}; };

struct VehicleState {
    double position_x_m{0}; double position_y_m{0}; double velocity_mps{0}; double heading_rad{0};
    double confidence{0};
    SensorHealth imu, gnss, lidar, wheel_odometry;
};

struct ControlCommand { double steering_rad{0}; double target_speed_mps{0}; bool safe_stop{false}; };

struct FaultEvent {
    std::uint32_t fault_id{0}; FaultSeverity severity{FaultSeverity::INFORMATIONAL};
    FaultType type{FaultType::NONE}; std::uint64_t timestamp_ms{0}; bool active{false};
};

struct SystemHealth {
    SystemMode mode{SystemMode::BOOT};
    bool communication_ok{false}; bool localization_ok{false}; bool control_ok{false};
    bool timing_ok{false}; bool watchdog_ok{false}; bool sensor_ok{false};
};

struct Waypoint { double x_m{0}; double y_m{0}; double speed_mps{0}; };
struct Obstacle { double x_m{0}; double y_m{0}; double radius_m{0.5}; };

}
