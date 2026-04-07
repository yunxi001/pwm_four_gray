#ifndef BRIGHTNESS_SENSOR_H
#define BRIGHTNESS_SENSOR_H
#include "stm32f10x.h"
#include <stdint.h>
#include <stdbool.h>

//Analog_1:PC0 ADC1通道10,Analog_2:PC1 ADC1通道11,Analog_3:PC2 ADC1通道12
//Analog_4：PA0 ADC1通道0,Analog_5：PA4 ADC1通道4，Analog_6:PA6 ADC1通道6
#define Analog_CLK_ENABLE()			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE)
#define Analog_1_PIN						GPIO_Pin_0
#define Analog_2_PIN						GPIO_Pin_1
#define Analog_5_PIN						GPIO_Pin_4
//#define Analog_6_PIN						GPIO_Pin_6

// 声明外部变量
extern volatile uint16_t Analog_data[3];
// 光照传感器原始ADC数据（s1对应bright_data[0]）
extern volatile uint16_t bright_data[1];

void Two_gray_sensor_Init(void);
void Two_gray_sensor_Test(void);

#endif
