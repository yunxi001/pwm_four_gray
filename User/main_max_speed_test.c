#include "stm32f10x.h"
#include "misc.h"
#include <stdio.h>
#include "Delay.h"
#include "Serial.h"

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

// 单个电机全速运行测试
void Run_Single_Motor_Max_Speed(uint8_t motor_id, uint16_t duration_ms) {
    printf("\r\n=== 电机%d 全速运行测试 ===\r\n", motor_id);
    
    // 启动电机全速正转
    switch(motor_id) {
        case 1: Motor1_Control(1); break;
        case 2: Motor2_Control(1); break;
        case 3: Motor3_Control(1); break;
        case 4: Motor4_Control(1); break;
    }
    
    printf("电机%d 全速正转中... (%d ms)\r\n", motor_id, duration_ms);
    
    // 运行指定时间
    Delay_ms(duration_ms);
    
    // 停止电机
    switch(motor_id) {
        case 1: Motor1_Control(0); break;
        case 2: Motor2_Control(0); break;
        case 3: Motor3_Control(0); break;
        case 4: Motor4_Control(0); break;
    }
    
    printf("电机%d 已停止\r\n", motor_id);
}

// 所有电机同时全速运行测试
void Run_All_Motors_Max_Speed(uint16_t duration_ms) {
    printf("\r\n=== 所有电机同时全速运行测试 ===\r\n");
    
    // 启动所有电机全速正转
    Motor1_Control(1);
    Motor2_Control(1);
    Motor3_Control(1);
    Motor4_Control(1);
    
    printf("所有电机全速正转中... (%d ms)\r\n", duration_ms);
    
    // 运行指定时间
    Delay_ms(duration_ms);
    
    // 停止所有电机
    Stop_All_Motors();
    
    printf("所有电机已停止\r\n");
}

int main(void) {
    // 系统初始化
    SystemClock_Config();
    SystemInit();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    // 初始化外设
    Serial_Init();
    Motor_GPIO_Init();  // 使用推挽输出模式

    printf("\r\n========================================\r\n");
    printf("     STM32 电机全速运行测试程序\r\n");
    printf("     使用推挽输出 1/0 控制模式\r\n");
    printf("========================================\r\n");
    
    Delay_ms(1000);  // 等待1秒准备
    
    // 逐个测试每个电机
    for(int i = 1; i <= 4; i++) {
        Run_Single_Motor_Max_Speed(i, 3000);  // 每个电机运行3秒
        Delay_ms(1000);  // 间隔1秒
    }
    
    printf("\r\n=== 开始所有电机同时运行测试 ===\r\n");
    Run_All_Motors_Max_Speed(5000);  // 所有电机一起运行5秒
    
    printf("\r\n=== 电机测试完成 ===\r\n");
    
    // 主循环 - 保持电机停止状态
    while(1) {
        Stop_All_Motors();
        Delay_ms(2000);
        printf("系统空闲中...\r\n");
    }
}