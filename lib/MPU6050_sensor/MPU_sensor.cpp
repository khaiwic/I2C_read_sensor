#include <Arduino.h>
#include <Kinematics.h>
#include <Config.h>
#include <Wire.h>
#include <math.h>
#include "MPU_sensor.h"

MPURaw raw;
InitialState initialState = {0, 0, 0, 0, 0, 0};

constexpr uint32_t PRINT_INTERVAL_MS = 200;
constexpr float FILTER_ALPHA = 0.96f;
constexpr float GYRO_SCALE = 131.0f; 
constexpr float ACCEL_SCALE = 16384.0f;

volatile bool initialReady = false;


float gyroBiasX = 0.0f;
float gyroBiasY = 0.0f;
float gyroBiasZ = 0.0f;

float fusedRoll = 0.0f;
float fusedPitch = 0.0f;
float yawAngle = 0.0f;

float refRoll = 0.0f;
float refPitch = 0.0f;
float refAccelX = 0.0f;
float refAccelY = 0.0f;
float refAccelZ = 0.0f;

uint32_t lastPrintMs = 0;
uint32_t preUS = 0;

MPUdata convertRawData(const MPURaw &raw);
float calcRollFromAccel(const MPUdata &data);
float calcPitchFromAccel(const MPUdata &data);
bool parseSetCommand(const String &line, InitialState &state);

Sensor_6050::Sensor_6050(){}
bool Sensor_6050::initSensor_6050(){
    Wire.beginTransmission(MPU::MPU6050_ADDR);
    Wire.write(MPU::REG_PWR_MGMT_1);
    Wire.write(0x00);
    if (Wire.endTransmission() != 0) {
        return false;
    }

    delay(100);

    Wire.beginTransmission(MPU::MPU6050_ADDR);
    Wire.write(MPU::REG_ACCEL_CONFIG);
    Wire.write(0x00);
    if (Wire.endTransmission() != 0) {
        return false;
    }

    Wire.beginTransmission(MPU::MPU6050_ADDR);
    Wire.write(MPU::REG_GYRO_CONFIG);
    Wire.write(0x00);
    return Wire.endTransmission() == 0;
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
            sumX += raw.gx / GYRO_SCALE;
            sumY += raw.gy / GYRO_SCALE;
            sumZ += raw.gz / GYRO_SCALE;
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
bool Sensor_6050::readMPURaw(MPURaw &data){
    Wire.beginTransmission(MPU::MPU6050_ADDR);
    Wire.write(MPU::REG_ACCEL_XOUT_H);

    if(Wire.endTransmission(false) != 0){
        return false;
    }
    if(Wire.requestFrom(MPU::MPU6050_ADDR, static_cast<uint8_t>(14)) != 14){
        return false;
    }
    data.ax = Sensor_6050::ReadInt16();
    data.ay = Sensor_6050::ReadInt16();
    data.az = Sensor_6050::ReadInt16();

    Sensor_6050::ReadInt16();

    data.gx = Sensor_6050::ReadInt16();
    data.gy = Sensor_6050::ReadInt16();
    data.gz = Sensor_6050::ReadInt16();  
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

    InitialState state;
    if(parseSetCommand(line, state)){
        setInitialState(state);
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
MPUdata convertRawData(const MPURaw &raw){
    MPUdata data;
    data.axG = raw.ax / ACCEL_SCALE;
    data.ayG = raw.ay / ACCEL_SCALE;
    data.azG = raw.az / ACCEL_SCALE;
    data.gxDps = raw.gx / GYRO_SCALE - gyroBiasX;
    data.gyDps = raw.gy / GYRO_SCALE - gyroBiasY;
    data.gzDps = raw.gz / GYRO_SCALE - gyroBiasZ;
  return data;
}
bool Sensor_6050::setInitialState(const InitialState &state) {
  MPURaw raw;
  if (!readMPURaw(raw)) {
    Serial.println("Loi: khong doc duoc MPU6050 de lay moc ban dau.");
    return false;
  }

  const MPUdata data = convertRawData(raw);

  initialState = state;
  fusedRoll = calcRollFromAccel(data);
  fusedPitch = calcPitchFromAccel(data);
  yawAngle = initialState.yaw;

  refRoll = fusedRoll;
  refPitch = fusedPitch;
  refAccelX = data.axG;
  refAccelY = data.ayG;
  refAccelZ = data.azG;

  initialReady = true;
  preUS = micros();

  Serial.println("Da nhan moc ban dau. Bat dau doc du lieu...");
  Serial.println("roll,pitch,yaw,x,y,z,ax_g,ay_g,az_g,gx_dps,gy_dps,gz_dps");
  return true;
}
bool parseSetCommand(const String &line, InitialState &state) {
  char command[8] = {0};
  float roll = 0.0f;
  float pitch = 0.0f;
  float yaw = 0.0f;
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;

  const int count = sscanf(line.c_str(), "%7s %f %f %f %f %f %f", command,
                           &roll, &pitch, &yaw, &x, &y, &z);

  if (count != 7 || !String(command).equalsIgnoreCase("SET")) {
    return false;
  }

  state.roll = roll;
  state.pitch = pitch;
  state.yaw = yaw;
  state.x = x;
  state.y = y;
  state.z = z;
  return true;
}
float calcRollFromAccel(const MPUdata &data){
    return atan2f(data.ayG, data.azG) * 180.0f / PI;

}
float calcPitchFromAccel(const MPUdata &data){
    return atan2f(-data.axG, sqrtf(data.ayG * data.ayG + data.azG * data.azG)) *
        180.0f / PI;
}
void Sensor_6050::updateAndPrintMPU(){
    // chua biet dc raw co phai la du lieu hay khong 
    if(!readMPURaw(raw)){
        Serial.println("Loi cam bien MPU"); delay(100);
        return;
    }

    const int32_t nowUS = micros();
    const float dt = (nowUS - preUS) / 1000000.0f;
    preUS = nowUS;
    const MPUdata data = convertRawData(raw);
    const float accelRoll = calcRollFromAccel(data);
    const float accelPitch = calcPitchFromAccel(data);
    //cong thuc tinh toan sau se day vao kinematics.h
    fusedRoll = FILTER_ALPHA * (fusedRoll + data.gxDps * dt) +
              (1.0f - FILTER_ALPHA) * accelRoll;
    fusedPitch = FILTER_ALPHA * (fusedPitch + data.gyDps * dt) +
               (1.0f - FILTER_ALPHA) * accelPitch;
    yawAngle += data.gzDps * dt;

    //
    const float roll = initialState.roll + (fusedRoll - refRoll);
    const float pitch = initialState.pitch + (fusedPitch - refPitch);
    const float yaw = yawAngle;

    const float x = initialState.x + (data.axG - refAccelX);
    const float y = initialState.y + (data.ayG - refAccelY);
    const float z = initialState.z + (data.azG - refAccelZ);

    if (millis() - lastPrintMs < PRINT_INTERVAL_MS) {
        return;
    }
    lastPrintMs = millis();

    Serial.print(roll, 2);
    Serial.print(",");
    Serial.print(pitch, 2);
    Serial.print(",");
    Serial.print(yaw, 2);
    Serial.print(",");
    Serial.print(x, 4);
    Serial.print(",");
    Serial.print(y, 4);
    Serial.print(",");
    Serial.print(z, 4);
    Serial.print(",");
    Serial.print(data.axG, 4);
    Serial.print(",");
    Serial.print(data.ayG, 4);
    Serial.print(",");
    Serial.print(data.azG, 4);
    Serial.print(",");
    Serial.print(data.gxDps, 3);
    Serial.print(",");
    Serial.print(data.gyDps, 3);
    Serial.print(",");
    Serial.println(data.gzDps, 3);
}
