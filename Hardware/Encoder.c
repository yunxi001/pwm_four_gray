
#include "stm32f10x.h"
//#include "Encoder.h"




volatile int encoder[4] = {0};

void SystemClock_Config(void) {
    RCC_DeInit();
    
    RCC_HSEConfig(RCC_HSE_ON);
    while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);
    
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    RCC_PLLCmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
    
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while(RCC_GetSYSCLKSource() != 0x08);
    
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    RCC_PCLK1Config(RCC_HCLK_Div2);
    RCC_PCLK2Config(RCC_HCLK_Div1);
    
    // ???????????
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | 
                          RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
                          RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1 |
                          RCC_APB2Periph_TIM1 | RCC_APB2Periph_TIM8, ENABLE);
}

// ???????????? - ??????
void Encoder_EXTI_Init(void) {
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // ??????????????
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    // ???1: PC5(A?), PB0(B?)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // ???2: PB1(A?), PB12(B?)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_12;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // ???3: PC11(A?), PC12(B?)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    // ???4: PD2(A?), PB6(B?)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // ???1:PC5 (A?) ????,?????
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource5);
    EXTI_InitStructure.EXTI_Line = EXTI_Line5;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    //EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  //ÉÏÉýÑØ´¥·¢
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;  //ÉÏÉýÑØºÍÏÂ½µÑØÍ¬Ê±´¥·¢
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    // ???2:PB1 (A?) ????,?????
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);
    EXTI_InitStructure.EXTI_Line = EXTI_Line1;
    EXTI_Init(&EXTI_InitStructure);
    
    // ???3:PC11 (A?) ????,?????
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource11);
    EXTI_InitStructure.EXTI_Line = EXTI_Line11;
    EXTI_Init(&EXTI_InitStructure);
    
    // ???4:PD2 (A?) ????,?????
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource2);
    EXTI_InitStructure.EXTI_Line = EXTI_Line2;
    EXTI_Init(&EXTI_InitStructure);
    
    // ??NVIC?????
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    
    // EXTI0 (PB0 - ???1 B?) - ????,???B???
    // NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    // NVIC_Init(&NVIC_InitStructure);
    
    // EXTI1 (PB1 - ???2 A?)
    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_Init(&NVIC_InitStructure);
    
    // EXTI2 (PD2 - ???4 A?)
    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
    NVIC_Init(&NVIC_InitStructure);
    
    // EXTI9_5 (??PC5 - ???1 A?)
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_Init(&NVIC_InitStructure);
    
    // EXTI15_10 (??PC11 - ???3 A?)
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_Init(&NVIC_InitStructure);
}

// ?? EXTI0_IRQHandler - B??????
/*
void EXTI0_IRQHandler(void) {
    if(EXTI_GetITStatus(EXTI_Line0) != RESET) {
        if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5) == Bit_SET) {
            encoder[0]--;
        } else {
            encoder[0]++;
        }			  
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}
*/


void EXTI1_IRQHandler(void) {
    if(EXTI_GetITStatus(EXTI_Line1) != RESET) 
		{
			  int a = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);
			  int b = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1);
				if(a==1)
        {
					if(b==1)
					{
						encoder[1]++; 
					}
					else
					{
						encoder[1]--;
					}
				}
				else
				{
					if(b==1)
					{
						encoder[1]--; 
					}
					else
					{
						encoder[1]++;
					}
				}
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}


void EXTI2_IRQHandler(void) {
    if(EXTI_GetITStatus(EXTI_Line2) != RESET)
		{
			  int a = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2);
			  int b = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6);
				if(a==1)
        {
					if(b==1)
					{
						encoder[3]++; 
					}
					else
					{
						encoder[3]--;
					}
				}
				else
				{
					if(b==1)
					{
						encoder[3]--; 
					}
					else
					{
						encoder[3]++;
					}
				}				
        EXTI_ClearITPendingBit(EXTI_Line2);
    }
}


void EXTI9_5_IRQHandler(void)
{
	
    if(EXTI_GetITStatus(EXTI_Line5) != RESET)
		{
			  int a = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0);
			  int b = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5);
				if(a==1)
        {
					if(b==1)
					{
						encoder[0]++; 
					}
					else
					{
						encoder[0]--;
					}
				}
				else
				{
					if(b==1)
					{
						encoder[0]--; 
					}
					else
					{
						encoder[0]++;
					}
				}							
        EXTI_ClearITPendingBit(EXTI_Line5);
    }
    
    // ??PB6?????
    /*
    if(EXTI_GetITStatus(EXTI_Line6) != RESET) {
        // ???4 B?,??A?????
        if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2) == Bit_SET) {
            encoder[3]--;
        } else {
            encoder[3]++;
        }
        EXTI_ClearITPendingBit(EXTI_Line6);
    }
    */
}


void EXTI15_10_IRQHandler(void) {
    if(EXTI_GetITStatus(EXTI_Line11) != RESET) 
		{
			  int a = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11);
			  int b = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_12);
				if(a==1)
        {
					if(b==1)
					{
						encoder[2]++; 
					}
					else
					{
						encoder[2]--;
					}
				}
				else
				{
					if(b==1)
					{
						encoder[2]--; 
					}
					else
					{
						encoder[2]++;
					}
				}					
        EXTI_ClearITPendingBit(EXTI_Line11);
    }
    
    // ??PB12?????
    /*
    if(EXTI_GetITStatus(EXTI_Line12) != RESET) {
        // PB12 - ???2 B?
        // ??:PB12?PC12??Line12?,????
        if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == Bit_SET) {
            encoder[1]--;
        } else {
            encoder[1]++;
        }
        EXTI_ClearITPendingBit(EXTI_Line12);
    }
    */
}

void GET_Encoder(int *recv_buf) 
{
    
    __disable_irq();
    recv_buf[0] = -encoder[0];
    recv_buf[1] = -encoder[1];
    recv_buf[2] = encoder[3];  
    recv_buf[3] = encoder[2];  
    __enable_irq();
}

void RESET_Encoder(void) 
{
    __disable_irq();
    encoder[0] = 0;
    encoder[1] = 0;
    encoder[2] = 0;
    encoder[3] = 0;
    __enable_irq();
}


float App_Encoder_Pos_1(void)   //»ñÈ¡×óÇ°µç»úµ±Ç°½Ç¶È
{
	return -encoder[0] / 26.0f/90.0f*60.0f;
}

float App_Encoder_Pos_2(void)   //»ñÈ¡Òµç»úµ±Ç°½Ç¶È
{
	return -encoder[1] / 26.0f/90.0f*60.0f;
}

float App_Encoder_Pos_3(void)   //»ñÈ¡ÓÒºó µç»úµ±Ç°½Ç¶È
{
	return encoder[3] / 26.0f/90.0f*60.0f;
}

float App_Encoder_Pos_4(void)   //»ñÈ¡ÓÓÒµç»úµ±Ç°½Ç¶È
{
	return encoder[2] / 26.0f/90.0f*60.0f;
}
