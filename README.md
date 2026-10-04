 # EASE-RT X

## Embedded Autonomous Software-Defined Real-Time Platform

EASE-RT X is a **simulation-first heterogeneous embedded platform** designed to demonstrate practical engineering across **real-time embedded software, distributed communication, autonomous-system software, fault management, safety supervision, and automated verification**.

The platform combines a **FreeRTOS-based real-time execution layer** with an **Embedded Linux/ROS2 integration point**, connected through a modeled **CAN-FD communication layer** and a simulated autonomous sensor/control pipeline.

The project focuses on one engineering question:

> **Can a distributed embedded system continue operating safely when timing, communication, sensing, or software faults occur?**

---

## Architecture

```text
                    EASE-RT X
                         │
        ┌────────────────┴────────────────┐
        │                                 │
 Real-Time Embedded Layer          Autonomy Software Layer
        │                                 │
   C / FreeRTOS                    C++ / Linux / ROS2
        │                                 │
 ┌──────┴────────┐                 ┌──────┴──────────┐
 │ Periodic Tasks│                 │ Localization    │
 │ Health Monitor│                 │ Perception      │
 │ Watchdog      │                 │ Planning        │
 │ Safety        │                 │ Control         │
 │ Diagnostics   │                 │                 │
 └──────┬────────┘                 └──────┬──────────┘
        │                                 │
        └──────────── CAN-FD ─────────────┘
                         │
              ┌──────────┴──────────┐
              │ Simulated Sensors   │
              │ IMU / GNSS / LiDAR  │
              │ Wheel Odometry      │
              └─────────────────────┘
