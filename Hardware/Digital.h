#ifndef DIGITAL_H
#define DIGITAL_H

#include "stm32f10x.h"
#include "stdint.h"
#include "string.h"
#include "Delay.h"
#include <stdio.h>

#define Digital_CLK_ENABLE()		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC,ENABLE)
#define Digital_1_PIN						GPIO_Pin_0
#define Digital_2_PIN						GPIO_Pin_1
#define Digital_3_PIN						GPIO_Pin_5
#define Digital_4_PIN						GPIO_Pin_7

void Digital_Init(void);
void Read_Digital(uint8_t* Read_Data);

#endif
