#include "Digital.h"

void Digital_Init(void)
{
	// 1.开启数字传感器时钟
	Digital_CLK_ENABLE();
	// 2.定义GPIO结构体
	GPIO_InitTypeDef GPIO_InitStruct;
	// 3.结构体参数设置，采用下拉输入模式
	GPIO_InitStruct.GPIO_Pin = Digital_1_PIN | Digital_2_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	// 4.GPIOA端口引脚初始化
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void Read_Digital(uint8_t *Read_Data)
{
	Read_Data[1] = GPIO_ReadInputDataBit(GPIOA, Digital_1_PIN);
	Read_Data[2] = GPIO_ReadInputDataBit(GPIOA, Digital_2_PIN);
}
