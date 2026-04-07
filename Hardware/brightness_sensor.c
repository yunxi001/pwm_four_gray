#include "brightness_sensor.h"
#include "delay.h"
#include <stdio.h>

volatile uint16_t bright_data[1]; // 光照传感器ADC数据，bright_data[0]对应s1

static void Two_gray_sensor_IO_Init(void)
{
    // 1.开启模拟传感器时钟
    // 2.定义GPIO结构体
    GPIO_InitTypeDef GPIO_InitStruct;
    // 3.结构体参数设置，采用模拟输入模式
    GPIO_InitStruct.GPIO_Pin   = Analog_1_PIN | Analog_2_PIN;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    // 4.初始化GPIO
    GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin   = Analog_5_PIN;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    // 4.初始化GPIO
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}

// Analog_1:PC0 ADC1通道10,Analog_2:PC1 ADC1通道11,Analog_3:PC2 ADC1通道12
// Analog_4：PA0 ADC1通道0,Analog_5：PA4 ADC1通道4，Analog_6:PA6 ADC1通道6
static void Adc_Init(void)
{
    // 初始化模拟引脚
    Two_gray_sensor_IO_Init();
    // 开启ADC1的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    // 开启DMA1的时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_InitTypeDef DMA_InitStruct;
    ADC_InitTypeDef ADC_InitStruct;
    // DMA参数配置
    DMA_InitStruct.DMA_BufferSize         = 2;                                 // 读取2个长度的数据
    DMA_InitStruct.DMA_DIR                = DMA_DIR_PeripheralSRC;             // 方向选择外设到内存
    DMA_InitStruct.DMA_M2M                = DMA_M2M_Disable;                   // 不开启内存到内存
    DMA_InitStruct.DMA_MemoryBaseAddr     = (uint32_t)bright_data;             // 将存储adc数据的数组地址传入
    DMA_InitStruct.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;       // 传输宽度半字（16位）
    DMA_InitStruct.DMA_MemoryInc          = DMA_MemoryInc_Enable;              // 内存地址递增
    DMA_InitStruct.DMA_Mode               = DMA_Mode_Circular;                 // DMA循环模式
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;               // 外设地址选择ADC1的数据寄存器
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;   // 外设数据宽度半字（16位）
    DMA_InitStruct.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;         // 外设地址不递增
    DMA_InitStruct.DMA_Priority           = DMA_Priority_VeryHigh;             // 开启高优先级
    DMA_Init(DMA1_Channel1, &DMA_InitStruct);
    // 使能DMA传输
    DMA_Cmd(DMA1_Channel1, ENABLE);
    // ADC参数配置
    ADC_InitStruct.ADC_Mode               = ADC_Mode_Independent;        // ADC独立模式
    ADC_InitStruct.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;   // ADC软件触发
    ADC_InitStruct.ADC_DataAlign          = ADC_DataAlign_Right;         // 数据右对齐
    ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;                      // ADc连续扫描模式
    ADC_InitStruct.ADC_ScanConvMode       = ENABLE;                      // ADC扫描模式
    ADC_InitStruct.ADC_NbrOfChannel       = 2;                           // ADC通道数

    ADC_Init(ADC1, &ADC_InitStruct);
    // 配置ADC通道的采集
//    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_239Cycles5);
//    ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 2, ADC_SampleTime_239Cycles5);
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1,ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1,ADC_SampleTime_239Cycles5);
		ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 2,ADC_SampleTime_239Cycles5);

//	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 3,ADC_SampleTime_239Cycles5);

    // 使能ADC采集
    ADC_Cmd(ADC1, ENABLE);
    // 使能DMA请求
    ADC_DMACmd(ADC1, ENABLE);
    // ADC校准
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1) == SET);
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1) == SET);
    // 开启软件触发
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

// 模拟传感器初始化
void Two_gray_sensor_Init(void)
{
    Adc_Init();
}

void Two_gray_sensor_Read(uint8_t* data)
{
    float left_norm  = (bright_data[0] - 500) / 1800.0;   // 1800 = 2300 - 500
}

void Two_gray_sensor_Test(void)
{
    printf("Analog_5:%d\r\n", bright_data[0]);
    // 延时处理
    Delay_ms(500);
}
