#ifndef _TM1637_H
#define _TM1637_H

#include "stm32f10x.h"
#include "stdint.h"
#include "string.h"
#include "Delay.h"

#define TM1637_IO_CLK_ENABLE()	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE)
#define TM1637_CLK_PORT    			GPIOA
#define TM1637_CLK_PIN     			GPIO_Pin_4
#define TM1637_DIO_PORT    			GPIOA
#define TM1637_DIO_PIN    			GPIO_Pin_5
// 引脚电平操作宏（基于标准库兼容的寄存器操作，简化时序代码）
#define TM1637_CLK_HIGH()  GPIO_SetBits(TM1637_CLK_PORT, TM1637_CLK_PIN)
#define TM1637_CLK_LOW()   GPIO_ResetBits(TM1637_CLK_PORT, TM1637_CLK_PIN)
#define TM1637_DIO_HIGH()  GPIO_SetBits(TM1637_DIO_PORT, TM1637_DIO_PIN)
#define TM1637_DIO_LOW()   GPIO_ResetBits(TM1637_DIO_PORT, TM1637_DIO_PIN)
#define TM1637_DIO_READ()  GPIO_ReadInputDataBit(TM1637_DIO_PORT, TM1637_DIO_PIN)

static void TM1637_DelayUs(uint16_t us);
// 起始信号：CLK高电平时，DIO从高拉低
static void TM1637_Start(void);
// 停止信号：CLK高电平时，DIO从低拉高
static void TM1637_Stop(void);
// 写1个字节（低位在前），返回应答状态
static uint8_t TM1637_WriteByte(uint8_t byte);
void TM1637_Init(void);
// 设置亮度：level=0~7（0最暗，7最亮）
void TM1637_SetBrightness(uint8_t level);
// 清除显示（所有数码管熄灭）
void TM1637_ClearDisplay(void);
// 显示4位数字：num=0~9999，超出范围显示"----"
void TM1637_DisplayNumber(uint16_t num);
// 显示指定位置的字符（pos=0~3，对应千位~个位；seg_code=段码表索引）
void TM1637_DisplaySeg(uint8_t pos, uint8_t seg_code);

#endif


