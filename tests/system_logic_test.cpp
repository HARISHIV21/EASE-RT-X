#include <cassert>
#include <cmath>
#include <iostream>
#include "common/system_types.hpp"
#include "embedded/safety/timing_fault_manager.hpp"
#include "embedded/safety/safety_supervisor.hpp"
#include "embedded/diagnostics/diagnostic_manager.hpp"
#include "simulation/sensor_simulator.hpp"
#include "autonomy/estimation/localization.hpp"
#include "autonomy/perception/lidar_perception.hpp"
#include "autonomy/planning/local_planner.hpp"
#include "autonomy/control/pid_controller.hpp"

using namespace ease_rt_x;

int main()
{
    TimingFaultManager timing;
    timing.update(0);
    assert(timing.state() == TimingFaultState::OK);
    timing.update(3);
    assert(timing.state() == TimingFaultState::WARNING);
    for (unsigned i = 4; i <= 10; ++i) timing.update(i);
    assert(timing.state() == TimingFaultState::DEGRADED);
    for (unsigned i = 11; i <= 50; ++i) timing.update(i);
    assert(timing.state() == TimingFaultState::CRITICAL);

    SafetySupervisor safety;
    SystemHealth healthy{};
    healthy.communication_ok = true;
    healthy.localization_ok = true;
    healthy.control_ok = true;
    healthy.timing_ok = true;
    healthy.watchdog_ok = true;
    healthy.sensor_ok = true;
    safety.update(healthy, nullptr);
    safety.update(healthy, nullptr);
    safety.update(healthy, nullptr);
    assert(safety.mode() == SystemMode::NORMAL);

    FaultEvent critical{0x1, FaultSeverity::CRITICAL, FaultType::TIMING, 10, true};
    safety.update(healthy, &critical);
    assert(safety.mode() == SystemMode::FAIL_SAFE);
    safety.request_recovery();
    safety.update(healthy, nullptr);
    assert(safety.mode() == SystemMode::RECOVERY);
    safety.update(healthy, nullptr);
    assert(safety.mode() == SystemMode::NORMAL);

    SensorSimulator sensors;
    auto normal = sensors.step(0, 8.0, 0.0);
    Localization localization;
    auto state = localization.update(normal.imu, normal.gnss, normal.wheel, 0.02);
    assert(state.gnss.valid);
    assert(state.confidence > 0.5);

    sensors.set_fault(SensorFault::GNSS_DROPOUT);
    auto degraded = sensors.step(20, 8.0, 0.0);
    auto fallback = localization.update(degraded.imu, degraded.gnss, degraded.wheel, 0.02);
    assert(!fallback.gnss.valid);
    assert(fallback.wheel_odometry.valid);

    LidarPerception perception;
    auto lidar = perception.process(normal.lidar);
    assert(lidar.valid);
    assert(lidar.obstacle_detected == false);

    LocalPlanner planner;
    Waypoint waypoint{100.0, 0.0, 8.0};
    auto command = planner.plan(fallback, lidar, waypoint);
    assert(command.target_speed_mps <= 8.0);

    PidController controller;
    auto controlled = controller.update(fallback, command, 0.02);
    assert(controlled.target_speed_mps >= 0.0 && controlled.target_speed_mps <= 15.0);

    DiagnosticManager diagnostics;
    FaultEvent fault{0x3001, FaultSeverity::DEGRADED, FaultType::SENSOR, 100, true};
    diagnostics.record(fault);
    diagnostics.record(fault);
    const auto* record = diagnostics.find(0x3001);
    assert(record != nullptr);
    assert(record->occurrences == 2);
    assert(record->confirmed);

    std::cout << "EASE-RT X system logic verification: PASS\n";
    return 0;
}
