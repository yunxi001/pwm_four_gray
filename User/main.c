#include "stm32f10x.h"
#include "misc.h"
#include <stdio.h>
#include "Delay.h"
#include "Serial.h"
#include "Motor.h"
#include "Encoder.h"
#include "Two_gray_sensor.h"
#include "Two_Gray_Line_Following.h"
#include "Timer.h"  // 添加定时器头文件

uint8_t USART_TX_BUF[27]={0XAA,0X55,0X04,0X17,0X04,0X04,0X01,0X00,0X00,0X00,0X00,0X02,0X00,0X00,0X00,0X00,0X03,0X00,0X00,0X00,0X00,0X04,0X00,0X00,0X00,0X00,0X00};

// PWM控制变量
volatile uint16_t pwm_duty1 = 0, pwm_duty2 = 0, pwm_duty3 = 0, pwm_duty4 = 0;
volatile uint8_t motor_dir1 = 0, motor_dir2 = 0, motor_dir3 = 0, motor_dir4 = 0;
	
int encoder_buf[4];

void Motor_Position_Test(void);
int main(void) {
    // 初始化系统
    SystemClock_Config();      // 配置系统时钟72MHz
    SystemInit();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Serial_Init();             // 初始化串口
    PWM_Motors_Init();         // PWM初始化
    Two_gray_sensor_Init();    // 初始化两路灰度传感器
    Timer_Init();              // 初始化定时器，用于高频传感器检测

    printf("系统初始化完成 - 巡线模式\r\n");
    printf("定时器高频检测已启动 (5ms间隔)\r\n");
    
    // 传感器已校准，开始巡线
    Delay_ms(2000);  // 等待2秒开始巡线
    printf("开始巡线...\r\n");
    
    while(1) {
//        // 方案1：简单巡线（推荐初学者）
//        Simple_Line_Following_FourDrive(600);  // 速度600
        
//        // 方案2：PID巡线（更平滑，注释掉上面的Simple_Line_Following使用这个）
//        Two_Gray_FourDrive_PD_Patrol(1, 998, 6.0, 10.0);  // 白底黑线，速度600，kp=1.0，kd=2.5
        
        // 方案3：带转弯检测的巡线（使用定时器高频检测）
        Line_Following_With_Turns(999, 6.0, 10.0);  // 定时器5ms检测，十字路口自动右转
        
		Delay_ms(20);
//        // 调试输出（启用观察传感器数值和PID输出）
//        printf("L:%d R:%d\r\n", Analog_data[0], Analog_data[1]);
        
        // 不需要延时，让主循环尽可能快地执行PID控制
    }
}
// 电机测试函数 - 更详细的测试
void Motor_Position_Test(void) {
    printf("=== 电机位置和方向测试 ===\r\n");
    printf("逐个测试每个电机...\r\n");
    printf("观察哪个轮子转动以及转动方向\r\n");
    Delay_ms(2000);
    
    // 测试电机1
    printf("\r\n--- 测试 MOTOR1 ---\r\n");
    printf("MOTOR1 速度 +500 (如果正确应该是前进)\r\n");
    Motor_SetSpeed(MOTOR1, 500);
    Delay_ms(3000);
    Motor_SetSpeed(MOTOR1, 0);
    Delay_ms(1000);
    
    printf("MOTOR1 速度 -500 (如果正确应该是后退)\r\n");
    Motor_SetSpeed(MOTOR1, -500);
    Delay_ms(3000);
    Motor_SetSpeed(MOTOR1, 0);
    Delay_ms(2000);
    
    // 测试电机2
    printf("\r\n--- 测试 MOTOR2 ---\r\n");
    printf("MOTOR2 速度 +500 (如果正确应该是前进)\r\n");
    Motor_SetSpeed(MOTOR2, 500);
    Delay_ms(3000);
    Motor_SetSpeed(MOTOR2, 0);
    Delay_ms(1000);
    
    printf("MOTOR2 速度 -500 (如果正确应该是后退)\r\n");
    Motor_SetSpeed(MOTOR2, -500);
    Delay_ms(3000);
    Motor_SetSpeed(MOTOR2, 0);
    Delay_ms(2000);
    
    // 测试电机3
    printf("\r\n--- 测试 MOTOR3 ---\r\n");
    printf("MOTOR3 速度 +500 (如果正确应该是前进)\r\n");
    Motor_SetSpeed(MOTOR3, 500);
    Delay_ms(3000);
    Motor_SetSpeed(MOTOR3, 0);
    Delay_ms(1000);
    
    printf("MOTOR3 速度 -500 (如果正确应该是后退)\r\n");
    Motor_SetSpeed(MOTOR3, -500);
    Delay_ms(3000);
    Motor_SetSpeed(MOTOR3, 0);
    Delay_ms(2000);
    
    // 测试电机4
    printf("\r\n--- 测试 MOTOR4 ---\r\n");
    printf("MOTOR4 速度 +500 (如果正确应该是前进)\r\n");
    Motor_SetSpeed(MOTOR4, 500);
    Delay_ms(3000);
    Motor_SetSpeed(MOTOR4, 0);
    Delay_ms(1000);
    
    printf("MOTOR4 速度 -500 (如果正确应该是后退)\r\n");
    Motor_SetSpeed(MOTOR4, -500);
    Delay_ms(3000);
    Motor_SetSpeed(MOTOR4, 0);
    Delay_ms(2000);
    
    printf("\r\n=== 测试完成 ===\r\n");
    printf("请按以下格式记录结果:\r\n");
    printf("MOTOR1: 位置=[前左/前右/后左/后右], +500=[前进/后退]\r\n");
    printf("MOTOR2: 位置=[前左/前右/后左/后右], +500=[前进/后退]\r\n");
    printf("MOTOR3: 位置=[前左/前右/后左/后右], +500=[前进/后退]\r\n");
    printf("MOTOR4: 位置=[前左/前右/后左/后右], +500=[前进/后退]\r\n");
}

/*
 aa 55 0e 02 82 6d
输入指令2 (十六进制): aa 55 04 0f 01 04 01 43 00 02 43 00 03 43 00 04 43 00 d1
输入指令3 (十六进制): aa 55 04 0f 04 04 01 00 00 02 00 00 03 00 00 04 00 00 da
*/


//    while(1) {
//        if(USART_RX_STA) 
//				{
//            if(USART_RX_BUF[2] == 0x01) {
//                //Serial_SendArray(USART_RX_BUF, 16);
//                for(uint8_t i = 0; i < USART_RX_BUF[3]; i++) {
//                    int16_t speed = 0;
//                    speed = (USART_RX_BUF[5+i*3] | ((int16_t)USART_RX_BUF[6+i*3] << 8)) * 4;
//                    Motor_SetSpeed((Motor_ID)USART_RX_BUF[4+i*3], speed);
//                }
//            }
//            else if(USART_RX_BUF[2] == 0x04) {
//                uint8_t checksum = 0;
//                GET_Encoder(encoder_buf);
//                for(uint8_t i = 0; i < 4; i++) {
//                    USART_TX_BUF[7+i*5]   = (uint8_t)(encoder_buf[i] & 0xFF);
//                    USART_TX_BUF[7+i*5+1] = (uint8_t)((encoder_buf[i] >> 8) & 0xFF);
//                    USART_TX_BUF[7+i*5+2] = (uint8_t)((encoder_buf[i] >> 16) & 0xFF);
//                    USART_TX_BUF[7+i*5+3] = (uint8_t)((encoder_buf[i] >> 24) & 0xFF);
//                }
//                for(uint8_t i = 2; i <= 25; i++) {
//                    checksum += USART_TX_BUF[i];
//                }
//                checksum = ~checksum;
//                USART_TX_BUF[26] = checksum;
//                for(uint8_t i = 0; i < 27; i++) {
//                    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
//                    USART_SendData(USART1, USART_TX_BUF[i]);
//                }
//            }
//            USART_RX_STA = 0;
//        }
//    }

//			    // 测试电机2
//     Motor_SetSpeed(MOTOR1, 400);
//    Delay_ms(2000);
//         Motor_SetSpeed(MOTOR1, 0);
//    Delay_ms(1000);
//         Motor_SetSpeed(MOTOR1, -400);
//	    Delay_ms(2000);
//	Motor_SetSpeed(MOTOR1, 0);
//    Delay_ms(1000);
//	
//     Motor_SetSpeed(MOTOR3, 400);
//    Delay_ms(2000);
//         Motor_SetSpeed(MOTOR3, 0);
//    Delay_ms(1000);
//         Motor_SetSpeed(MOTOR3, -400);
//		     Delay_ms(2000);
//	Motor_SetSpeed(MOTOR3, 0);
//    Delay_ms(1000);     
//	
//	Motor_SetSpeed(MOTOR4, 400);
//    Delay_ms(2000);
//         Motor_SetSpeed(MOTOR4, 0);
//    Delay_ms(1000);
//         Motor_SetSpeed(MOTOR4, -400);
//	Delay_ms(2000);
//	Motor_SetSpeed(MOTOR4, 0);
//    Delay_ms(1000);

