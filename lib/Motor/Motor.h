#ifndef MOTOR_H
#define MOTOR_H
#include "Kinematics.h"
#include "config.h"

extern int encoder_1_val;
extern int encoder_2_val;

void init_encoder();
void read_encoder_A();
void read_encoder_B();
void reset();
void go(int speedA, int speedB);
#endif