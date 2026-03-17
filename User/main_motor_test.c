#include "stm32f10x.h"
#include "misc.h"
#include <stdio.h>
#include "Delay.h"
#include "Serial.h"
#include "Motor.h"
#include "Encoder.h"
#include "Two_gray_sensor.h"
#include "Two_Gray_Line_Following.h"

uint8_t USART_TX_BUF[27]={0XAA,0X55,0X04,0X17,0X04,0X04,0X01,0X00,0X00,0X00,0X00,0X02,0X00,0X00,0X00,0X00,0X03,0X00,0X00,0X00,0X00,0X04,0X00,0X00,0X00,0X00,0X00};

// PWM控制变量
volatile uint16_t pwm_duty1 = 0, pwm_duty2 = 0, pwm_duty3 = 0, pwm_duty4 = 0;
volatile uint8_t motor_dir1 = 0, motor_dir2 = 0, motor_dir3 = 0, motor_dir4 = 0;
	
int encoder_buf[4];

// 电机测试函数 - 更详细的测试
void Motor_Position_Test(void) {
    printf("=== Motor Position and Direction Test ===\r\n");
    printf("Testing each motor individually...\r\n");
    printf("Observe which wheel moves and in what direction\r\n");
    Delay_ms(2000);
    
    // 测试电机1
    printf("\r\n--- Testing MOTOR1 ---\r\n");
    printf("MOTOR1 Speed +500 (should be forward if correct)\r\n");
    Motor_SetSpeed(MOTOR1, 500);
    Delay_ms(3000);
    Motor_SetSpeed(MOTOR1, 0);
    Delay_ms(1000);
    
    printf("MOTOR1 Speed -500 (should be backward if correct)\r\n");
    Motor_SetSpeed(MOTOR1, -500);
    Delay_ms(3000);
    Motor_SetSpeed(MOTOR1, 0);
    Delay_ms(2000);
    
    // 测试电机2
    printf("\r\n--- Testing MOTOR2 ---\r\n");
    printf("MOTOR2 Speed +500 (should be forward if correct)\r\n");
    Motor_SetSpeed(MOTOR2, 500);
    Delay_ms(3000);
    Motor_SetSpeed(MOTOR2, 0);
    Delay_ms(1000);
    
    printf("MOTOR2 Speed -500 (should be backward if correct)\r\n");
    Motor_SetSpeed(MOTOR2, -500);
    Delay_ms(3000);
    Motor_SetSpeed(MOTOR2, 0);
    Delay_ms(2000);
    
    // 测试电机3
    printf("\r\n--- Testing MOTOR3 ---\r\n");
    printf("MOTOR3 Speed +500 (should be forward if correct)\r\n");
    Motor_SetSpeed(MOTOR3, 500);
    Delay_ms(3000);
    Motor_SetSpeed(MOTOR3, 0);
    Delay_ms(1000);
    
    printf("MOTOR3 Speed -500 (should be backward if correct)\r\n");
    Motor_SetSpeed(MOTOR3, -500);
    Delay_ms(3000);
    Motor_SetSpeed(MOTOR3, 0);
    Delay_ms(2000);
    
    // 测试电机4
    printf("\r\n--- Testing MOTOR4 ---\r\n");
    printf("MOTOR4 Speed +500 (should be forward if correct)\r\n");
    Motor_SetSpeed(MOTOR4, 500);
    Delay_ms(3000);
    Motor_SetSpeed(MOTOR4, 0);
    Delay_ms(1000);
    
    printf("MOTOR4 Speed -500 (should be backward if correct)\r\n");
    Motor_SetSpeed(MOTOR4, -500);
    Delay_ms(3000);
    Motor_SetSpeed(MOTOR4, 0);
    Delay_ms(2000);
    
    printf("\r\n=== Test Complete ===\r\n");
    printf("Please record results in this format:\r\n");
    printf("MOTOR1: Position=[front-left/front-right/back-left/back-right], +500=[forward/backward]\r\n");
    printf("MOTOR2: Position=[front-left/front-right/back-left/back-right], +500=[forward/backward]\r\n");
    printf("MOTOR3: Position=[front-left/front-right/back-left/back-right], +500=[forward/backward]\r\n");
    printf("MOTOR4: Position=[front-left/front-right/back-left/back-right], +500=[forward/backward]\r\n");
}

int main(void) {
    // 初始化系统
    SystemClock_Config();      // 配置系统时钟72MHz
    SystemInit();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Serial_Init();             // 初始化串口
    PWM_Motors_Init();         // PWM初始化
    Two_gray_sensor_Init();    // 初始化两路灰度传感器

    printf("System initialized\r\n");
    
    // 电机位置和方向测试
    Delay_ms(3000);  // 等待3秒准备
    Motor_Position_Test();
    
    // 测试完成后停止所有电机
    while(1) {
        Motor_SetSpeed(MOTOR1, 0);
        Motor_SetSpeed(MOTOR2, 0);
        Motor_SetSpeed(MOTOR3, 0);
        Motor_SetSpeed(MOTOR4, 0);
        Delay_ms(1000);
        printf("All motors stopped\r\n");
    }
}