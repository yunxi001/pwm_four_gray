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

	
int main(void) {
    // 初始化系统
    SystemClock_Config();      // 配置系统时钟72MHz
    SystemInit();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Serial_Init();             // 初始化串口
    PWM_Motors_Init();         // PWM初始化
    Two_gray_sensor_Init();    // 初始化两路灰度传感器

    printf("System initialized\r\n");
    
    // 传感器位置测试
    printf("Sensor Position Test\r\n");
    printf("Cover each sensor with your finger to identify position\r\n");
    printf("Format: Sensor0:%d Sensor1:%d\r\n");
    
    while(1) {
        printf("Sensor0:%d Sensor1:%d\r\n", Analog_data[0], Analog_data[1]);
        Delay_ms(300);  // 每300ms打印一次，方便观察
    }
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

