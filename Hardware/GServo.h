#ifndef _GSERVO_H
#define _GSERVO_H

#include "stm32f10x.h"
#include "Delay.h"

// 接收缓冲区大小
#define RX_BUFFER_SIZE 64

// RS485 串口引脚定义
#define S485_IO_CLK_ENABLE()        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE)
#define S485_IO_PORT                GPIOA
#define S485_TX_PIN                 GPIO_Pin_2
#define S485_RX_PIN                 GPIO_Pin_3

// RS485 方向控制引脚定义
#define S485_DIR_CLK_ENABLE()       RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE)
#define S485_DIR_PORT               GPIOC
#define S485_DIR_PIN                GPIO_Pin_4

// 全局变量声明
extern uint8_t rx_buffer[RX_BUFFER_SIZE];
extern uint8_t rx_count;
extern uint8_t rx_flag;

// 函数声明
void Gservo_IRQHandler(void);
void GServo_Init(void);
void S485_Dir_Control(uint8_t dir);
void S485_Senddata(uint8_t* data, uint8_t length);
uint8_t S485_ReceiveByte(void);
uint8_t Parity_Check(uint8_t *data);
void Set_Servo_position(uint8_t id, uint16_t position, uint8_t speed);

// 保留原有函数声明（可选）
uint16_t SERVO_ReadPosition(uint8_t id);
void SERVO_SetPosition_angle(uint8_t id, uint16_t angle);
void SERVO_SetPosition(uint8_t id, uint16_t position_L, uint16_t position_H);
uint8_t SERVO_ReadID(void);
void SERVO_SetID(uint8_t id, uint8_t tar_id);

// 新增函数声明
uint8_t SERVO_ScanIDs(uint8_t* found_ids, uint8_t max_count);
uint16_t SERVO_ReadPosition_Enhanced(uint8_t id);

#endif
