#include "stm32f10x.h"

volatile uint64_t sys_tick_counter = 0;

void SysTick_Handler(void)
{
	sys_tick_counter++;
}

void Systick_Init(void)
{
	const uint32_t sys_load = SystemCoreClock / 1000 - 1;
	SysTick->LOAD = sys_load;
	SysTick->VAL  = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk|
									SysTick_CTRL_TICKINT_Msk |
									SysTick_CTRL_ENABLE_Msk;	
}

void delay_ms_AAA(uint32_t ms)
{
	uint32_t start = sys_tick_counter;
	while((sys_tick_counter - start) < ms);
}

/**
  * @brief  微秒级延时
  * @param  xus 延时时长，范围：0~233015
  * @retval 无
  */
void Delay_us(uint32_t xus)
{
	SysTick->LOAD = 72 * xus;				//设置定时器重装值
	SysTick->VAL = 0x00;					//清空当前计数值
	SysTick->CTRL = 0x00000005;				//设置时钟源为HCLK，启动定时器
	while(!(SysTick->CTRL & 0x00010000));	//等待计数到0
	SysTick->CTRL = 0x00000004;				//关闭定时器
}

/**
  * @brief  毫秒级延时
  * @param  xms 延时时长，范围：0~4294967295
  * @retval 无
  */
void Delay_ms(uint32_t xms)
{
	while(xms--)
	{
		Delay_us(1000);
	}
}
 
/**
  * @brief  秒级延时
  * @param  xs 延时时长，范围：0~4294967295
  * @retval 无
  */
void Delay_s(uint32_t xs)
{
	while(xs--)
	{
		Delay_ms(1000);
	}
} 
