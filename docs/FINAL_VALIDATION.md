# EASE-RT-X — Final Validation Report

## 1. Validation Overview

EASE-RT-X is validated using a Windows host-based simulation environment with FreeRTOS, MinGW and CMake.

The validation focuses on:

- Real-time task execution and health monitoring.

- Timing supervision and deadline-miss detection.

- CAN-FD protocol validation.

- Autonomous-system logic.

- Fault detection and safety-state transitions.

- Diagnostic event recording.

- Recovery after controlled fault injection.

All runtime measurements in this report are host-simulation results.

## 2. Automated Verification

The current CMake configuration registers four CTest tests.
| Test | Verification scope | Result |
|---|---|---|
| `canfd_protocol_test` | CAN-FD protocol validation, sequence checking and freshness handling | PASS |
| `system_logic_test` | Timing thresholds, safety transitions, recovery, sensor fallback, perception, planning/control and diagnostics | PASS |
| `timing_monitor_test` | Timing-monitor behaviour | PASS |
| `integrated_system_test` | FreeRTOS execution and controlled integration fault injection | PASS |

**Latest recorded CTest result: 4/4 tests passed.**

## 3. Integrated Runtime Fault-Injection Test

### 3.1 Timing fault injection

A controlled delay is introduced into the Control task to evaluate timing supervision and safety behaviour.

Observed behaviour:

`NORMAL → FAIL_SAFE`

Measured results:
| Measurement | Result |
|---|---:|
| Control deadline misses | 60 |
| Maximum Control execution | 15 ticks |
| Timing state reached | CRITICAL |
| Diagnostic records | 1 |

### 3.2 Timing thresholds
| Timing condition | Configured threshold |
|---|---:|
| Warning | 3 misses |
| Degraded | 10 misses |
| Critical | 50 misses |

The injected timing fault causes the timing monitor to escalate the system to the critical state and the safety supervisor to enter `FAIL_SAFE`.

## 4. CAN-FD Communication Validation

Controlled invalid CAN-FD frames are injected to evaluate message validation and rejection behaviour.

Recorded results:
| Measurement | Result |
|---|---:|
| Frames received | 3,425 |
| Frames accepted | 3,421 |
| Frames rejected | 4 |
| Consecutive rejections at final report | 0 |

The results demonstrate protocol-level validation and rejection of invalid frames in the simulation environment.

## 5. Safety Recovery Verification

A separate recovery-test execution evaluates system behaviour after the injected timing fault is cleared.

Observed state sequence:

`FAIL_SAFE → RECOVERY → NORMAL`

The recovery test reported:

```text

RECOVERY TEST: PASS

FAIL_SAFE observed: YES

RECOVERY observed: YES

Final safety mode: NORMAL

Timing state: OK

All task heartbeats healthy: YES

Sensors healthy: YES

```

Final recovery conditions:

- System mode: NORMAL

- RTOS task health: YES

- Timing state: OK

- Sensors healthy: YES

The dedicated recovery test is executed separately from CTest and is not counted as a fifth automated CTest test.

## 6. Runtime Measurements

Recorded values from the integrated runtime evidence:
| Measurement | Result |
|---|---:|
| Communication executions | 3,421 |
| Communication deadline misses | 0 |
| Communication period | 5 ticks |
| Autonomy executions | 856 |
| Autonomy deadline misses | 0 |
| Autonomy period | 20 ticks |
| Safety executions | 1,711 |
| Safety deadline misses | 0 |
| Safety period | 10 ticks |
| Control executions | 1,711 |
| Control deadline misses | 60 |
| Control maximum execution | 15 ticks |
| CAN-FD frames received | 3,425 |
| CAN-FD frames accepted | 3,421 |
| CAN-FD frames rejected | 4 |
| Diagnostic records | 1 |

These are observed host-simulation execution counts and configured task periods, not hardware performance benchmarks.

## 7. Validation Summary

The current validation demonstrates:

- Successful compilation of the EASE-RT-X executable.

- Four passing automated CTest tests.

- Detection of injected Control-task timing faults.

- Timing-state escalation to CRITICAL.

- Safety transition into FAIL_SAFE.

- CAN-FD invalid-frame rejection.

- Diagnostic event recording.

- Recovery through RECOVERY to NORMAL.

- Restoration of healthy task and sensor status.

## 8. Evidence and Limitations

The results represent Windows 11 / MinGW host-simulation measurements.

They do not establish:

- Hardware-measured worst-case execution time (WCET).

- Physical CAN-FD hardware performance.

- Production ECU guarantees.

- Hardware-in-the-loop validation.

- ISO 26262 compliance or safety certification.

- Flight-qualified or vehicle-qualified software.

The purpose of this validation is to demonstrate implemented real-time software behaviour, communication validation, fault handling, diagnostic recording, safety supervision and recovery logic in a controlled simulation environment.

---

**Project:** EASE-RT-X

**Validation environment:** Windows 11 / MinGW / CMake / FreeRTOS

**Automated verification:** 4/4 CTest tests passed

**Additional verification:** Dedicated recovery test passed
