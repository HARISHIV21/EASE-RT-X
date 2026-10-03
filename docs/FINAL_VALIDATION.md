 # EASE-RT X Final Validation

The Windows host build is the primary executable demonstration. The results below are actual host-simulation measurements.

## Automated verification

- canfd_protocol_test: CAN-FD validation, sequence checking and freshness handling.
- system_logic_test: timing thresholds, safety transitions, recovery, sensor fallback, perception, planning/control and diagnostics.
- integrated_system_test: complete FreeRTOS execution with controlled timing faults, CAN-FD invalid-frame injection and safety recovery.

## Integrated runtime evidence

### Timing fault

The Control task is intentionally delayed during a controlled interval.

Observed behavior:

FAIL_SAFE -> RECOVERY -> NORMAL

Measured:

- Control deadline misses: 60
- Maximum Control execution: 15 ticks
- Timing state reached: CRITICAL
- Timing state after recovery: OK
- Final system mode: NORMAL

Timing thresholds:

- Warning: 3 misses
- Degraded: 10 misses
- Critical: 50 misses

### CAN-FD validation

Controlled invalid CAN-FD frames were injected.

Final result:

- Received: 3425
- Accepted: 3421
- Rejected: 4
- Consecutive rejections: 0

### Diagnostics

- Diagnostic records: 1

### Safety recovery

The timing fault was cleared and the system recovered through:

FAIL_SAFE -> RECOVERY -> NORMAL

Final state:

- System mode: NORMAL
- RTOS task health: YES
- Timing state: OK

## Runtime measurements

| Measurement | Result |
|---|---:|
| Communication executions | 3421 |
| Communication deadline misses | 0 |
| Communication period | 5 ticks |
| Autonomy executions | 856 |
| Autonomy deadline misses | 0 |
| Autonomy period | 20 ticks |
| Safety executions | 1711 |
| Safety deadline misses | 0 |
| Safety period | 10 ticks |
| Control executions | 1711 |
| Control deadline misses | 60 |
| Control maximum execution | 15 ticks |
| CAN-FD received | 3425 |
| CAN-FD accepted | 3421 |
| CAN-FD rejected | 4 |
| Diagnostic records | 1 |

## Automated test result

3/3 CTest tests passed:

- canfd_protocol_test: PASS
- system_logic_test: PASS
- integrated_system_test: PASS

## Evidence limitations

These results are Windows/MinGW host-simulation measurements.

They are not hardware WCET measurements, production ECU guarantees, certification evidence, ISO 26262 compliance or production safety certification.

The purpose is to demonstrate implemented real-time behavior, fault handling, diagnostics, safety recovery and automated verification.
