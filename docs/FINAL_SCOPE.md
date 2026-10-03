# Final Scope

1. Embedded real-time core: FreeRTOS tasks, priorities, periodic scheduling, timing measurement, watchdog/heartbeats.
2. CAN-FD protocol layer: typed IDs, freshness, sequence validation, acceptance/rejection statistics.
3. Autonomy pipeline: simulated IMU/GNSS/wheel odometry, localization/fallback, LiDAR validation/perception, local planning and PID control.
4. Safety: health aggregation, timing fault classification, degraded/recovery/fail-safe states.
5. Diagnostics: persistent fault records with occurrence and confirmation state.
6. Verification: CAN-FD test, Python verification specification, runtime timing and fault evidence.
7. Linux/ROS2 integration point: minimal ROS2 health publisher source, intentionally excluded from the Windows FreeRTOS host build.

Explicitly excluded: deep learning, SLAM, cloud services, dashboards, mobile apps, AUTOSAR, certification claims and unnecessary infrastructure.
