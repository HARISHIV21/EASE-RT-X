#include "sensor_simulator.hpp"
#include <cmath>
namespace ease_rt_x {
SimSensorFrame SensorSimulator::step(std::uint64_t t,double speed,double heading){
    (void)heading;
    SimSensorFrame f{}; f.imu.timestamp_ms=t; f.imu.health={true,true,false,0}; f.imu.gyro_z_rad_s=0;
    f.gnss.timestamp_ms=t; f.gnss.health={true,true,false,0}; f.gnss.position_x_m=speed*(t/1000.0); f.gnss.position_y_m=0; f.gnss.velocity_mps=speed;
    f.wheel.timestamp_ms=t; f.wheel.health={true,true,false,0}; f.wheel.velocity_mps=speed; f.wheel.distance_m=speed*0.01;
    f.lidar.timestamp_ms=t; f.lidar.valid=true; f.lidar.faulted=false; f.lidar.nearest_obstacle_m=25; f.lidar.free_path_width_m=6;
    switch(fault_){case SensorFault::GNSS_DROPOUT:f.gnss.health={false,false,true,100};break;case SensorFault::LIDAR_INVALID:f.lidar.valid=false;f.lidar.faulted=true;break;case SensorFault::IMU_BIAS:f.imu.gyro_z_rad_s=0.08;break;case SensorFault::SENSOR_DELAY:f.gnss.health.fresh=false;f.gnss.health.age_ms=100;break;default:break;}
    return f;
}
}
