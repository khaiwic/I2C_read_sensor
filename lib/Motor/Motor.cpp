#include <Arduino.h>
#include "Motor.h"
#include "config.h"
#include "Kinematics.h"
#include <Config.h>

int encoder_1_val = 0;
int encoder_2_val = 0;

const int16_t encoderA_1 = encoder::encoder1A;
const int16_t encoderA_2 = encoder::encoder1B;
const int16_t encoderB_1 = encoder::encoder2A;
const int16_t encoderB_2 = encoder::encoder2B;
void init_encoder(){
    pinMode(encoderA_1, INPUT_PULLUP);
    pinMode(encoderA_2, INPUT_PULLUP);
    pinMode(encoderB_1, INPUT_PULLUP);
    pinMode(encoderB_2, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(encoderA_1), read_encoder_A, RISING);
    attachInterrupt(digitalPinToInterrupt(encoderB_1), read_encoder_B, RISING);
}
void IRAM_ATTR read_encoder_A(){
    int A = digitalRead(encoderA_1);
    int B = digitalRead(encoderA_2);
    if((A == HIGH) != (B == LOW)){
        encoder_1_val++;
    }
    else {
       encoder_1_val--;
    }
}
void IRAM_ATTR read_encoder_B(){
    int A = digitalRead(encoderB_1);
    int B = digitalRead(encoderB_2);
    if((A == HIGH) != (B == LOW)){
        encoder_2_val--;
    }
    else {
        encoder_2_val++;
    }
}
//chua dung gia tri tuyet doi

void reset(){
    encoder_1_val = 0;
    encoder_2_val = 0;
}

void go(int speedA, int speedB){
    if(speedA > 0){
        if(speedB < 0){
            ledcWrite(MOTOR::channela, speedA);
            ledcWrite(MOTOR::channelb, -speedB);
        }
        else{
            ledcWrite(MOTOR::channela, speedA);
            ledcWrite(MOTOR::channelb, speedB);
        }
    }
    else if (speedA < 0){
        if(speedB > 0){
            ledcWrite(MOTOR::channela, -speedA);
            ledcWrite(MOTOR::channelb, speedB);
        }
        else{
            ledcWrite(MOTOR::channela, -speedA);
            ledcWrite(MOTOR::channelb, -speedB);
        }
    }
    else if(speedA == 0){
        if(speedB == 0){
            ledcWrite(MOTOR::channela, 0);
            ledcWrite(MOTOR::channelb, 0);
        }
    }
}