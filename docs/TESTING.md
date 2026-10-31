
# EASE-RT-X — Verification and Testing

## 1. Purpose

This document describes the build, test and fault-injection verification process for EASE-RT-X.

The objective is to verify the software behaviour of the simulated real-time embedded platform, including communication validation, timing monitoring, fault escalation, diagnostic recording and safety supervision.

## 2. Build Environment

- Operating system: Windows
- Programming languages: C and C++
- RTOS: FreeRTOS (Windows MSVC-MingW simulation port)
- Build system: CMake
- Compiler: MinGW
- Test framework: CTest

## 3. Build Instructions

Configure:

```powershell
cmake -S . -B build -G "MinGW Makefiles"
```

Build:

```powershell
cmake --build build -j 4
```

Run the automated test suite:

```powershell
ctest --test-dir build --output-on-failure
```

Run the application:

```powershell
.\build\ease_rt_x.exe --test
```

## 4. Automated Test Coverage

| Test | Verification Focus |
|---|---|
| canfd_protocol_test | CAN-FD protocol behaviour |
| system_logic_test | System and safety logic |
| timing_monitor_test | Timing calculations and deadline detection |
| integrated_system_test | Integrated system fault-response behaviour |

## 5. Timing Fault-Injection Scenario

A deliberate delay is introduced into the control task to simulate a task response exceeding its configured deadline.

The timing monitor evaluates the timestamps and identifies deadline violations.

The timing fault manager escalates the accumulated faults according to configured thresholds.

| Deadline Misses | Timing State |
|---:|---|
| 0–2 | OK |
| 3–9 | WARNING |
| 10–49 | DEGRADED |
| 50 or more | CRITICAL |

## 6. Observed Integration-Test Result

The successful integration-test run produced the following output:

```text
INTEGRATION TEST: PASS
Control deadline misses: 60
Timing state: CRITICAL
Diagnostics recorded: 1
Safety mode: FAIL_SAFE
```

Additional observed system metrics:

| Metric | Observed Value |
|---|---:|
| Control task executions | 690 |
| Control deadline misses | 60 |
| CAN-FD frames received | 1,383 |
| CAN-FD frames accepted | 1,379 |
| CAN-FD frames rejected | 4 |
| Diagnostic records | 1 |
| Final safety mode | FAIL_SAFE |

These values represent one observed simulation run and may vary between executions.

## 7. Verified Fault-Response Sequence

```text
Injected Control-Task Delay
          |
          v
Deadline-Miss Detection
          |
          v
Timing Fault Escalation
          |
          v
CRITICAL Timing State
          |
          v
Diagnostic Recording
          |
          v
Safety Supervisor
          |
          v
FAIL_SAFE
```

## 8. Simulation Limitations

The current implementation uses the FreeRTOS Windows simulation port.

Consequently:

- Timing measurements are host-simulation observations.
- Windows scheduling does not provide hard real-time guarantees.
- CAN-FD communication is software-modeled.
- Sensor inputs are simulated.
- Physical MCU and hardware-in-the-loop validation have not yet been demonstrated.

The observed results verify software-level fault detection and safety-response behaviour, not production hardware performance or certification.
