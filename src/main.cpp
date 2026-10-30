
#include <iostream>
#include <cstdint>
#include <string>
#include <chrono>
#include <limits>

extern "C" {
#include "FreeRTOS.h"
#include "task.h"
}

#include "common/system_types.hpp"
#include "embedded/rtos/timing_monitor.hpp"
#include "embedded/rtos/health_monitor.hpp"
#include "embedded/safety/timing_fault_manager.hpp"
#include "embedded/safety/safety_supervisor.hpp"
#include "embedded/diagnostics/diagnostic_manager.hpp"
#include "communication/canfd_interface.hpp"
#include "simulation/sensor_simulator.hpp"
#include "autonomy/estimation/localization.hpp"
#include "autonomy/perception/lidar_perception.hpp"
#include "autonomy/planning/local_planner.hpp"
#include "autonomy/control/pid_controller.hpp"

using namespace ease_rt_x;

// ============================================================
// SYSTEM COMPONENTS
// ============================================================

static TimingMonitor timing_monitor;
static HealthMonitor health_monitor;
static TimingFaultManager timing_faults;
static SafetySupervisor safety;
static DiagnosticManager diagnostics;
static CanFdInterface canfd;

static SensorSimulator sensors;
static Localization localization;
static LidarPerception perception;
static LocalPlanner planner;
static PidController controller;

// ============================================================
// TIMING STATISTICS
// ============================================================

struct Stats {
    std::uint32_t executions = 0;
    std::uint32_t deadline_misses = 0;

    std::uint64_t max_exec_us = 0;
    std::uint64_t min_period_us = UINT64_MAX;
    std::uint64_t max_period_us = 0;
    std::uint64_t max_jitter_us = 0;
};

static Stats control_stats;
static Stats comm_stats;
static Stats autonomy_stats;
static Stats safety_stats;

static bool integration_test_mode = false;

static std::uint32_t previous_deadline_misses = 0;

static volatile std::uint64_t sim_time_ms = 0;

// ============================================================
// MONOTONIC MICROSECOND CLOCK
// ============================================================

static std::uint64_t now_us() {

    const auto now = std::chrono::steady_clock::now();

    return static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::microseconds>(
            now.time_since_epoch()
        ).count()
    );
}

// ============================================================
// SIMULATED WORKLOAD
// ============================================================

static void work(std::uint32_t n) {

    volatile std::uint32_t x = 0;

    for (std::uint32_t i = 0; i < n; ++i) {
        x += (i * 3U) ^ (i >> 2U);
    }

    (void)x;
}

// ============================================================
// UPDATE TIMING STATISTICS
// ============================================================

static void stats(
    Stats& s,
    const TimingSample& t,
    bool period_valid
) {

    ++s.executions;

    // Ignore the first sample because it has no previous start.
    if (period_valid) {

        if (t.actual_period_us < s.min_period_us) {
            s.min_period_us = t.actual_period_us;
        }

        if (t.actual_period_us > s.max_period_us) {
            s.max_period_us = t.actual_period_us;
        }

        if (t.jitter_us > s.max_jitter_us) {
            s.max_jitter_us = t.jitter_us;
        }
    }

    if (t.deadline_missed) {
        ++s.deadline_misses;
    }

    if (t.execution_time_us > s.max_exec_us) {
        s.max_exec_us = t.execution_time_us;
    }
}

// ============================================================
// PRINT TIMING STATISTICS
// ============================================================

static void print_stats(
    const char* name,
    const Stats& s
) {

    std::cout
        << name
        << " | executions=" << s.executions
        << " | deadline_misses=" << s.deadline_misses
        << " | max_exec_us=" << s.max_exec_us
        << " | period_us=["
        << (s.min_period_us == UINT64_MAX
                ? 0
                : s.min_period_us)
        << ", "
        << s.max_period_us
        << "]"
        << " | max_jitter_us="
        << s.max_jitter_us
        << "\n";
}

// ============================================================
// SYSTEM MODE NAME
// ============================================================

static const char* mode_name(SystemMode m) {

    switch (m) {

        case SystemMode::BOOT:
            return "BOOT";

        case SystemMode::SELF_TEST:
            return "SELF_TEST";

        case SystemMode::READY:
            return "READY";

        case SystemMode::NORMAL:
            return "NORMAL";

        case SystemMode::DEGRADED:
            return "DEGRADED";

        case SystemMode::RECOVERY:
            return "RECOVERY";

        case SystemMode::FAIL_SAFE:
            return "FAIL_SAFE";
    }

    return "UNKNOWN";
}

// ============================================================
// TIMING FAULT STATE NAME
// ============================================================

static const char* timing_name(TimingFaultState s) {

    switch (s) {

        case TimingFaultState::OK:
            return "OK";

        case TimingFaultState::WARNING:
            return "WARNING";

        case TimingFaultState::DEGRADED:
            return "DEGRADED";

        case TimingFaultState::CRITICAL:
            return "CRITICAL";
    }

    return "UNKNOWN";
}

// ============================================================
// COMMUNICATION TASK - 5 ms
// ============================================================

void CommunicationTask(void*) {

    constexpr std::uint64_t PERIOD_US = 5000;
    constexpr std::uint64_t DEADLINE_US = 5000;

    TickType_t wake = xTaskGetTickCount();

    std::uint64_t previous_start_us = 0;

    bool first = true;

    std::uint16_t seq = 0;

    for (;;) {

        const std::uint64_t start_us = now_us();

        CanFdFrame frame{};

        frame.id = static_cast<std::uint32_t>(
            CanFdMessageId::VEHICLE_STATE
        );

        frame.length = 16;

        frame.timestamp_ms =
            static_cast<std::uint32_t>(sim_time_ms);

        frame.sequence = seq++;

        canfd.process(
            frame,
            static_cast<std::uint32_t>(sim_time_ms),
            20
        );

        // Inject stale CAN-FD frames.
        if (
            frame.sequence == 250 ||
            frame.sequence == 500 ||
            frame.sequence == 750 ||
            frame.sequence == 1000
        ) {

            CanFdFrame fault = frame;

            fault.timestamp_ms -= 100;

            canfd.process(
                fault,
                static_cast<std::uint32_t>(sim_time_ms),
                20
            );
        }

        work(25);

        const std::uint64_t completion_us = now_us();

        const auto timing = timing_monitor.measure(
            start_us,
            completion_us,
            previous_start_us,
            start_us,
            PERIOD_US,
            DEADLINE_US
        );

        stats(comm_stats, timing, !first);

        first = false;

        previous_start_us = start_us;

        health_monitor.heartbeat(
            "Communication",
            xTaskGetTickCount()
        );

        vTaskDelayUntil(
            &wake,
            pdMS_TO_TICKS(5)
        );
    }
}

// ============================================================
// AUTONOMY TASK - 20 ms
// ============================================================

void AutonomyTask(void*) {

    constexpr std::uint64_t PERIOD_US = 20000;
    constexpr std::uint64_t DEADLINE_US = 20000;

    TickType_t wake = xTaskGetTickCount();

    std::uint64_t previous_start_us = 0;

    bool first = true;

    std::uint32_t cycle = 0;

    for (;;) {

        const std::uint64_t start_us = now_us();

        ++cycle;

        // Sensor acquisition.
        auto frame = sensors.step(
            sim_time_ms,
            8.0,
            localization.state().heading_rad
        );

        // State estimation.
        auto state = localization.update(
            frame.imu,
            frame.gnss,
            frame.wheel,
            0.02
        );

        // LiDAR perception.
        auto perception_result =
            perception.process(frame.lidar);

        // Path planning.
        Waypoint waypoint{100, 0, 8};

        auto target = planner.plan(
            state,
            perception_result,
            waypoint
        );

        // Flight control.
        auto command = controller.update(
            state,
            target,
            0.02
        );

        (void)command;

        work(180);

        const std::uint64_t completion_us = now_us();

        const auto timing = timing_monitor.measure(
            start_us,
            completion_us,
            previous_start_us,
            start_us,
            PERIOD_US,
            DEADLINE_US
        );

        stats(autonomy_stats, timing, !first);

        first = false;

        previous_start_us = start_us;

        health_monitor.heartbeat(
            "Autonomy",
            xTaskGetTickCount()
        );

        // Sensor fault injection.
        if (cycle == 250) {
            sensors.set_fault(SensorFault::GNSS_DROPOUT);
        }

        if (cycle == 450) {
            sensors.set_fault(SensorFault::NONE);
        }

        if (cycle == 700) {
            sensors.set_fault(SensorFault::LIDAR_INVALID);
        }

        if (cycle == 850) {
            sensors.set_fault(SensorFault::NONE);
        }

        sim_time_ms += 20;

        vTaskDelayUntil(
            &wake,
            pdMS_TO_TICKS(20)
        );
    }
}

// ============================================================
// CONTROL TASK - 10 ms
// ============================================================

void ControlTask(void*) {

    constexpr std::uint64_t PERIOD_US = 10000;
    constexpr std::uint64_t DEADLINE_US = 10000;

    TickType_t wake = xTaskGetTickCount();

    std::uint64_t previous_start_us = 0;

    bool first = true;

    std::uint32_t cycle = 0;

    for (;;) {

        const std::uint64_t start_us = now_us();

        ++cycle;

        // Simulated control computation.
        work(1200);

        // Intentional timing-fault injection.
        if (cycle >= 600 && cycle < 660) {

            vTaskDelay(
                pdMS_TO_TICKS(15)
            );
        }

        const std::uint64_t completion_us = now_us();

        const auto timing = timing_monitor.measure(
            start_us,
            completion_us,
            previous_start_us,
            start_us,
            PERIOD_US,
            DEADLINE_US
        );

        stats(control_stats, timing, !first);

        first = false;

        previous_start_us = start_us;

        // Simulated heartbeat failure.
        if (cycle < 600 || cycle >= 660) {

            health_monitor.heartbeat(
                "Control",
                xTaskGetTickCount()
            );
        }

        vTaskDelayUntil(
            &wake,
            pdMS_TO_TICKS(10)
        );
    }
}

// ============================================================
// SAFETY TASK - 10 ms
// ============================================================

void SafetyTask(void*) {

    constexpr std::uint64_t PERIOD_US = 10000;
    constexpr std::uint64_t DEADLINE_US = 10000;

    TickType_t wake = xTaskGetTickCount();

    std::uint64_t previous_start_us = 0;

    bool first = true;

    for (;;) {

        const std::uint64_t start_us = now_us();

        work(500);

        const std::uint64_t completion_us = now_us();

        const auto timing = timing_monitor.measure(
            start_us,
            completion_us,
            previous_start_us,
            start_us,
            PERIOD_US,
            DEADLINE_US
        );

        stats(safety_stats, timing, !first);

        first = false;

        previous_start_us = start_us;

        health_monitor.heartbeat(
            "Safety",
            xTaskGetTickCount()
        );

        vTaskDelayUntil(
            &wake,
            pdMS_TO_TICKS(10)
        );
    }
}

// ============================================================
// MONITORING TASK - 100 ms
// ============================================================

void MonitoringTask(void*) {

    TickType_t wake = xTaskGetTickCount();

    for (;;) {

        const auto now = xTaskGetTickCount();

        health_monitor.update(now);

        const bool tasks_ok =
            health_monitor.all_healthy();

        const auto new_misses =
            control_stats.deadline_misses -
            previous_deadline_misses;

        previous_deadline_misses =
            control_stats.deadline_misses;

        timing_faults.update(
            control_stats.deadline_misses
        );

        SystemHealth health{};

        health.mode = safety.mode();

        health.communication_ok =
            canfd.healthy();

        health.localization_ok =
            localization.state().confidence > 0.5;

        health.control_ok =
            control_stats.executions > 0;

        health.timing_ok =
            timing_faults.timing_ok();

        health.watchdog_ok =
            tasks_ok;

        health.sensor_ok =
            sensors.healthy();

        if (new_misses > 0) {

            FaultEvent fault{
                0x3001,

                timing_faults.state() ==
                    TimingFaultState::CRITICAL
                    ? FaultSeverity::CRITICAL
                    : (
                        timing_faults.state() ==
                            TimingFaultState::WARNING
                        ? FaultSeverity::WARNING
                        : FaultSeverity::DEGRADED
                    ),

                FaultType::TIMING,

                sim_time_ms,

                true
            };

            diagnostics.record(fault);

            safety.update(health, &fault);
        }

        else {

            if (
                timing_faults.state() ==
                    TimingFaultState::OK &&
                safety.mode() ==
                    SystemMode::FAIL_SAFE
            ) {
                safety.request_recovery();
            }

            safety.update(health, nullptr);
        }

        // System report.
        std::cout
            << "\n=== EASE-RT X SYSTEM REPORT ===\n"

            << "MODE | "
            << mode_name(safety.mode())
            << " ("
            << static_cast<int>(safety.mode())
            << ")\n"

            << "RTOS | all_tasks_healthy="
            << (tasks_ok ? "YES" : "NO")
            << "\n"

            << "TIMING | state="
            << timing_name(timing_faults.state())
            << " | new_misses="
            << new_misses
            << " | total_misses="
            << control_stats.deadline_misses
            << "\n";

        print_stats("Communication", comm_stats);
        print_stats("Autonomy", autonomy_stats);
        print_stats("Safety", safety_stats);
        print_stats("Control", control_stats);

        const auto& can_stats = canfd.statistics();

        std::cout
            << "CAN-FD | received="
            << can_stats.received
            << " | accepted="
            << can_stats.accepted
            << " | rejected="
            << can_stats.rejected
            << " | consecutive_rejections="
            << canfd.consecutive_rejections()
            << "\n";

        std::cout
            << "DIAGNOSTICS | records="
            << diagnostics.size()
            << "\n";

        // Existing integration-test completion condition.
               // Integration test passes only after the complete fault response.
        if (
            integration_test_mode &&
            control_stats.executions >= 660 &&
            control_stats.deadline_misses >= 50 &&
            timing_faults.state() == TimingFaultState::CRITICAL &&
            diagnostics.size() >= 1 &&
            safety.mode() == SystemMode::FAIL_SAFE
        ) {
            std::cout
                << "\nINTEGRATION TEST: PASS\n"
                << "Control deadline misses: "
                << control_stats.deadline_misses << "\n"
                << "Timing state: CRITICAL\n"
                << "Diagnostics recorded: "
                << diagnostics.size() << "\n"
                << "Safety mode: FAIL_SAFE\n";

            std::cout.flush();
            vTaskEndScheduler();
            return;
        }

// ============================================================
// FREERTOS ERROR HOOKS
// ============================================================
}}
extern "C" void vApplicationMallocFailedHook(void) {

    std::cout << "FATAL | malloc failed\n";

    taskDISABLE_INTERRUPTS();

    for (;;) {}
}

extern "C" void vApplicationStackOverflowHook(
    TaskHandle_t,
    char* name
) {

    std::cout
        << "FATAL | stack overflow | task="
        << name
        << "\n";

    taskDISABLE_INTERRUPTS();

    for (;;) {}
}

// ============================================================
// MAIN
// ============================================================

int main(int argc, char** argv) {

    integration_test_mode =
        (argc > 1 && std::string(argv[1]) == "--test");

    std::cout
        << "EASE-RT-X | Integrated Embedded "
        << "Real-Time Autonomous Platform\n";

    health_monitor.register_task("Communication", 50);
    health_monitor.register_task("Autonomy", 50);
    health_monitor.register_task("Safety", 50);
    health_monitor.register_task("Control", 50);

    xTaskCreate(
        CommunicationTask,
        "Communication",
        1024,
        nullptr,
        7,
        nullptr
    );

    xTaskCreate(
        AutonomyTask,
        "Autonomy",
        1536,
        nullptr,
        6,
        nullptr
    );

    xTaskCreate(
        ControlTask,
        "Control",
        1024,
        nullptr,
        8,
        nullptr
    );

    xTaskCreate(
        SafetyTask,
        "Safety",
        1024,
        nullptr,
        9,
        nullptr
    );

    xTaskCreate(
        MonitoringTask,
        "Monitoring",
        1536,
        nullptr,
        2,
        nullptr
    );

    vTaskStartScheduler();

    return 0;
}
