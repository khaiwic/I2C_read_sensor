#ifndef MPU_6050_H
#define MPU_6050_H

#include <Arduino.h>
#include <Wire.h>
#include <Config.h>
#include <Kinematics.h>

class Sensor_6050{
public:
    Sensor_6050();
    void initSensor_6050();
    void CalibrateGrypo();
};
#endif