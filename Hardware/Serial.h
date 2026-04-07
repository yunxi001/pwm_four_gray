#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdio.h>

#define S485_IO_CLK_ENABLE()		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE)
#define	S485_IO_PORT						GPIOA
#define	S485_TX_PIN							GPIO_Pin_2
#define S485_RX_PIN							GPIO_Pin_3	
#define S485_DIR_CLK_ENABLE()		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE)
#define S485_DIR_PORT						GPIOC
#define S485_DIR_PIN						GPIO_Pin_4 
void S485_Init(uint32_t bauds);
void S485_Dir_Control(uint8_t state);

extern char Serial_RxPacket[];
extern uint8_t Serial_RxFlag;

void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String);
void Serial_SendNumber(uint32_t Number, uint8_t Length);
void Serial_Printf(char *format, ...);

#define USART_REC_LEN  			200  	//定义最大接收字节数 200
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u8 USART_RX_STA;         		//接收状态标记	
//如果想串口中断接收，请不要注释以下宏定义
void USART1_CONTROL(uint8_t data);

#endif
