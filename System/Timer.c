#include "stm32f10x.h"                  // Device header

/**
  * 函    数：定时中断初始化 - 用于高频传感器检测
  * 参    数：无
  * 返 回 值：无
  */
void Timer_Init(void)
{
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);			//开启TIM2的时钟
	
	/*配置时钟源*/
	TIM_InternalClockConfig(TIM2);		//选择TIM2为内部时钟，若不调用此函数，TIM默认也为内部时钟
	
	/*时基单元初始化*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				//定义结构体变量
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//计数器模式，选择向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;				//计数周期，5ms中断 (72MHz/72/360 = 200Hz = 5ms)
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;				//预分频器，即PSC的值
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;			//重复计数器，高级定时器才会用到
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);				//将结构体变量交给TIM_TimeBaseInit，配置TIM2的时基单元	
	
	/*中断输出配置*/
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);						//清除定时器更新标志位
																//TIM_TimeBaseInit函数末尾，手动产生了更新事件
																//若不清除此标志位，则开启中断后，会立刻进入一次中断
																//如果不介意此问题，则不清除此标志位也可
	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);					//开启TIM2的更新中断
	
	/*NVIC中断分组*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);				//配置NVIC为分组2
																//即抢占优先级范围：0~3，响应优先级范围：0~3
																//此分组配置在整个工程中仅需调用一次
																//若有多个中断，可以把此代码放在main函数内，while循环之前
																//若调用多次配置分组的代码，则后执行的配置会覆盖先执行的配置
	
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;						//定义结构体变量
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;				//选择配置NVIC的TIM2线
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//指定NVIC线路使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//指定NVIC线路的抢占优先级为1（高优先级）
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//指定NVIC线路的响应优先级为0
	NVIC_Init(&NVIC_InitStructure);								//将结构体变量交给NVIC_Init，配置NVIC外设
	
	/*TIM使能*/
	TIM_Cmd(TIM2, ENABLE);			//使能TIM2，定时器开始运行
}
// 转弯检测相关变量
uint8_t gray_scan = 0;
uint8_t g_turn_request = 0;  // 转弯请求标志
uint8_t g_turn_type = 0;     // 转弯类型：0=无，1=左转，2=右转，3=十字路口
extern uint8_t g_turning_in_progress;

uint32_t my_aaa = 0;
// 定时器中断函数 - 1ms高频传感器检测
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		if(g_turning_in_progress)
		{
		my_aaa ++;
		}
		if(my_aaa >= 10000)
		{
				g_turn_request = 0;
		g_turning_in_progress = 0;
			my_aaa = 0;
		}


		gray_scan = 1;
		
		// 在中断中进行快速转弯检测
		extern uint16_t Analog_data[2];
		extern uint8_t g_turning_in_progress;
		
		// 如果正在转弯，跳过检测
		if(!g_turning_in_progress) {
			static int both_confirm_count = 0;
			const int CONFIRM_THRESHOLD = 3;  // 2次确认（2*5ms=10ms）
			
			uint16_t left_threshold = (3815 + 841) / 2;   // ≈ 2328
			uint16_t right_threshold = (3884 + 1254) / 2; // ≈ 2569
			
			uint16_t left = Analog_data[0];
			uint16_t right = Analog_data[1];
			
			// 检测当前状态
			uint8_t left_on_line = (left < left_threshold) ? 1 : 0;
			uint8_t right_on_line = (right < right_threshold) ? 1 : 0;
			
			// 十字路口检测（两个传感器都在黑线上）
			if(left_on_line && right_on_line) {
				both_confirm_count++;
				if(both_confirm_count >= CONFIRM_THRESHOLD) {
					both_confirm_count = 0;  // 重置计数
					g_turn_request = 1;      // 设置转弯请求
					g_turning_in_progress = 1;
					g_turn_type = 3;         // 十字路口右转
				}
			} else {
				both_confirm_count = 0;  // 重置计数
			}
		}
		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
