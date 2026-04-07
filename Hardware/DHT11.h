#ifndef _DHT11_H
#define _DHT11_H

#include "stm32f10x.h"
#include "stdint.h"
#include "string.h"
#include "Delay.h"
#include <stdio.h>
#include "TM1637.h"


#define DHT11_IO_CLK_ENABLE()		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE)
#define DHT11_IO_PORT						GPIOC
#define DHT11_IO_PIN						GPIO_Pin_3

//#define DHT11_IO_CLK_ENABLE()		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE)
//#define DHT11_IO_PORT						GPIOC
//#define DHT11_IO_PIN						GPIO_Pin_0

uint8_t DHT11_Start(void);
uint8_t DHT11_Read_Byte(void);
void DHT_Read(void);

// 温湿度数据（由 DHT_Read() 更新）
extern volatile uint8_t dht11_temp;  // 温度整数部分
extern volatile uint8_t dht11_humi;  // 湿度整数部分

#endif
