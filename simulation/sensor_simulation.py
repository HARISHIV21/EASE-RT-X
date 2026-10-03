"""Deterministic sensor/fault generator used for EASE-RT X verification."""
from dataclasses import dataclass
from enum import Enum
class Fault(Enum):
    NONE="none"; GNSS_DROPOUT="gnss_dropout"; LIDAR_INVALID="lidar_invalid"; SENSOR_DELAY="sensor_delay"; IMU_BIAS="imu_bias"
@dataclass
class SensorFrame:
    t_ms:int; position_x:float; velocity:float; gnss_valid:bool=True; lidar_valid:bool=True; imu_bias:float=0.0
def generate(t_ms:int,speed:float=8.0,fault:Fault=Fault.NONE)->SensorFrame:
    return SensorFrame(t_ms,t_ms/1000.0*speed,speed,fault is not Fault.GNSS_DROPOUT,fault is not Fault.LIDAR_INVALID,0.08 if fault is Fault.IMU_BIAS else 0.0)
