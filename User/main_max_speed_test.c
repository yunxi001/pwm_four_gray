#include "stm32f10x.h"
#include "misc.h"
#include <stdio.h>
#include "Delay.h"
#include "Serial.h"
#include "Encoder.h"

// 电机引脚定义
// 电机1: PC8(M+), PC7(M-)
// 电机2: PB14(M+), PB9(M-)  
// 电机3: PA8(M+), PC9(M-)
// 电机4: PC6(M+), PB15(M-)

// 初始化所有电机引脚为推挽输出模式
void Motor_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | 
                           RCC_APB2Periph_GPIOC, ENABLE);
    
    // 配置为推挽输出，50MHz速度
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    // 电机1引脚: PC8, PC7
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_7;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    // 电机2引脚: PB14, PB9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_9;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // 电机3引脚: PA8, PC9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    // 电机4引脚: PC6, PB15
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // 初始化所有引脚为低电平（停止状态）
    GPIO_ResetBits(GPIOC, GPIO_Pin_8 | GPIO_Pin_7 | GPIO_Pin_9 | GPIO_Pin_6);
    GPIO_ResetBits(GPIOB, GPIO_Pin_14 | GPIO_Pin_9 | GPIO_Pin_15);
    GPIO_ResetBits(GPIOA, GPIO_Pin_8);
    
    printf("电机GPIO初始化完成 - 推挽输出模式\r\n");
}

// 停止所有电机
void Stop_All_Motors(void) {
    GPIO_ResetBits(GPIOC, GPIO_Pin_8 | GPIO_Pin_7 | GPIO_Pin_9 | GPIO_Pin_6);
    GPIO_ResetBits(GPIOB, GPIO_Pin_14 | GPIO_Pin_9 | GPIO_Pin_15);
    GPIO_ResetBits(GPIOA, GPIO_Pin_8);
}

// 电机1控制 (PC8+, PC7-)
void Motor1_Control(uint8_t direction) {
    if (direction == 1) {  // 正转
        GPIO_SetBits(GPIOC, GPIO_Pin_8);    // M+ = 1
        GPIO_ResetBits(GPIOC, GPIO_Pin_7);  // M- = 0
    } else if (direction == 2) {  // 反转
        GPIO_ResetBits(GPIOC, GPIO_Pin_8);  // M+ = 0
        GPIO_SetBits(GPIOC, GPIO_Pin_7);    // M- = 1
    } else {  // 停止
        GPIO_ResetBits(GPIOC, GPIO_Pin_8 | GPIO_Pin_7);
    }
}

// 电机2控制 (PB14+, PB9-)
void Motor2_Control(uint8_t direction) {
    if (direction == 1) {  // 正转
        GPIO_SetBits(GPIOB, GPIO_Pin_14);   // M+ = 1
        GPIO_ResetBits(GPIOB, GPIO_Pin_9);  // M- = 0
    } else if (direction == 2) {  // 反转
        GPIO_ResetBits(GPIOB, GPIO_Pin_14); // M+ = 0
        GPIO_SetBits(GPIOB, GPIO_Pin_9);    // M- = 1
    } else {  // 停止
        GPIO_ResetBits(GPIOB, GPIO_Pin_14 | GPIO_Pin_9);
    }
}

// 电机3控制 (PA8+, PC9-)
void Motor3_Control(uint8_t direction) {
    if (direction == 1) {  // 正转
        GPIO_SetBits(GPIOA, GPIO_Pin_8);    // M+ = 1
        GPIO_ResetBits(GPIOC, GPIO_Pin_9);  // M- = 0
    } else if (direction == 2) {  // 反转
        GPIO_ResetBits(GPIOA, GPIO_Pin_8);  // M+ = 0
        GPIO_SetBits(GPIOC, GPIO_Pin_9);    // M- = 1
    } else {  // 停止
        GPIO_ResetBits(GPIOA, GPIO_Pin_8);
        GPIO_ResetBits(GPIOC, GPIO_Pin_9);
    }
}

// 电机4控制 (PC6+, PB15-)
void Motor4_Control(uint8_t direction) {
    if (direction == 1) {  // 正转
        GPIO_SetBits(GPIOC, GPIO_Pin_6);     // M+ = 1
        GPIO_ResetBits(GPIOB, GPIO_Pin_15);  // M- = 0
    } else if (direction == 2) {  // 反转
        GPIO_ResetBits(GPIOC, GPIO_Pin_6);   // M+ = 0
        GPIO_SetBits(GPIOB, GPIO_Pin_15);    // M- = 1
    } else {  // 停止
        GPIO_ResetBits(GPIOC, GPIO_Pin_6);
        GPIO_ResetBits(GPIOB, GPIO_Pin_15);
    }
}
// 单个电机最大转速测试
void Test_Single_Motor_Max_Speed(uint8_t motor_id) {
    int encoder_buf[4];
    int32_t encoder_count_start, encoder_count_end;
    float rpm;
    
    printf("\r\n=== 电机%d 最大转速测试 ===\r\n", motor_id);
    
    // 清零编码器
    RESET_Encoder();
    Delay_ms(100);
    
    // 获取初始编码器值
    GET_Encoder(encoder_buf);
    encoder_count_start = encoder_buf[motor_id - 1];  // 数组索引从0开始
    
    // 启动电机全速正转
    switch(motor_id) {
        case 1: Motor1_Control(1); break;
        case 2: Motor2_Control(1); break;
        case 3: Motor3_Control(1); break;
        case 4: Motor4_Control(1); break;
    }
    
    printf("电机%d 全速正转中...\r\n", motor_id);
    
    // 运行5秒测量转速
    Delay_ms(5000);
    
    // 获取结束编码器值
    GET_Encoder(encoder_buf);
    encoder_count_end = encoder_buf[motor_id - 1];
    
    // 停止电机
    switch(motor_id) {
        case 1: Motor1_Control(0); break;
        case 2: Motor2_Control(0); break;
        case 3: Motor3_Control(0); break;
        case 4: Motor4_Control(0); break;
    }
    
    // 计算转速 (根据编码器代码，每转约26*90=2340个脉冲)
    int32_t pulse_count = encoder_count_end - encoder_count_start;
    float revolutions = (float)pulse_count / 2340.0f;  // 转数
    rpm = (revolutions / 5.0f) * 60.0f;  // RPM = (转数/时间秒) * 60
    
    printf("电机%d 测试结果:\r\n", motor_id);
    printf("  编码器脉冲: %ld -> %ld (差值: %ld)\r\n", 
           encoder_count_start, encoder_count_end, pulse_count);
    printf("  转数: %.2f 转\r\n", revolutions);
    printf("  转速: %.2f RPM\r\n", rpm);
    
    Delay_ms(2000);
}

// 所有电机同时运行最大转速测试
void Test_All_Motors_Max_Speed(void) {
    int encoder_buf[4];
    int32_t encoder_start[4], encoder_end[4];  // 数组索引0-3对应电机1-4
    float rpm[4];
    
    printf("\r\n=== 所有电机同时最大转速测试 ===\r\n");
    
    // 清零编码器
    RESET_Encoder();
    Delay_ms(100);
    
    // 获取初始编码器值
    GET_Encoder(encoder_buf);
    for(int i = 0; i < 4; i++) {
        encoder_start[i] = encoder_buf[i];
    }
    
    // 启动所有电机全速正转
    Motor1_Control(1);
    Motor2_Control(1);
    Motor3_Control(1);
    Motor4_Control(1);
    
    printf("所有电机全速正转中...\r\n");
    
    // 运行5秒测量转速
    Delay_ms(5000);
    
    // 获取结束编码器值
    GET_Encoder(encoder_buf);
    for(int i = 0; i < 4; i++) {
        encoder_end[i] = encoder_buf[i];
    }
    
    // 停止所有电机
    Stop_All_Motors();
    
    // 计算并显示结果
    printf("\r\n=== 所有电机测试结果 ===\r\n");
    for(int i = 0; i < 4; i++) {
        int32_t pulse_count = encoder_end[i] - encoder_start[i];
        float revolutions = (float)pulse_count / 2340.0f;  // 每转2340个脉冲
        rpm[i] = (revolutions / 5.0f) * 60.0f;
        
        printf("电机%d: 脉冲=%ld, 转数=%.2f, 转速=%.2f RPM\r\n", 
               i+1, pulse_count, revolutions, rpm[i]);
    }
    
    // 找出最快和最慢的电机
    float max_rpm = rpm[0], min_rpm = rpm[0];
    int max_motor = 1, min_motor = 1;
    
    for(int i = 1; i < 4; i++) {
        if(rpm[i] > max_rpm) {
            max_rpm = rpm[i];
            max_motor = i + 1;
        }
        if(rpm[i] < min_rpm) {
            min_rpm = rpm[i];
            min_motor = i + 1;
        }
    }
    
    printf("\r\n最快电机: 电机%d (%.2f RPM)\r\n", max_motor, max_rpm);
    printf("最慢电机: 电机%d (%.2f RPM)\r\n", min_motor, min_rpm);
    printf("转速差异: %.2f RPM\r\n", max_rpm - min_rpm);
}

// 电压对转速影响测试（通过PWM模拟不同电压）
void Test_Voltage_Speed_Relationship(uint8_t motor_id) {
    printf("\r\n=== 电机%d 电压-转速关系测试 ===\r\n", motor_id);
    printf("注意：此测试使用数字1/0控制，无法模拟不同电压\r\n");
    printf("如需测试不同电压下的转速，需要使用PWM控制或外部调压\r\n");
    
    // 这里只能测试满电压下的转速
    Test_Single_Motor_Max_Speed(motor_id);
}

int main(void) {
    // 系统初始化
    SystemClock_Config();
    SystemInit();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    // 初始化外设
    Serial_Init();
    Motor_GPIO_Init();  // 使用推挽输出模式
    Encoder_EXTI_Init();     // 初始化编码器
    
    printf("\r\n========================================\r\n");
    printf("     STM32 电机最大转速测试程序\r\n");
    printf("     使用推挽输出 1/0 控制模式\r\n");
    printf("========================================\r\n");
    
    Delay_ms(3000);  // 等待3秒准备
    
    // 测试菜单
    printf("\r\n请选择测试模式:\r\n");
    printf("1. 单个电机测试 (逐个测试每个电机)\r\n");
    printf("2. 所有电机同时测试\r\n");
    printf("3. 连续循环测试\r\n");
    
    // 自动执行所有测试
    printf("\r\n开始自动测试...\r\n");
    
    // 1. 逐个测试每个电机
    for(int i = 1; i <= 4; i++) {
        Test_Single_Motor_Max_Speed(i);
    }
    
    // 2. 所有电机同时测试
    Test_All_Motors_Max_Speed();
    
    // 3. 连续循环测试（观察稳定性）
    printf("\r\n=== 连续循环测试 (5次) ===\r\n");
    for(int loop = 1; loop <= 5; loop++) {
        printf("\r\n--- 第%d次循环测试 ---\r\n", loop);
        Test_All_Motors_Max_Speed();
        Delay_ms(2000);
    }
    
    printf("\r\n=== 所有测试完成 ===\r\n");
    printf("测试结论:\r\n");
    printf("1. 推挽输出1/0控制下的最大转速已测量\r\n");
    printf("2. 各电机转速差异已记录\r\n");
    printf("3. 系统稳定性已验证\r\n");
    
    // 主循环 - 保持电机停止状态
    while(1) {
        Stop_All_Motors();
        Delay_ms(1000);
        printf("所有电机已停止\r\n");
    }
}