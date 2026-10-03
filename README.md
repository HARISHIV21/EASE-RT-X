# EASE-RT X

**Embedded Autonomous Software-Defined Real-Time Platform**

A simulation-first flagship project demonstrating C/C++, FreeRTOS, deterministic periodic execution, CAN-FD, embedded Linux/ROS2 integration points, sensor simulation, localization, LiDAR perception, planning, control, diagnostics, fault detection, degraded/fail-safe behavior and quantitative verification.

## Core technologies

C, C++, FreeRTOS, CMake, CAN-FD protocol modelling, Linux/ROS2 integration point, Python verification tooling.

## Demonstrated fault classes

- GNSS dropout / delayed / invalid sensor data
- LiDAR invalidity
- CAN-FD stale frames and sequence errors
- control deadline violations
- task heartbeat/watchdog timeout
- timing degradation and recovery

## Build

Run `setup.ps1` from PowerShell. It obtains FreeRTOS-Kernel if absent, configures MinGW Makefiles and builds the runtime plus verification tests. The CTest suite contains both CAN-FD protocol verification and integrated system-logic verification.

## Evidence

The runtime prints system mode, RTOS health, timing state, task execution/deadline statistics, CAN-FD acceptance/rejection statistics and diagnostic record count. These values are runtime measurements and should be preserved as project evidence rather than invented.

## Final validation

See `docs/FINAL_VALIDATION.md` for the controlled fault campaign and the measurements to preserve as evidence.
