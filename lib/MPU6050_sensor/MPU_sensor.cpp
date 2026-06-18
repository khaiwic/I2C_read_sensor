#include <Arduino.h>
#include <Kinematics.h>
#include <Config.h>
#include <Wire.h>
#include <math.h>
#include "MPU_sensor.h"

Sensor_6050::Sensor_6050(){}
void Sensor_6050::initSensor_6050(){
    pinMode(MPU::scl, INPUT);
    pinMode(MPU::sda, INPUT);
}
void Sensor_6050::CalibrateGrypo(){
    Serial.println("Please, hold MPU6050 steady for grypo Calibration"); delay(200);
    
    //Sampling frequency
    const int samples = MPU::freq_mpu;
    float sumX = 0.0f;
    float sumY = 0.0f;
    float sumZ = 0.0f;
    int validSamples = 0;

    for(int i = 0; i < samples; i++){
        
    }
}