#include <Arduino.h>
#include <Kinematics.h>
#include <Config.h>
#include <Wire.h>
#include <math.h>
#include "MPU_sensor.h"

MPURaw raw;

volatile bool initialReady = false;
Sensor_6050::Sensor_6050(){}
bool Sensor_6050::initSensor_6050(){
  
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
        if(Sensor_6050::readMPURaw(raw)){
            sumX += raw.gx / MPU::GYRO_SCALE;
            sumY += raw.gy / MPU::GYRO_SCALE;
            sumZ += raw.gz / MPU::GYRO_SCALE;
            ++validSamples;
        }
        delay(3);
        if(validSamples == 0){
            Serial.println("Khong doc duoc du lieu tu cam bien");
            delay(200);
            return;
        }
        gyroBiasX = sumX / validSamples;
        gyroBiasY = sumY / validSamples;
        gyroBiasZ = sumZ / validSamples;

        Serial.print("Gyro bias X/Y/Z: ");
        Serial.print(gyroBiasX, 4);
        Serial.print(", ");
        Serial.print(gyroBiasY, 4);
        Serial.print(", ");
        Serial.println(gyroBiasZ, 4);
    }
}
bool Sensor_6050::readMPURaw(MPURaw data){
    Wire.beginTransmission(MPU::MPU6050_ADDR);
    Wire.write(MPU::REG_ACCEL_XOUT_H);

    if(Wire.endTransmission(false) != 0){
        return false;
    }
    if(Wire.requestFrom(MPU::MPU6050_ADDR, static_cast<uint8_t>(14)) != 14){
        return false;
    }
    raw.ax = Sensor_6050::ReadInt16();
    raw.ay = Sensor_6050::ReadInt16();
    raw.az = Sensor_6050::ReadInt16();

    Sensor_6050::ReadInt16();

    raw.gx = Sensor_6050::ReadInt16();
    raw.gy = Sensor_6050::ReadInt16();
    raw.gz = Sensor_6050::ReadInt16();  
    return true;
}
int16_t Sensor_6050::ReadInt16(){
    const uint8_t MSB = Wire.read();
    const uint8_t LSB = Wire.read();  
    return static_cast<int16_t>((MSB << 8) | LSB);
}
void printSupport(){
  Serial.println();
  Serial.println("Nhap lenh setup ban dau:");
  Serial.println("  SET roll pitch yaw x y z");
  Serial.println("Vi du:");
  Serial.println("  SET 0 0 0 0 0 0");
  Serial.println();
}
void Sensor_6050::handleSerialCommand(String b){
    String line = b;
    line.trim();
    if(line.length() == 0){
        return;
    }

    if(line.equalsConstantTime("HELP")){
        printSupport();
        return;
    }
    Serial.println("Lenh khong hop le. Go HELP de xem cu phap.");
}
void Sensor_6050::waitForInitialSerialSetup(){
    printSupport();
    while(!initialReady){
        if(Serial.available() > 0){
            handleSerialCommand(Serial.readStringUntil('\n'));
        }
        delay(10);
    }
}