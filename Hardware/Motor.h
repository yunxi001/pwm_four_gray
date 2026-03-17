#ifndef MOTOR_H
#define MOTOR_H

#include "stm32f10x.h"


typedef enum {
    MOTOR1 = 1,
    MOTOR2 = 2,
    MOTOR3 = 3,
    MOTOR4 = 4
} Motor_ID;

void PWM_Motors_Init(void);  
void Motor_SetSpeed(Motor_ID motor_id, int16_t speed);

#endif
