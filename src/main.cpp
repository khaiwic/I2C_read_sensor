#include <Arduino.h>
#include <Wire.h>

#define sda 8
#define scl 9

const int MPU_address = 0x68;

// Các biến phục vụ tính toán góc và thời gian
unsigned long long last_time;
float dt;

float roll = 0, pitch = 0; // Góc nghiêng thực tế trong không gian (đơn vị: Độ)

void setup(){
  Serial.begin(115200);
  Wire.begin(sda, scl);
  
  // Đánh thức MPU6050
  Wire.beginTransmission(MPU_address);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission();
  
  last_time = millis(); // Khởi tạo mốc thời gian ban đầu
}

void loop(){
  // 1. Tính toán khoảng thời gian thực tế giữa 2 lần đọc (dt)
  unsigned long long now = millis();
  dt = (float)(now - last_time) / 1000.0f; // Đổi sang giây
  last_time = now;

  // 2. Đọc dữ liệu thô từ MPU6050
  Wire.beginTransmission(MPU_address);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_address, 14, 1);
  
  int16_t ax = (Wire.read()<< 8) | Wire.read();
  int16_t ay = (Wire.read()<< 8) | Wire.read();
  int16_t az = (Wire.read()<< 8) | Wire.read();
  int16_t raw_temp = (Wire.read()<< 8) | Wire.read();
  int16_t gx = (Wire.read()<< 8) | Wire.read();
  int16_t gy = (Wire.read()<< 8) | Wire.read();
  int16_t gz = (Wire.read()<< 8) | Wire.read();
  
  // 3. Đổi sang đơn vị chuẩn vật lý
  float accX = (float)ax / 16384.0f;
  float accY = (float)ay / 16384.0f;
  float accZ = (float)az / 16384.0f;
  
  float gyroX = (float)gx / 131.0f;
  float gyroY = (float)gy / 131.0f;
  
  // 4. Tính toán góc nghiêng dựa vào gia tốc trọng trường (Accelerometer)
  // Sử dụng hàm lượng giác atan2 để tính góc nghiêng tuyệt đối so với mặt đất
  float roll_acc = atan2(accY, accZ) * 180.0f / PI;
  float pitch_acc = atan2(-accX, sqrt(accY * accY + accZ * accZ)) * 180.0f / PI;

  // 5. Thuật toán Bộ lọc bù (Complementary Filter)
  // Lấy 96% góc từ việc tích phân Gyro (mượt mà, không nhiễu) 
  // kết hợp với 4% góc từ Accel (để giữ cho góc không bị trôi theo thời gian)
  roll = 0.96f * (roll + gyroX * dt) + 0.04f * roll_acc;
  pitch = 0.96f * (pitch + gyroY * dt) + 0.04f * pitch_acc;

  // 6. In kết quả góc không gian ra màn hình
  Serial.printf("GOC KHONG GIAN -> Roll (Nghieng trai/phai): %.2f do | Pitch (Nghieng chuoi/ngua): %.2f do\n", roll, pitch);
  Serial.println("---------------------------------------------------------------------------------");

  delay(300); // Đọc nhanh hơn (mỗi 20ms) để phép tích phân chính xác, giảm delay 500ms xuống!
}