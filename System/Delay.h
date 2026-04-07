#ifndef __DELAY_H
#define __DELAY_H
#include "stm32f10x.h"

#include <stdint.h>
extern uint64_t sys_tick_counter;
void Delay_us(uint32_t us);
void Delay_ms(uint32_t ms);
void Delay_s(uint32_t s);
void Systick_Init(void);
void delay_ms_AAA(uint32_t ms);

#endif
