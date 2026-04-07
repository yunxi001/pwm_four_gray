#ifndef MOTOR_H
#define MOTOR_H

#include "stm32f10x.h"

// 实际电机配置说明：
// MOTOR1正转 → 右前轮反转
// MOTOR2正转 → 左前轮正转
// MOTOR3正转 → 右后轮正转
// MOTOR4正转 → 左后轮反转
typedef enum
{
    MOTOR1 = 1, // 控制右前轮（正转时车轮反转）
    MOTOR2 = 2, // 控制左前轮（正转时车轮正转）
    MOTOR3 = 3, // 控制右后轮（正转时车轮正转）
    MOTOR4 = 4  // 控制左后轮（正转时车轮反转）
} Motor_ID;

// 车轮位置枚举
typedef enum
{
    WHEEL_FRONT_LEFT = 0,  // 左前轮
    WHEEL_FRONT_RIGHT = 1, // 右前轮
    WHEEL_REAR_LEFT = 2,   // 左后轮
    WHEEL_REAR_RIGHT = 3   // 右后轮
} Wheel_Position;

void PWM_Motors_Init(void);
void Motor_SetSpeed(Motor_ID motor_id, int16_t speed);

// 新的车轮控制接口 - 统一的速度方向映射
// speed: +正转（车轮向前），-反转（车轮向后），范围 -1000 到 +1000
// 内部自动处理各电机的方向差异
void Set_Wheel_Speed(Wheel_Position wheel, int16_t speed);

// 四轮统一控制接口
void Set_All_Wheels(int16_t front_left, int16_t front_right, int16_t rear_left, int16_t rear_right);

// 车辆运动控制接口
void Car_Move_Forward(int16_t speed);  // 前进
void Car_Move_Backward(int16_t speed); // 后退
void Car_Turn_Left(int16_t speed);     // 左转
void Car_Turn_Right(int16_t speed);    // 右转
void Car_Stop(void);                   // 停止
void Motor_Position_Test(void);        // 测试
#endif
