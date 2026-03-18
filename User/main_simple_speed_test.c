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
    
    printf("电机GPIO初始化完成 - 推挽输出模式，50MHz\r\n");
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

// 简单的电机测试函数
void Simple_Motor_Test(void) {
    printf("\r\n=== 简单电机最大转速测试 ===\r\n");
    printf("使用推挽输出1/0控制，测试电机最快转速\r\n");
    
    // 逐个测试每个电机
    for(int motor = 1; motor <= 4; motor++) {
        printf("\r\n--- 测试电机%d ---\r\n", motor);
        
        // 正转测试
        printf("电机%d 正转 (M+=1, M-=0)\r\n", motor);
        switch(motor) {
            case 1: Motor1_Control(1); break;
            case 2: Motor2_Control(1); break;
            case 3: Motor3_Control(1); break;
            case 4: Motor4_Control(1); break;
        }
        Delay_ms(3000);  // 运行3秒
        
        // 停止
        switch(motor) {
            case 1: Motor1_Control(0); break;
            case 2: Motor2_Control(0); break;
            case 3: Motor3_Control(0); break;
            case 4: Motor4_Control(0); break;
        }
        printf("电机%d 停止\r\n", motor);
        Delay_ms(1000);
        
        // 反转测试
        printf("电机%d 反转 (M+=0, M-=1)\r\n", motor);
        switch(motor) {
            case 1: Motor1_Control(2); break;
            case 2: Motor2_Control(2); break;
            case 3: Motor3_Control(2); break;
            case 4: Motor4_Control(2); break;
        }
        Delay_ms(3000);  // 运行3秒
        
        // 停止
        switch(motor) {
            case 1: Motor1_Control(0); break;
            case 2: Motor2_Control(0); break;
            case 3: Motor3_Control(0); break;
            case 4: Motor4_Control(0); break;
        }
        printf("电机%d 停止\r\n", motor);
        Delay_ms(2000);
    }
}

// 所有电机同时运行测试
void All_Motors_Test(void) {
    printf("\r\n=== 所有电机同时运行测试 ===\r\n");
    
    // 所有电机同时正转
    printf("所有电机同时正转...\r\n");
    Motor1_Control(1);
    Motor2_Control(1);
    Motor3_Control(1);
    Motor4_Control(1);
    Delay_ms(5000);  // 运行5秒
    
    // 停止所有电机
    Stop_All_Motors();
    printf("所有电机停止\r\n");
    Delay_ms(2000);
    
    // 所有电机同时反转
    printf("所有电机同时反转...\r\n");
    Motor1_Control(2);
    Motor2_Control(2);
    Motor3_Control(2);
    Motor4_Control(2);
    Delay_ms(5000);  // 运行5秒
    
    // 停止所有电机
    Stop_All_Motors();
    printf("所有电机停止\r\n");
    Delay_ms(2000);
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
    printf("     STM32 电机最大转速测试程序\r\n");
    printf("     推挽输出模式 - 1/0 数字控制\r\n");
    printf("     系统时钟: 72MHz\r\n");
    printf("     GPIO速度: 50MHz\r\n");
    printf("========================================\r\n");
    
    Delay_ms(3000);  // 等待3秒准备
    
    printf("\r\n开始测试...\r\n");
    printf("请观察电机转速并记录结果\r\n");
    
    // 执行测试
    Simple_Motor_Test();    // 单个电机测试
    All_Motors_Test();      // 所有电机同时测试
    
    printf("\r\n=== 测试完成 ===\r\n");
    printf("测试结论:\r\n");
    printf("1. 推挽输出1/0控制下的电机已测试\r\n");
    printf("2. 这是电机在当前电压下的最大转速\r\n");
    printf("3. 无PWM调制，电机获得满电压驱动\r\n");
    printf("4. 请记录观察到的转速情况\r\n");
    
    // 主循环 - 保持电机停止状态
    while(1) {
        Stop_All_Motors();
        Delay_ms(1000);
        printf("系统运行中，所有电机已停止\r\n");
    }
}