#ifndef CONTROL_H
#define CONTROL_H 
#include <Arduino.h>
#include <Config.h>
#include <Kinematics.h>
#include "Motor.h"
#include "MPU_sensor.h"

struct PID_data{
    int32_t sum_err_enc_a = 0;
    int32_t sum_err_enc_b = 0;
    int32_t err_enc_a_pre = 0;
    int32_t err_enc_b_pre = 0;
    int32_t output_enc_a = 0;
    int32_t output_enc_b = 0;
};

class PID{
public:
    PID();
    int32_t calcPIDFromEncoder();
    float calcPIDFromMPU();
};
#endif