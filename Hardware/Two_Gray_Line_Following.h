#ifndef __TWO_GRAY_LINE_FOLLOWING_FIXED_H
#define __TWO_GRAY_LINE_FOLLOWING_FIXED_H

#include "stm32f10x.h"

// 传感器校准函数
void Calibrate_Gray_Sensors(void);

// 四驱PID巡线函数
void Two_Gray_FourDrive_PD_Patrol(uint8_t map, uint16_t speed, float kp, float kd);

// 四驱简单巡线函数
void Simple_Line_Following_FourDrive(uint16_t speed);

// 带转弯检测的巡线函数
void Line_Following_With_Turns(uint16_t speed, float kp, float kd);

#endif
