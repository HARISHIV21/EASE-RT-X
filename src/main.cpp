#include <iostream>
#include <cstdint>
#include <string>
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
static TimingMonitor timing_monitor; static HealthMonitor health_monitor; static TimingFaultManager timing_faults; static SafetySupervisor safety; static DiagnosticManager diagnostics; static CanFdInterface canfd;
static SensorSimulator sensors; static Localization localization; static LidarPerception perception; static LocalPlanner planner; static PidController controller;
struct Stats{std::uint32_t executions=0,deadline_misses=0,max_exec=0,min_period=UINT32_MAX,max_period=0;};
static Stats control_stats,comm_stats,autonomy_stats,safety_stats; static bool integration_test_mode=false; static std::uint32_t previous_deadline_misses=0;
static volatile std::uint64_t sim_time_ms=0;
static void work(std::uint32_t n){volatile std::uint32_t x=0;for(std::uint32_t i=0;i<n;++i)x+=(i*3U)^(i>>2U);(void)x;}
static void stats(Stats&s,const TimingSample&t,bool period){++s.executions;if(period){if(t.period_ticks<s.min_period)s.min_period=t.period_ticks;if(t.period_ticks>s.max_period)s.max_period=t.period_ticks;}if(t.deadline_missed)++s.deadline_misses;if(t.execution_time_ticks>s.max_exec)s.max_exec=t.execution_time_ticks;}
static void print_stats(const char*n,const Stats&s){std::cout<<n<<" | executions="<<s.executions<<" | deadline_misses="<<s.deadline_misses<<" | max_exec_ticks="<<s.max_exec<<" | period_ticks=["<<(s.min_period==UINT32_MAX?0:s.min_period)<<", "<<s.max_period<<"]\n";}
static const char* mode_name(SystemMode m){switch(m){case SystemMode::BOOT:return"BOOT";case SystemMode::SELF_TEST:return"SELF_TEST";case SystemMode::READY:return"READY";case SystemMode::NORMAL:return"NORMAL";case SystemMode::DEGRADED:return"DEGRADED";case SystemMode::RECOVERY:return"RECOVERY";case SystemMode::FAIL_SAFE:return"FAIL_SAFE";}return"UNKNOWN";}
static const char* timing_name(TimingFaultState s){switch(s){case TimingFaultState::OK:return"OK";case TimingFaultState::WARNING:return"WARNING";case TimingFaultState::DEGRADED:return"DEGRADED";case TimingFaultState::CRITICAL:return"CRITICAL";}return"UNKNOWN";}

void CommunicationTask(void*){TickType_t wake=xTaskGetTickCount(),prev=0;bool first=true;std::uint16_t seq=0;std::uint32_t fault_frames=0;for(;;){auto start=xTaskGetTickCount();CanFdFrame f{};f.id=static_cast<std::uint32_t>(CanFdMessageId::VEHICLE_STATE);f.length=16;f.timestamp_ms=static_cast<std::uint32_t>(sim_time_ms);f.sequence=seq++;auto r=canfd.process(f,static_cast<std::uint32_t>(sim_time_ms),20);if(f.sequence==250||f.sequence==500||f.sequence==750||f.sequence==1000){CanFdFrame fault=f;fault.timestamp_ms-=100;canfd.process(fault,static_cast<std::uint32_t>(sim_time_ms),20);}work(25);auto end=xTaskGetTickCount();auto t=timing_monitor.measure(start,end,prev,pdMS_TO_TICKS(5));stats(comm_stats,t,!first);first=false;prev=start;health_monitor.heartbeat("Communication",end);vTaskDelayUntil(&wake,pdMS_TO_TICKS(5));}}
void AutonomyTask(void*){TickType_t wake=xTaskGetTickCount(),prev=0;bool first=true;std::uint32_t cycle=0;for(;;){auto start=xTaskGetTickCount();++cycle;auto frame=sensors.step(sim_time_ms,8.0,localization.state().heading_rad);auto state=localization.update(frame.imu,frame.gnss,frame.wheel,0.02);auto p=perception.process(frame.lidar);Waypoint w{100,0,8};auto target=planner.plan(state,p,w);auto cmd=controller.update(state,target,0.02);(void)cmd;work(180);auto end=xTaskGetTickCount();auto t=timing_monitor.measure(start,end,prev,pdMS_TO_TICKS(20));stats(autonomy_stats,t,!first);first=false;prev=start;health_monitor.heartbeat("Autonomy",end);if(cycle==250)sensors.set_fault(SensorFault::GNSS_DROPOUT);if(cycle==450)sensors.set_fault(SensorFault::NONE);if(cycle==700)sensors.set_fault(SensorFault::LIDAR_INVALID);if(cycle==850)sensors.set_fault(SensorFault::NONE);sim_time_ms+=20;vTaskDelayUntil(&wake,pdMS_TO_TICKS(20));}}
void ControlTask(void*){TickType_t wake=xTaskGetTickCount(),prev=0;bool first=true;std::uint32_t n=0;for(;;){auto start=xTaskGetTickCount();++n;work(1200);if(n>=600&&n<660)vTaskDelay(pdMS_TO_TICKS(15));auto end=xTaskGetTickCount();auto t=timing_monitor.measure(start,end,prev,pdMS_TO_TICKS(10));stats(control_stats,t,!first);first=false;prev=start;if(n<600||n>=660)health_monitor.heartbeat("Control",end);vTaskDelayUntil(&wake,pdMS_TO_TICKS(10));}}
void SafetyTask(void*){TickType_t wake=xTaskGetTickCount(),prev=0;bool first=true;for(;;){auto start=xTaskGetTickCount();work(500);auto end=xTaskGetTickCount();auto t=timing_monitor.measure(start,end,prev,pdMS_TO_TICKS(10));stats(safety_stats,t,!first);first=false;prev=start;health_monitor.heartbeat("Safety",end);vTaskDelayUntil(&wake,pdMS_TO_TICKS(10));}}
void MonitoringTask(void*){TickType_t wake=xTaskGetTickCount();for(;;){auto now=xTaskGetTickCount();health_monitor.update(now);const bool tasks_ok=health_monitor.all_healthy();const auto new_misses=control_stats.deadline_misses-previous_deadline_misses;previous_deadline_misses=control_stats.deadline_misses;timing_faults.update(control_stats.deadline_misses);SystemHealth h{};h.mode=safety.mode();h.communication_ok=canfd.healthy();h.localization_ok=localization.state().confidence>0.5;h.control_ok=true;h.timing_ok=timing_faults.timing_ok();h.watchdog_ok=tasks_ok;h.sensor_ok=sensors.healthy();if(new_misses){FaultEvent f{0x3001, timing_faults.state()==TimingFaultState::CRITICAL?FaultSeverity::CRITICAL:(timing_faults.state()==TimingFaultState::WARNING?FaultSeverity::WARNING:FaultSeverity::DEGRADED),FaultType::TIMING,sim_time_ms,true};diagnostics.record(f);safety.update(h,&f);}else{if(timing_faults.state()==TimingFaultState::OK && safety.mode()==SystemMode::FAIL_SAFE)safety.request_recovery();safety.update(h,nullptr);}std::cout<<"\n=== EASE-RT X SYSTEM REPORT ===\n"<<"MODE | "<<mode_name(safety.mode())<<" ("<<static_cast<int>(safety.mode())<<")\n"<<"RTOS | all_tasks_healthy="<<(tasks_ok?"YES":"NO")<<"\n"<<"TIMING | state="<<timing_name(timing_faults.state())<<" | new_misses="<<new_misses<<" | total_misses="<<control_stats.deadline_misses<<"\n";print_stats("Communication",comm_stats);print_stats("Autonomy",autonomy_stats);print_stats("Safety",safety_stats);print_stats("Control",control_stats);const auto&cs=canfd.statistics();std::cout<<"CAN-FD | received="<<cs.received<<" | accepted="<<cs.accepted<<" | rejected="<<cs.rejected<<" | consecutive_rejections="<<canfd.consecutive_rejections()<<"\n";std::cout<<"DIAGNOSTICS | records="<<diagnostics.size()<<"\n";if(integration_test_mode && timing_faults.state()==TimingFaultState::OK && safety.mode()==SystemMode::NORMAL && control_stats.deadline_misses>=60 && canfd.statistics().rejected>=4 && diagnostics.size()>=1){vTaskEndScheduler();return;}vTaskDelayUntil(&wake,pdMS_TO_TICKS(100));}}
extern "C" void vApplicationMallocFailedHook(void){std::cout<<"FATAL | malloc failed\n";taskDISABLE_INTERRUPTS();for(;;){}}
extern "C" void vApplicationStackOverflowHook(TaskHandle_t, char*name){std::cout<<"FATAL | stack overflow | task="<<name<<"\n";taskDISABLE_INTERRUPTS();for(;;){}}
int main(int argc,char** argv){integration_test_mode=(argc>1 && std::string(argv[1])=="--test");std::cout<<"EASE-RT X | Integrated Embedded Real-Time Autonomous Platform\n";health_monitor.register_task("Communication",50);health_monitor.register_task("Autonomy",50);health_monitor.register_task("Safety",50);health_monitor.register_task("Control",50);xTaskCreate(CommunicationTask,"Communication",1024,nullptr,7,nullptr);xTaskCreate(AutonomyTask,"Autonomy",1536,nullptr,6,nullptr);xTaskCreate(ControlTask,"Control",1024,nullptr,8,nullptr);xTaskCreate(SafetyTask,"Safety",1024,nullptr,9,nullptr);xTaskCreate(MonitoringTask,"Monitoring",1536,nullptr,2,nullptr);vTaskStartScheduler();return 0;}









