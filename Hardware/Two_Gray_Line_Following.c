#include "Delay.h"
#include "Motor.h"
#include "Serial.h"
#include "Timer.h" // 添加定时器头文件
#include "Two_gray_sensor.h"
#include <math.h>
#include <stdio.h>

// 声明外部变量
extern volatile uint16_t Analog_data[3];
extern volatile uint8_t g_turning_in_progress;

// 传感器校准函数 - 用于确定归一化参数
void Calibrate_Gray_Sensors(void)
{
    uint16_t white_left = 0, white_right = 0;
    uint16_t black_left = 4095, black_right = 4095;

    printf("Start sensor calibration...\r\n");
    printf("Place sensors on WHITE area, start in 5 seconds\r\n");
    Delay_ms(5000);

    // 采集白色区域数据
    for (int i = 0; i < 100; i++)
    {
        if (Analog_data[0] > white_left)
            white_left = Analog_data[0];
        if (Analog_data[1] > white_right)
            white_right = Analog_data[1];
        Delay_ms(10);
    }

    printf("Place sensors on BLACK line, start in 5 seconds\r\n");
    Delay_ms(5000);

    // 采集黑线数据
    for (int i = 0; i < 100; i++)
    {
        if (Analog_data[0] < black_left)
            black_left = Analog_data[0];
        if (Analog_data[1] < black_right)
            black_right = Analog_data[1];
        Delay_ms(10);
    }

    printf("Calibration complete:\r\n");
    printf("Left sensor: White=%d, Black=%d\r\n", white_left, black_left);
    printf("Right sensor: White=%d, Black=%d\r\n", white_right, black_right);
}

/**************
Two_Gray_FourDrive_PD_Patrol 函数为四驱二灰巡线PID
根据新车电机配置调整：
- MOTOR1(右前): +速度=后退, MOTOR2(左前): +速度=前进
- MOTOR3(右后): +速度=前进, MOTOR4(左后): +速度=后退
- 前进需要: M1=-, M2=+, M3=+, M4=-
***************** */
void Two_Gray_FourDrive_PD_Patrol(uint8_t map, uint16_t speed, float kp, float kd)
{
    static float last_error = 0;
    static float error = 0;
    static float output = 0;

    // 根据校准结果设置的参数
    // 左传感器: 白色≈3815, 黑色≈841
    // 右传感器: 白色≈3884, 黑色≈1254
    //    static const uint16_t left_white  = 3400;
    //    static const uint16_t left_black  = 800 ;
    //    static const uint16_t right_white = 3400 ;
    //    static const uint16_t right_black = 800  ;
    static const uint16_t left_white = 4095;
    static const uint16_t left_black = 1100;
    static const uint16_t right_white = 4095;
    static const uint16_t right_black = 1100;
    // 分别归一化每个传感器（0=黑线，1=白色）
    float left_norm = (float)(Analog_data[1] - left_black) / (left_white - left_black);
    float right_norm = (float)(Analog_data[0] - right_black) / (right_white - right_black);

    // 限制归一化值在0-1范围内
    if (left_norm < 0)
        left_norm = 0;
    if (left_norm > 1)
        left_norm = 1;
    if (right_norm < 0)
        right_norm = 0;
    if (right_norm > 1)
        right_norm = 1;

    // 计算误差
    if (map == 1)
    {
        // 白底黑线模式
        error = 510 * (right_norm - left_norm);
    }
    else if (map == 2)
    {
        // 黑底白线模式
        error = 510 * (left_norm - right_norm);
    }

    // 误差死区
    const int ERROR_DEAD_ZONE = 100;
    if (fabs(error) < ERROR_DEAD_ZONE)
    {
        error = 0; // 微小误差视为0，不调整
    }

    // PD控制输出
    output = kp * error + kd * (error - last_error);

    // 限制输出范围，防止电机速度过大
    if (output > speed)
        output = speed;
    if (output < -speed)
        output = -speed;

    // 电机控制（根据新车配置调整）
    // 左轮组速度：基础速度 - 输出（左转时减速）
    int16_t left_speed_base = speed - (int16_t)output;
    // 右轮组速度：基础速度 + 输出（左转时加速）
    int16_t right_speed_base = speed + (int16_t)output;

    // 应用到各个车轮，使用新的车轮控制接口
    Set_All_Wheels(left_speed_base,   // 左前轮
                   right_speed_base,  // 右前轮
                   left_speed_base,   // 左后轮
                   right_speed_base); // 右后轮

    last_error = error;

// 调试输出（可选）
#ifdef DEBUG_LINE_FOLLOWING
    printf("L:%d R:%d Ln:%.2f Rn:%.2f E:%.1f O:%.1f LS:%d RS:%d\r\n",
           Analog_data[0],
           Analog_data[1],
           left_norm,
           right_norm,
           error,
           output,
           left_speed_base,
           right_speed_base);
#endif
}

// 执行左转90度 - 原地转弯，快速响应
void Execute_Left_Turn(uint16_t speed)
{
    //    uint16_t left_threshold  = (3815 + 841) / 2;    // ≈ 2328
    //    uint16_t right_threshold = (3884 + 1254) / 2;   // ≈ 2569
    static uint16_t right_threshold = (4095 + 1100) / 2;
    static uint16_t left_threshold = (4095 + 1100) / 2;
    // 原地右转：左轮正转，右轮反转，速度更快
    // 第一阶段：快速转到左传感器离开黑线
    uint8_t turn_timeout = 0;
    while (Analog_data[0] < right_threshold || turn_timeout < 30) // 左传感器还在黑线上
    {
        Car_Turn_Left(500);
        Delay_ms(5); // 很短的延时
        turn_timeout++;
    }

    // 第二阶段：继续转到右传感器找到新的黑线
    while (Analog_data[1] >= left_threshold)
    {
        Car_Turn_Left(500);
        Delay_ms(5);
    }

    // 停止转弯
    Car_Stop();
    Delay_ms(50);

    // 左转一点，调正方向
    Car_Turn_Left(500);
    Delay_ms(20);

    // 停止转弯
    Car_Stop();
    Delay_ms(50);
    // 重置转弯状态
    g_turning_in_progress = 0;
    //    printf("执行左转\r\n");

    //    uint16_t left_threshold  = (3815 + 841) / 2;    // ≈ 2328
    //    uint16_t right_threshold = (3884 + 1254) / 2;   // ≈ 2569

    //    // 原地左转：左轮反转，右轮正转
    //    // 第一阶段：强制转够最小角度（不管传感器状态）
    //    uint8_t turn_timeout = 0;
    //    while (turn_timeout < 100)   // 100 * 5ms = 500ms，根据实际调整
    //    {
    //        Car_Turn_Left(speed);
    //        Delay_ms(5);
    //        turn_timeout++;
    //    }

    //    // 第二阶段：继续转到右传感器找到新的黑线
    //    turn_timeout = 0;
    //    while (Analog_data[1] >= right_threshold && turn_timeout < 80)   // 右传感器不在黑线上，继续转
    //    {
    //        Car_Turn_Left(speed * 0.6);  // 降速
    //        Delay_ms(5);
    //        turn_timeout++;
    //    }

    //    // 停止转弯
    //    Car_Stop();
    //    Delay_ms(50);

    //    // 右转一点，调正方向
    //    Car_Turn_Right(speed * 0.6);
    //    Delay_ms(200);

    //    // 重置转弯状态
    //    g_turning_in_progress = 0;

    //    printf("左转完成\r\n");
}

// 执行右转90度 - 原地转弯，快速响应
void Execute_Right_Turn(uint16_t speed)
{
    //    uint16_t left_threshold  = (3815 + 841) / 2;    // ≈ 2328
    //    uint16_t right_threshold = (3884 + 1254) / 2;   // ≈ 2569
    static uint16_t right_threshold = (4095 + 1100) / 2;
    static uint16_t left_threshold = (4095 + 1100) / 2;
    // 原地右转：左轮正转，右轮反转，速度更快
    // 第一阶段：快速转到左传感器离开黑线
    uint16_t turn_timeout = 0;
    while (Analog_data[1] < left_threshold || turn_timeout < 50) // 左传感器还在黑线上
    {
        Car_Turn_Right(500);

        //		Set_All_Wheels(500, -600, 500, -600);
        Delay_ms(5); // 很短的延时
        turn_timeout++;
    }

    // 第二阶段：继续转到右传感器找到新的黑线
    while (Analog_data[0] >= right_threshold)
    {
        Car_Turn_Right(500);
        //				Set_All_Wheels(500, -600, 500, -600);

        Delay_ms(5);
    }

    // 停止转弯
    Car_Stop();
    Delay_ms(50);

    //    // 左转一点，调正方向
    //    Car_Turn_Right(500);
    //    Delay_ms(10);

    // 停止转弯
    Car_Stop();
    Delay_ms(50);
    // 重置转弯状态
    g_turning_in_progress = 0;
}

// 带转弯检测的巡线函数 - 使用定时器高频检测
void Line_Following_With_Turns(uint16_t speed, float kp, float kd)
{
    // 检查定时器中断是否检测到转弯请求
    extern uint8_t g_turn_type;

    if (g_turning_in_progress)
    {
        switch (g_turn_type)
        {
        case 1: // 左转
            Execute_Left_Turn(speed);
            break;
        case 2: // 右转
            Execute_Right_Turn(speed);
            break;
        case 3: // 十字路口 - 默认右转
            // 先停止
            Car_Stop();
            Delay_ms(100);
            //            // 回退一点距离
            Car_Move_Backward(600);
            Delay_ms(190);
            //            Delay_ms(150);
            //		    Car_Stop();
            //            Delay_ms(100);
            Execute_Right_Turn(speed);
            //		Execute_Left_Turn(speed);
            break;

        default:
            break;
        }
    }
    else
    {
        // 正常PID巡线
        Two_Gray_FourDrive_PD_Patrol(1, speed, kp, kd);
        Delay_ms(10);
    }
}

/*
 * 函数功能：实现基于双灰度传感器的简单循线算法，控制四驱车运动
 * 参数说明：
 *   speed: 电机基础运行速度，正值表示前进方向的速度值
 * 返回值：无
 * 实现逻辑：通过读取左右两个灰度传感器的值，与预设阈值比较，判断车辆相对于黑线的位置，
 *         并控制四个电机以不同的速度组合实现循线动作
 */
void Simple_Line_Following_FourDrive(uint16_t speed)
{
    // 根据校准结果设置阈值（黑白之间的中间值）
    uint16_t left_threshold = (3862 + 1380) / 2 - 100;
    uint16_t right_threshold = (3863 + 1030) / 2 - 100;

    uint16_t left = Analog_data[0];
    uint16_t right = Analog_data[1];

    // 根据新车电机配置：
    // MOTOR1(右前): +速度=后退, MOTOR2(左前): +速度=前进
    // MOTOR3(右后): +速度=前进, MOTOR4(左后): +速度=后退
    // 前进需要: M1=-, M2=+, M3=+, M4=-

    if (left < left_threshold && right < right_threshold)
    {
        // 两个都在黑线上 - 直行
        Car_Move_Forward(speed);
    }
    else if (left < left_threshold && right >= right_threshold)
    {
        // 左传感器在黑线上 - 左转（右轮快，左轮慢）
        Set_All_Wheels(speed / 3, // 左前轮减速
                       speed,     // 右前轮正常
                       speed / 3, // 左后轮减速
                       speed);    // 右后轮正常
    }
    else if (left >= left_threshold && right < right_threshold)
    {
        // 右传感器在黑线上 - 右转（左轮快，右轮慢）
        Set_All_Wheels(speed,      // 左前轮正常
                       speed / 3,  // 右前轮减速
                       speed,      // 左后轮正常
                       speed / 3); // 右后轮减速
    }
    else
    {
        // 都没在黑线上 - 停止
        Car_Stop();
    }
}
