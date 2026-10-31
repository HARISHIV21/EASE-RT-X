# EASE-RT-X
### Real-Time Embedded Software Platform for Autonomous Systems

**Embedded C/C++ · FreeRTOS · CMake · CAN-FD · Real-Time Monitoring · Fault Management · Autonomous Systems · Software Verification**

EASE-RT-X is a simulation-first embedded software platform developed to demonstrate the design, implementation and verification of dependable real-time systems for autonomous applications.

The platform combines a FreeRTOS-based execution environment with simulated sensor inputs, modular autonomous processing, CAN-FD communication, timing supervision, fault management, diagnostics and safety recovery.

Its central engineering challenge is:

> How can an embedded autonomous system detect abnormal operating conditions, enter a safe state and recover when normal operating conditions return?

The project emphasizes modular software architecture, observable system behaviour, controlled fault injection and repeatable software verification.

---

## 1. Engineering Objectives

EASE-RT-X demonstrates practical engineering concepts in:

- Real-time task execution and periodic scheduling.
- Modular embedded C/C++ software development.
- Sensor-data processing and autonomous-system pipelines.
- CAN-FD communication and message validation.
- Task health monitoring and timing supervision.
- Deadline-miss detection and fault escalation.
- Safety-state management and recovery logic.
- Diagnostic event recording.
- Unit testing, integration testing and fault-injection validation.

## 2. System Architecture

![EASE-RT-X System Architecture](docs/assets/EASE-RT-X-architecture.png)

The implementation is organized into six functional areas:

| Functional area | Responsibility |
|---|---|
| Real-time execution | FreeRTOS-based task execution |
| Sensor simulation | Simulated IMU, GNSS, LiDAR and wheel odometry |
| Autonomous processing | Localization, perception, planning and PID control |
| Communication | CAN-FD message handling and protocol validation |
| Health and safety | Task monitoring, timing supervision and safety-state management |
| Diagnostics | Diagnostic event recording |

### Architecture principles

- **Modularity:** Software responsibilities are separated into individual components.
- **Testability:** Core functionality is exercised through independent test executables.
- **Separation of concerns:** Autonomy, communication, monitoring and safety logic are maintained as distinct modules.
- **Fault awareness:** Abnormal operating conditions are detected and reflected in system state.
- **Recoverability:** Safety logic supports a controlled transition to normal operation after recovery conditions are satisfied.

The repository also contains a ROS2 autonomy-node source file and a Python sensor-simulation script. These are not currently compiled into the main CMake executable.

## 3. Real-Time Embedded Software

The execution layer uses FreeRTOS and includes periodic software tasks for communication, autonomy, safety supervision and control.

Implemented functionality includes:

- Periodic task execution.
- Task heartbeat reporting.
- Task health monitoring.
- Timing supervision.
- Deadline-miss tracking.
- Timing-state classification.
- Fault escalation.
- Safety supervision.

### Timing supervision

The timing monitor evaluates deadline-miss behaviour using configured thresholds.

| Timing condition | Configured threshold |
|---|---:|
| Warning | 3 misses |
| Degraded | 10 misses |
| Critical | 50 misses |

These thresholds represent software-level monitoring conditions in the host simulation, not hardware timing guarantees.

## 4. Autonomous-System Software

The autonomy components form a modular processing pipeline using simulated sensor inputs.

| Component | Responsibility |
|---|---|
| Sensor simulator | Generates simulated sensor data |
| Localization | Estimates system position and state |
| LiDAR perception | Processes simulated LiDAR information |
| Local planner | Implements local planning logic |
| PID controller | Implements feedback-based control |

Separating sensor generation, estimation, perception, planning and control makes the components easier to develop and test independently.

## 5. CAN-FD Communication

The communication layer contains message definitions, a communication interface and protocol-handling logic.

Implemented functionality includes:

- CAN-FD message representation.
- Message processing and validation.
- Sequence checking.
- Freshness-related handling.
- Invalid-message rejection.
- Communication protocol testing.

### Recorded communication results

From the dedicated recovery-test run:

| Measurement | Result |
|---|---:|
| Frames received | 3,425 |
| Frames accepted | 3,421 |
| Frames rejected | 4 |
| Consecutive rejections at final report | 0 |

These are software simulation results, not measurements from a physical CAN-FD bus.

## 6. Fault Management and Safety Recovery

A central objective is to demonstrate how the software responds to abnormal timing behaviour.

The implementation includes:

- Controlled control-task delay injection.
- Deadline-miss detection.
- Timing degradation classification.
- Safety-state transitions.
- Diagnostic event recording.
- Recovery-condition monitoring.

### Demonstrated safety-state sequence

`NORMAL → FAIL_SAFE → RECOVERY → NORMAL`

During the controlled fault scenario:

1. A delay is introduced into control execution.
2. The timing monitor detects increasing deadline misses.
3. Timing severity escalates to the critical state.
4. The safety supervisor enters `FAIL_SAFE`.
5. After the injected fault is cleared and health conditions recover, the system enters `RECOVERY`.
6. The system returns to `NORMAL` after recovery checks are satisfied.

### Recorded fault-injection results

| Measurement | Observed result |
|---|---:|
| Control deadline misses | 60 |
| Maximum control execution | 17,856 us (host-observed) |
| Timing state after recovery | OK |
| Diagnostic records | 1 |
| Final safety mode | NORMAL |

The dedicated recovery test explicitly verifies that both `FAIL_SAFE` and `RECOVERY` were observed before the final `NORMAL` state.

## 7. Verification and Testing

The project uses CMake and CTest to build and execute independent software tests.

### Automated test suite

| Test | Verification objective |
|---|---|
| `canfd_protocol_test` | CAN-FD protocol validation |
| `system_logic_test` | Autonomy and safety-related system logic |
| `timing_monitor_test` | Timing-monitor behaviour |
| `integrated_system_test` | FreeRTOS-based integration and controlled fault injection |

**Latest recorded automated verification: 4/4 CTest tests passed.**

### Additional recovery verification

The recovery scenario was also executed separately:

```powershell
.\build-mingw\ease_rt_x.exe --recovery-test
```

Recorded result:

```text
RECOVERY TEST: PASS
FAIL_SAFE observed: YES
RECOVERY observed: YES
Final safety mode: NORMAL
Timing state: OK
All task heartbeats healthy: YES
Sensors healthy: YES
```

The recovery scenario is a separate executable run, not a fifth CTest test.

## 8. Technology Stack

| Area | Technology |
|---|---|
| Programming languages | C, C++17, Python |
| Real-time operating system | FreeRTOS |
| Build system | CMake |
| Compiler | GCC / MinGW |
| Communication | CAN-FD protocol simulation |
| Autonomous processing | Localization, LiDAR perception, planning and PID control |
| Verification | CTest and C++ test executables |
| Development environment | Windows 11, PowerShell and VS Code |

## 9. Repository Structure

```text
EASE-RT-X/
├── config/
│   └── FreeRTOSConfig.h
├── docs/
│   ├── ARCHITECTURE.md
│   ├── FINAL_SCOPE.md
│   ├── FINAL_VALIDATION.md
│   ├── runtime_evidence.txt
│   └── assets/
│       └── EASE-RT-X-architecture.png
├── external/
│   └── FreeRTOS-Kernel/
├── simulation/
│   └── sensor_simulation.py
├── src/
│   ├── autonomy/
│   │   ├── control/
│   │   ├── estimation/
│   │   ├── perception/
│   │   ├── planning/
│   │   └── ros2/
│   ├── communication/
│   ├── common/
│   ├── embedded/
│   │   ├── diagnostics/
│   │   ├── rtos/
│   │   └── safety/
│   └── simulation/
├── tests/
│   ├── canfd_protocol_test.cpp
│   ├── system_logic_test.cpp
│   ├── test_system_logic.py
│   └── timing_monitor_test.cpp
├── CMakeLists.txt
├── README.md
└── setup.ps1
```

## 10. Build and Run

### Prerequisites

- Windows 11
- CMake 3.20 or later
- MinGW GCC/G++
- FreeRTOS Kernel source under `external/FreeRTOS-Kernel`

If the FreeRTOS Kernel source is missing, run:

```powershell
.\setup.ps1
```

### Configure

```powershell
cmake -S . -B build-mingw -G "MinGW Makefiles" -DCMAKE_MAKE_PROGRAM="C:/msys64/ucrt64/bin/mingw32-make.exe" -DCMAKE_BUILD_TYPE=Debug
```

### Build

```powershell
cmake --build build-mingw --parallel 4
```

### Run the application

```powershell
.\build-mingw\ease_rt_x.exe
```

### Run the fault-injection integration scenario

```powershell
.\build-mingw\ease_rt_x.exe --test
```

### Run the recovery scenario

```powershell
.\build-mingw\ease_rt_x.exe --recovery-test
```

### Run automated tests

```powershell
ctest --test-dir build-mingw --output-on-failure
```

## 11. Validation Scope and Limitations

EASE-RT-X is currently a host-based software simulation and verification project.

The recorded results demonstrate software-level task monitoring, timing-fault detection, communication validation, safety-state transitions, diagnostic recording and recovery logic.

The current results do not establish:

- Hardware-measured worst-case execution time (WCET).
- Physical CAN-FD hardware validation.
- Production ECU qualification.
- ISO 26262 compliance or safety certification.
- Flight-qualified or vehicle-qualified software.
- A fully integrated Linux/ROS2 runtime.

These are potential future development and validation directions.

## 12. Future Engineering Work

Potential extensions include:

- Porting the real-time execution layer to an STM32 target.
- Integrating a physical CAN-FD interface.
- Integrating the autonomy components with a Linux/ROS2 runtime.
- Hardware-in-the-loop testing.
- Sensor fault injection and redundancy handling.
- Extended stress testing and timing analysis.
- More comprehensive automated verification.

---

**Engineering focus:** Real-Time Embedded Software · Embedded C/C++ · FreeRTOS · CAN-FD · Autonomous Systems · Timing Supervision · Fault Management · Safety Recovery · Software Verification
