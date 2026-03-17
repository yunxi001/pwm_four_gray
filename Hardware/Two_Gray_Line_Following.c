#include "Two_gray_sensor.h"
#include "Motor.h"
#include "Delay.h"
#include "Serial.h"
#include "Timer.h"  // 添加定时器头文件
#include <stdio.h>
#include <math.h>

// 声明外部变量
extern uint16_t Analog_data[2];

// 全局转弯状态管理
uint8_t g_turning_in_progress = 0;

// 传感器校准函数 - 用于确定归一化参数
void Calibrate_Gray_Sensors(void) {
    uint16_t white_left = 0, white_right = 0;
    uint16_t black_left = 4095, black_right = 4095;
    
    printf("Start sensor calibration...\r\n");
    printf("Place sensors on WHITE area, start in 5 seconds\r\n");
    Delay_ms(5000);
    
    // 采集白色区域数据
    for(int i = 0; i < 100; i++) {
        if(Analog_data[0] > white_left) white_left = Analog_data[0];
        if(Analog_data[1] > white_right) white_right = Analog_data[1];
        Delay_ms(10);
    }
    
    printf("Place sensors on BLACK line, start in 5 seconds\r\n");
    Delay_ms(5000);
    
    // 采集黑线数据
    for(int i = 0; i < 100; i++) {
        if(Analog_data[0] < black_left) black_left = Analog_data[0];
        if(Analog_data[1] < black_right) black_right = Analog_data[1];
        Delay_ms(10);
    }
    
    printf("Calibration complete:\r\n");
    printf("Left sensor: White=%d, Black=%d\r\n", white_left, black_left);
    printf("Right sensor: White=%d, Black=%d\r\n", white_right, black_right);
}

/**************
Two_Gray_FourDrive_PD_Patrol 函数为四驱二灰巡线PID
根据实际电机配置调整：
- MOTOR1(左后): +速度=前进, MOTOR2(右后): +速度=后退
- MOTOR3(左前): +速度=后退, MOTOR4(右前): +速度=前进
- 前进需要: M1=+, M2=-, M3=-, M4=+
***************** */
void Two_Gray_FourDrive_PD_Patrol(uint8_t map, uint16_t speed, float kp, float kd) {
    static float last_error = 0;
    static float error = 0;
    static float output = 0;
    
    // 根据校准结果设置的参数
    // 左传感器: 白色≈3815, 黑色≈841
    // 右传感器: 白色≈3884, 黑色≈1254
    static const uint16_t left_white = 3815;
    static const uint16_t left_black = 841;
    static const uint16_t right_white = 3884;
    static const uint16_t right_black = 1254;
    
    // 分别归一化每个传感器（0=黑线，1=白色）
    float left_norm = (float)(Analog_data[0] - left_black) / (left_white - left_black);
    float right_norm = (float)(Analog_data[1] - right_black) / (right_white - right_black);
    
    // 限制归一化值在0-1范围内
    if(left_norm < 0) left_norm = 0;
    if(left_norm > 1) left_norm = 1;
    if(right_norm < 0) right_norm = 0;
    if(right_norm > 1) right_norm = 1;
    
    // 计算误差
    if(map == 1) {
        // 白底黑线模式
        error = 510 * (right_norm - left_norm);
    } else if(map == 2) {
        // 黑底白线模式  
        error = 510 * (left_norm - right_norm);
    }
    
    // 误差死区
    const int ERROR_DEAD_ZONE = 5;
    if(fabs(error) < ERROR_DEAD_ZONE) {
        error = 0;  // 微小误差视为0，不调整
    }
    
    // PD控制输出
    output = kp * error + kd * (error - last_error);
    
    // 限制输出范围，防止电机速度过大
    if(output > speed) output = speed;
    if(output < -speed) output = -speed;
    
    // 电机控制（根据实际配置调整）
    // 左轮组速度：基础速度 - 输出（左转时减速）
    int16_t left_speed_base = speed - (int16_t)output;
    // 右轮组速度：基础速度 + 输出（左转时加速）
    int16_t right_speed_base = speed + (int16_t)output;
    
    // 应用到各个电机，考虑每个电机的方向特性
    Motor_SetSpeed(MOTOR1, left_speed_base);    // 左后轮：正转前进
    Motor_SetSpeed(MOTOR2, -right_speed_base);  // 右后轮：反转前进
    Motor_SetSpeed(MOTOR3, -left_speed_base);   // 左前轮：反转前进
    Motor_SetSpeed(MOTOR4, right_speed_base);   // 右前轮：正转前进
    
    last_error = error;
    
    // 调试输出（可选）
    #ifdef DEBUG_LINE_FOLLOWING
    printf("L:%d R:%d Ln:%.2f Rn:%.2f E:%.1f O:%.1f LS:%d RS:%d\r\n", 
           Analog_data[0], Analog_data[1], left_norm, right_norm, error, output, left_speed_base, right_speed_base);
    #endif
}

// 转弯检测和处理函数
typedef enum {
    STRAIGHT_LINE,    // 直线巡线
    LEFT_TURN,        // 左转
    RIGHT_TURN,       // 右转
    BOTH_TURN         // 十字路口或T字路口
} TurnType_t;

// 简单的毫秒计时器（使用系统滴答）
static uint32_t GetMillis(void) {
    return SysTick->VAL / (SystemCoreClock / 1000);
}

// 转弯检测 - 修复延时问题，使用更可靠的检测机制
TurnType_t Detect_Turn_Type(void) {
    static int both_confirm_count = 0;
    
    const int CONFIRM_THRESHOLD = 2;  // 改为2次确认（2*20ms=40ms）
    
    uint16_t left_threshold = (3815 + 841) / 2;   // ≈ 2328
    uint16_t right_threshold = (3884 + 1254) / 2; // ≈ 2569
    
    uint16_t left = Analog_data[0];
    uint16_t right = Analog_data[1];
    
    // 如果正在转弯，暂停检测
    if(g_turning_in_progress) {
        return STRAIGHT_LINE;
    }
    
    // 检测当前状态
    uint8_t left_on_line = (left < left_threshold) ? 1 : 0;
    uint8_t right_on_line = (right < right_threshold) ? 1 : 0;
    
    // 十字路口检测（两个传感器都在黑线上）
    if(left_on_line && right_on_line) {
        both_confirm_count++;
        if(both_confirm_count >= CONFIRM_THRESHOLD) {
            both_confirm_count = 0;  // 重置计数
            g_turning_in_progress = 1;  // 设置转弯标志
            return BOTH_TURN;
        }
    } else {
        both_confirm_count = 0;  // 重置计数
    }
    
    return STRAIGHT_LINE;
}

// 执行左转90度 - 原地转弯，快速响应
void Execute_Left_Turn(uint16_t speed) {
    printf("执行左转\r\n");
    
    uint16_t left_threshold = (3815 + 841) / 2;   // ≈ 2328
    
    // 原地左转：左轮反转，右轮正转，速度更快
    // 第一阶段：快速转到左传感器离开黑线
    int turn_timeout = 0;
    while(Analog_data[0] < left_threshold && turn_timeout < 50) {
        Motor_SetSpeed(MOTOR1, -speed);    // 左后轮反转
        Motor_SetSpeed(MOTOR2, -speed);    // 右后轮正转  
        Motor_SetSpeed(MOTOR3, speed);     // 左前轮反转
        Motor_SetSpeed(MOTOR4, speed);     // 右前轮正转
        Delay_ms(5);
        turn_timeout++;
    }
    
    // 第二阶段：继续转到传感器重新找到黑线
    turn_timeout = 0;
    while(Analog_data[0] >= left_threshold && turn_timeout < 50) {
        Motor_SetSpeed(MOTOR1, -speed);    // 左后轮反转，保持高速
        Motor_SetSpeed(MOTOR2, -speed);    // 右后轮正转  
        Motor_SetSpeed(MOTOR3, speed);     // 左前轮反转
        Motor_SetSpeed(MOTOR4, speed);     // 右前轮正转
        Delay_ms(5);
        turn_timeout++;
    }
    
    // 停止转弯
    Motor_SetSpeed(MOTOR1, 0);
    Motor_SetSpeed(MOTOR2, 0);
    Motor_SetSpeed(MOTOR3, 0);
    Motor_SetSpeed(MOTOR4, 0);
    
    // 重置转弯状态
//    g_turning_in_progress = 0;
    
    printf("左转完成\r\n");
}

// 执行右转90度 - 原地转弯，快速响应
void Execute_Right_Turn(uint16_t speed) {
    printf("执行右转\r\n");
    
    uint16_t left_threshold = (3815 + 841) / 2;   // ≈ 2328
    uint16_t right_threshold = (3884 + 1254) / 2; // ≈ 2569
    
    // 原地右转：左轮正转，右轮反转，速度更快
    // 第一阶段：快速转到右传感器离开黑线
//    int turn_timeout = 0;
    while(Analog_data[1] < right_threshold ) {
        Motor_SetSpeed(MOTOR1, 600);     // 左后轮正转
        Motor_SetSpeed(MOTOR2, 600);     // 右后轮反转  
        Motor_SetSpeed(MOTOR3, -600);    // 左前轮正转
        Motor_SetSpeed(MOTOR4, -600);    // 右前轮反转
        Delay_ms(5);  // 很短的延时
    }
    
    // 第二阶段：继续转到左传感器找到新的黑线
    while(Analog_data[0] >= left_threshold ) {
        Motor_SetSpeed(MOTOR1, 600);     // 左后轮正转，保持高速
        Motor_SetSpeed(MOTOR2, 600);     // 右后轮反转  
        Motor_SetSpeed(MOTOR3, -600);    // 左前轮正转
        Motor_SetSpeed(MOTOR4, -600);    // 右前轮反转
        Delay_ms(5);
//		g_turning_in_progress = 0;
//		g_turn_request = 0;
    }
    
//	g_turning_in_progress = 0;

//    // 停止转弯
//    Motor_SetSpeed(MOTOR1, 0);
//    Motor_SetSpeed(MOTOR2, 0);
//    Motor_SetSpeed(MOTOR3, 0);
//    Motor_SetSpeed(MOTOR4, 0);
//    
//	Delay_ms(50);
//	    Motor_SetSpeed(MOTOR1, -600);     // 左后轮正转，保持高速
//        Motor_SetSpeed(MOTOR2, -600);     // 右后轮反转  
//        Motor_SetSpeed(MOTOR3, 600);    // 左前轮正转
//        Motor_SetSpeed(MOTOR4, 600);    // 右前轮反转
//	Delay_ms(200);

    // 重置转弯状态
//    g_turning_in_progress = 0;
    
    printf("右转完成\r\n");
}

// 带转弯检测的巡线函数 - 使用定时器高频检测
void Line_Following_With_Turns(uint16_t speed, float kp, float kd) {
    // 检查定时器中断是否检测到转弯请求
    extern uint8_t g_turn_request;
    extern uint8_t g_turn_type;
    
    if(g_turn_request) {
        
        switch(g_turn_type) {
            case 1:  // 左转
                Execute_Left_Turn(speed);
                break;
                
            case 2:  // 右转
                Execute_Right_Turn(speed);
                break;
                
            case 3:  // 十字路口 - 默认右转
                // 先停止
                Motor_SetSpeed(MOTOR1, 0);
                Motor_SetSpeed(MOTOR2, 0);
                Motor_SetSpeed(MOTOR3, 0);
                Motor_SetSpeed(MOTOR4, 0);
				Delay_ms(200);
				Motor_SetSpeed(MOTOR1, -500);    // 左后轮反转，保持高速
				Motor_SetSpeed(MOTOR2, 500);    // 右后轮正转  
				Motor_SetSpeed(MOTOR3, 500);     // 左前轮反转
				Motor_SetSpeed(MOTOR4, -500);     // 右前轮正转
				Delay_ms(200);
			        Motor_SetSpeed(MOTOR1, 600);     // 左后轮正转
        Motor_SetSpeed(MOTOR2, 600);     // 右后轮反转  
        Motor_SetSpeed(MOTOR3, -600);    // 左前轮正转
        Motor_SetSpeed(MOTOR4, -600);    // 右前轮反转
			Delay_ms(300);
//				Motor_SetSpeed(MOTOR1, -500);    // 左后轮反转，保持高速
//				Motor_SetSpeed(MOTOR2, 500);    // 右后轮正转  
//				Motor_SetSpeed(MOTOR3, 500);     // 左前轮反转
//				Motor_SetSpeed(MOTOR4, -500);     // 右前轮正转
//				Delay_ms(200);
                printf("检测到十字路口，执行右转\r\n");
                Execute_Right_Turn(speed);
                break;
                
            default:
                break;
        }
		        g_turn_request = 0;  // 清除转弯请求

    } else {
        // 正常PID巡线
        Two_Gray_FourDrive_PD_Patrol(1, speed, kp, kd);
    }
}

void Simple_Line_Following_FourDrive(uint16_t speed) {
    // 根据校准结果设置阈值（黑白之间的中间值）
    uint16_t left_threshold = (3815 + 841) / 2;   // ≈ 2328
    uint16_t right_threshold = (3884 + 1254) / 2; // ≈ 2569
    
    uint16_t left = Analog_data[0];
    uint16_t right = Analog_data[1];
    
    // 根据实际电机配置：
    // MOTOR1(左后): +速度=前进, MOTOR2(右后): +速度=后退
    // MOTOR3(左前): +速度=后退, MOTOR4(右前): +速度=前进
    // 前进需要: M1=+, M2=-, M3=-, M4=+
    
    if(left < left_threshold && right < right_threshold) {
        // 两个都在黑线上 - 直行
        Motor_SetSpeed(MOTOR1, speed);   // 左后轮：正转前进
        Motor_SetSpeed(MOTOR2, -speed);  // 右后轮：反转前进  
        Motor_SetSpeed(MOTOR3, -speed);  // 左前轮：反转前进
        Motor_SetSpeed(MOTOR4, speed);   // 右前轮：正转前进
    }
    else if(left < left_threshold && right >= right_threshold) {
        // 左传感器在黑线上 - 左转（右轮快，左轮慢）
        Motor_SetSpeed(MOTOR1, speed/3);   // 左后轮减速
        Motor_SetSpeed(MOTOR2, -speed);    // 右后轮正常
        Motor_SetSpeed(MOTOR3, -speed/3);  // 左前轮减速
        Motor_SetSpeed(MOTOR4, speed);     // 右前轮正常
    }
    else if(left >= left_threshold && right < right_threshold) {
        // 右传感器在黑线上 - 右转（左轮快，右轮慢）
        Motor_SetSpeed(MOTOR1, speed);     // 左后轮正常
        Motor_SetSpeed(MOTOR2, -speed/3);  // 右后轮减速
        Motor_SetSpeed(MOTOR3, -speed);    // 左前轮正常
        Motor_SetSpeed(MOTOR4, speed/3);   // 右前轮减速
    }
    else {
        // 都没在黑线上 - 停止
        Motor_SetSpeed(MOTOR1, 0);
        Motor_SetSpeed(MOTOR2, 0);
        Motor_SetSpeed(MOTOR3, 0);
        Motor_SetSpeed(MOTOR4, 0);
    }
}