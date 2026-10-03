cmake_minimum_required(VERSION 3.20)
project(EASE_RT_X VERSION 1.0.0 LANGUAGES C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/FreeRTOS-Kernel/CMakeLists.txt")
  message(FATAL_ERROR "FreeRTOS-Kernel missing. Run setup.ps1 first.")
endif()
add_library(freertos_config INTERFACE)
target_include_directories(freertos_config SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/config)
target_compile_definitions(freertos_config INTERFACE projCOVERAGE_TEST=0)
set(FREERTOS_PORT MSVC_MINGW CACHE STRING "FreeRTOS port")
add_subdirectory(external/FreeRTOS-Kernel ${CMAKE_BINARY_DIR}/freertos)
add_executable(ease_rt_x
 src/main.cpp
 src/autonomy/estimation/localization.cpp
 src/autonomy/perception/lidar_perception.cpp
 src/autonomy/planning/local_planner.cpp
 src/autonomy/control/pid_controller.cpp
 src/simulation/sensor_simulator.cpp
 src/communication/canfd_protocol.cpp
 src/communication/canfd_interface.cpp
 src/embedded/rtos/timing_monitor.cpp
 src/embedded/rtos/health_monitor.cpp
 src/embedded/safety/timing_fault_manager.cpp
 src/embedded/safety/safety_supervisor.cpp
 src/embedded/diagnostics/diagnostic_manager.cpp
 external/FreeRTOS-Kernel/portable/MemMang/heap_4.c)
target_include_directories(ease_rt_x PRIVATE src)
target_link_libraries(ease_rt_x PRIVATE freertos_kernel)
add_executable(canfd_protocol_test tests/canfd_protocol_test.cpp src/communication/canfd_protocol.cpp src/communication/canfd_interface.cpp)
target_include_directories(canfd_protocol_test PRIVATE src)

add_executable(system_logic_test
    tests/system_logic_test.cpp
    src/autonomy/estimation/localization.cpp
    src/autonomy/perception/lidar_perception.cpp
    src/autonomy/planning/local_planner.cpp
    src/autonomy/control/pid_controller.cpp
    src/simulation/sensor_simulator.cpp
    src/embedded/safety/timing_fault_manager.cpp
    src/embedded/safety/safety_supervisor.cpp
    src/embedded/diagnostics/diagnostic_manager.cpp)
target_include_directories(system_logic_test PRIVATE src)

enable_testing()
add_test(NAME canfd_protocol_test COMMAND canfd_protocol_test)
add_test(NAME system_logic_test COMMAND system_logic_test)


add_test(NAME integrated_system_test COMMAND ease_rt_x --test)

