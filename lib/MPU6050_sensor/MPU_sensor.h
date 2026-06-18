#ifndef MPU_6050_H
#define MPU_6050_H

#include <Arduino.h>
#include <Wire.h>
#include <Config.h>
#include <Kinematics.h>

//declare values 
float gyroBiasX = 0.0f;
float gyroBiasY = 0.0f;
float gyroBiasZ = 0.0f;



//struct data input by MPU6050
struct MPURaw{
    int16_t ax;
    int16_t ay;
    int16_t az;
    int16_t gx;
    int16_t gy;
    int16_t gz;
};

//struct data caculate
struct MPUdata{
    float axG;
    float ayG;
    float azG;
    float gxDps;
    float gyDps;
    float gzDps;
};
// struct initialState when program started
struct InitialState {
  float roll;
  float pitch;
  float yaw;
  float x;
  float y;
  float z;
};

class Sensor_6050{
public:
    Sensor_6050();
    bool initSensor_6050();
    void CalibrateGrypo();
    bool readMPURaw(MPURaw data);
    void waitForInitialSerialSetup();
    void handleSerialCommand(String b);
    void updateAndPrintMPU();
    int16_t ReadInt16();
};
#endif