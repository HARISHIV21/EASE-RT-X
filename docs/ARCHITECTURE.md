# EASE-RT X Architecture

EASE-RT X is a simulation-first embedded real-time autonomy platform. The design is intentionally limited to capabilities that demonstrate embedded, real-time, distributed communication, autonomy, safety, fault tolerance and verification.

## Runtime chain

Sensor simulation -> validation -> localization/fusion -> LiDAR perception -> local planning -> closed-loop control -> CAN-FD/health -> safety supervision.

## Safety states

BOOT -> SELF_TEST -> READY -> NORMAL. Health/timing/watchdog/sensor/communication faults transition to DEGRADED. Restored health passes through RECOVERY and returns to NORMAL. Critical faults enter FAIL_SAFE.

## Timing evidence

Critical tasks are periodic. Execution time, period and deadline misses are measured. Timing faults are injected deliberately and classified as warning/degraded/critical. Recovery requires a sustained healthy interval.

## Scope boundary

No deep learning, SLAM, cloud backend, dashboard, AUTOSAR or certification claims are required. ROS2 is an optional Linux integration point and is not required for the Windows FreeRTOS verification build.
