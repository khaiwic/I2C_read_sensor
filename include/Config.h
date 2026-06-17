#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>

namespace encoder{
    //config encoder values pin
    constexpr int16_t encoder1A = 0;
    constexpr int16_t encoder1B = 0;
    constexpr int16_t encoder2A = 0;
    constexpr int16_t encoder2B = 0;
}
//
namespace motor{
    //config tb6612 fng input pin
    constexpr int16_t channela = 0;
    constexpr int16_t channelb = 1;
    constexpr int16_t freq = 20000;
    constexpr int16_t resolution = 10;
    constexpr int16_t pwma = 0;
    constexpr int16_t pwmb = 0;
    constexpr int16_t in1_a = 0;
    constexpr int16_t in1_b = 0;
    constexpr int16_t in2_a = 0;
    constexpr int16_t in2_b = 0;
    constexpr int16_t stby = 0;
}
//
namespace MPU{
    //config MPU_6050 comunicate by I2C
    constexpr int32_t sda = 0;
    constexpr int32_t scl = 0;
}
namespace PID{
    //PID controller setup
    constexpr int16_t K_p1 = 0;
    constexpr int16_t K_i1 = 0;
    constexpr int16_t K_d1 = 0;
    
    constexpr int16_t K_p2 = 0;
    constexpr int16_t K_i2 = 0;
    constexpr int16_t K_d2 = 0;
}
#endif