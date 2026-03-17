#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"

void Timer_Init(void);

// 转弯检测相关变量声明
extern uint8_t gray_scan;
extern uint8_t g_turn_request;  // 转弯请求标志
extern uint8_t g_turn_type;     // 转弯类型

#endif
