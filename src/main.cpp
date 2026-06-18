#include <Arduino.h>
#include <Config.h>
#include <Kinematics.h>
#include <Wire.h>
#include "Control.h"
#include "Motor.h"
#include "MPU_sensor.h"

Sensor_6050 ssr6050;

// cppcheck-suppress unusedFunction
void setup(){
  Serial.begin(115200); delay(3000);
  while(!Serial.available()){
    Serial.println("Khong khoi dong duoc he thong xin vui long thu lai");
    delay(200);
  }
  Serial.println("Khoi dong he thong thanh cong");
  delay(1000);
  Serial.println("Khoi tao cac ngoai vi");
  delay(1000);
  // IC TB6612FNG setup
  pinMode(motor::stby, OUTPUT);
  digitalWrite(motor::stby, HIGH);

  ledcSetup(motor::channela, motor::freq, motor::resolution);
  ledcAttachPin(motor::pwma, motor::channela);
  Serial.println("Khoi dong xong banh a"); delay(100);

  ledcSetup(motor::channelb, motor::freq, motor::resolution);
  ledcAttachPin(motor::pwmb, motor::channelb);
  Serial.println("Khoi dong xong banh b"); delay(100);

  //MPU_sensor setup
  Wire.begin(MPU::sda, MPU::scl, 400000);
  ssr6050.initSensor_6050();
  ssr6050.CalibrateGrypo();
  ssr6050.waitForInitialSerialSetup();
}
// cppcheck-suppress unusedFunction
void loop() {
  if (Serial.available() > 0) {
    ssr6050.handleSerialCommand(Serial.readStringUntil('\n'));
  }

  if (initialReady) {
    ssr6050.updateAndPrintMPU();
  }
}
