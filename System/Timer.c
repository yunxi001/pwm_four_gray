#include "Delay.h"

/**
 * 函    数：定时中断初始化 - 用于高频传感器检测
 * 参    数：无
 * 返 回 值：无
 */
void Timer_Init(void)
{
    /*开启时钟*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);   // 开启TIM2的时钟

    /*配置时钟源*/
    TIM_InternalClockConfig(TIM2);   // 选择TIM2为内部时钟，若不调用此函数，TIM默认也为内部时钟

    /*时基单元初始化*/
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;                      // 定义结构体变量
    TIM_TimeBaseInitStructure.TIM_ClockDivision     = TIM_CKD_DIV1;         // 时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
    TIM_TimeBaseInitStructure.TIM_CounterMode       = TIM_CounterMode_Up;   // 计数器模式，选择向上计数
    TIM_TimeBaseInitStructure.TIM_Period            = 1000 - 1;             // 计数周期，5ms中断 (72MHz/72/360 = 200Hz = 5ms)
    TIM_TimeBaseInitStructure.TIM_Prescaler         = 72 - 1;               // 预分频器，即PSC的值
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;                    // 重复计数器，高级定时器才会用到
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);                     // 将结构体变量交给TIM_TimeBaseInit，配置TIM2的时基单元

    /*中断输出配置*/
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);   // 清除定时器更新标志位
                                            // TIM_TimeBaseInit函数末尾，手动产生了更新事件
                                            // 若不清除此标志位，则开启中断后，会立刻进入一次中断
                                            // 如果不介意此问题，则不清除此标志位也可

    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);   // 开启TIM2的更新中断

    /*NVIC中断分组*/
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);   // 配置NVIC为分组2
                                                      // 即抢占优先级范围：0~3，响应优先级范围：0~3
                                                      // 此分组配置在整个工程中仅需调用一次
                                                      // 若有多个中断，可以把此代码放在main函数内，while循环之前
                                                      // 若调用多次配置分组的代码，则后执行的配置会覆盖先执行的配置

    /*NVIC配置*/
    NVIC_InitTypeDef NVIC_InitStructure;                                // 定义结构体变量
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM2_IRQn;   // 选择配置NVIC的TIM2线
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;      // 指定NVIC线路使能
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;           // 指定NVIC线路的抢占优先级为1（高优先级）
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;           // 指定NVIC线路的响应优先级为0
    NVIC_Init(&NVIC_InitStructure);                                     // 将结构体变量交给NVIC_Init，配置NVIC外设

    /*TIM使能*/
    TIM_Cmd(TIM2, ENABLE);   // 使能TIM2，定时器开始运行
}
//传感器读取
volatile uint8_t sensor_read           = 0; 
volatile uint16_t sensor_read_count           = 0;
// 转弯检测相关变量
volatile uint8_t g_turn_type           = 0;   // 转弯类型：0=无，1=左转，2=右转，3=十字路口
volatile uint8_t g_turning_in_progress = 0;   // 正在转弯中
#define CONFIRM_THRESHOLD 15                   // 确认
//volatile uint8_t first_g_turning_in_progress           = 1;   
// 定时器中断函数 - 1ms高频传感器检测
void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
		sensor_read_count ++;
		if(sensor_read_count >= 1000)
		{
			sensor_read_count = 0;
			sensor_read = 1;
		}
        // 在中断中进行快速转弯检测
        extern uint16_t Analog_data[3];

        // 如果正在转弯，跳过检测
        if (!g_turning_in_progress)
        {
            static int both_confirm_count = 0;   // 双传感器同时检测到黑线的确认计数
            static int left_only_count = 0;     // 仅左传感器检测到黑线的计数
            static int right_only_count = 0;    // 仅右传感器检测到黑线的计数

//			uint16_t left_threshold  = (3862 + 1380) / 2 -100;    
//			uint16_t right_threshold = (3863 + 1030) / 2 -100;   
			uint16_t left_threshold  = (4095 + 1100) / 2 -100;    
			uint16_t right_threshold = (4095 + 1100) / 2 -100;  
            uint16_t left  = Analog_data[1];   // 左传感器
            uint16_t right = Analog_data[0];   // 右传感器

            // 检测当前状态
            uint8_t left_on_line  = (left < left_threshold) ? 1 : 0;
            uint8_t right_on_line = (right < right_threshold) ? 1 : 0;

            // 十字路口检测（两个传感器都在黑线上）
            if (left_on_line && right_on_line)
            {
                both_confirm_count++;
                left_only_count = 0;   // 重置其他计数
                right_only_count = 0;
                
                if (both_confirm_count >= CONFIRM_THRESHOLD)
                {
//					if(first_g_turning_in_progress)
//					{
//						first_g_turning_in_progress = 0;
//						both_confirm_count    = 0;   // 重置计数
//					}
//					else
//					{
						both_confirm_count    = 0;   // 重置计数
						g_turning_in_progress = 1;
						g_turn_type           = 3;   // 十字路口
//					}

                }
            }
//            // 左转检测（仅右传感器在黑线上，左传感器离开）
//            else if (!left_on_line && right_on_line)
//            {
//                right_only_count++;
//                both_confirm_count = 0;   // 重置其他计数
//                left_only_count = 0;
//                
//                if (right_only_count >= CONFIRM_THRESHOLD)
//                {
//                    right_only_count      = 0;   // 重置计数
//                    g_turning_in_progress = 1;
//                    g_turn_type           = 1;   // 左转
//                }
//            }
//            // 右转检测（仅左传感器在黑线上，右传感器离开）
//            else if (left_on_line && !right_on_line)
//            {
//                left_only_count++;
//                both_confirm_count = 0;   // 重置其他计数
//                right_only_count = 0;
//                
//                if (left_only_count >= CONFIRM_THRESHOLD)
//                {
//                    left_only_count       = 0;   // 重置计数
//                    g_turning_in_progress = 1;
//                    g_turn_type           = 2;   // 右转
//                }
//            }
            else
            {
                // 都不在线或正常巡线状态，重置所有计数
                both_confirm_count = 0;
                left_only_count = 0;
                right_only_count = 0;
            }
        }
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
