#include "GTouch_sensor.h"

///**
// * @brief 初始化ADC外设（通用初始化，仅初始化一次）
// */
//static void ADC_Config(void)
//{
//    static uint8_t adc_init_flag = 0;
//    if (adc_init_flag) return;  // 避免重复初始化

//    ADC_InitTypeDef ADC_InitStructure;
//    GPIO_InitTypeDef GPIO_InitStructure;
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);  // 使能ADC1和GPIOA时钟

//    // 配置PA0-PA6为模拟输入模式
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | 
//                                  GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;  // 模拟输入模式
//    GPIO_Init(GPIOA, &GPIO_InitStructure);

//    // ADC配置：独立模式、单次转换、软件触发
//    RCC_ADCCLKConfig(RCC_PCLK2_Div6);  // ADC时钟分频（72M/6=12M，符合ADC时钟要求）
//    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
//    ADC_InitStructure.ADC_ScanConvMode = DISABLE;  // 单通道模式
//    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;  // 单次转换模式
//    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  // 软件触发
//    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  // 数据右对齐
//    ADC_InitStructure.ADC_NbrOfChannel = 1;  // 转换通道数：1
//    ADC_Init(ADC1, &ADC_InitStructure);

//    ADC_Cmd(ADC1, ENABLE);  // 使能ADC1

//    // ADC校准
//    ADC_ResetCalibration(ADC1);
//    while (ADC_GetResetCalibrationStatus(ADC1));
//    ADC_StartCalibration(ADC1);
//    while (ADC_GetCalibrationStatus(ADC1));

//    adc_init_flag = 1;
//}

///**
// * @brief 读取指定ADC通道的原始值
// * @param ch ADC通道号（0-6，对应PA0-PA6）
// * @retval uint16_t ADC转换结果（0-4095，12位精度）
// */
//static uint16_t ADC_Read(uint8_t ch)
//{
//    // 设置要转换的ADC通道和采样时间
//    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_55Cycles5);
//    
//    ADC_SoftwareStartConvCmd(ADC1, ENABLE);  // 软件触发转换
//    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));  // 等待转换完成
//    
//    return ADC_GetConversionValue(ADC1);  // 返回转换结果
//}

///**
// * @brief 初始化触摸传感器对应的ADC通道
// * @param sensor_io 传感器IO编号（1-7，对应TOUCH_SENSOR_IO1~IO7）
// */
//void GTouch_sensor_init(uint8_t sensor_io)
//{
//    // 初始化ADC外设（内部会保证只初始化一次）
//    ADC_Config();
//    
//    // 原Arduino的pinMode逻辑在STM32中已通过ADC_Config完成GPIO配置
//    // 此处保留参数兼容，无额外操作
//    (void)sensor_io;  // 消除未使用参数警告
//}

///**
// * @brief 读取触摸传感器的电平状态
// * @param sensor_io 传感器IO编号（1-7，对应TOUCH_SENSOR_IO1~IO7）
// * @retval bool 触摸状态：true(触摸检测到) / false(无触摸)
// */
//bool GTouch_sensor_getlevel(uint8_t sensor_io)
//{
//    uint8_t adc_ch = 0;
//    uint16_t adc_value = 0;

//    // 映射sensor_io到对应的ADC通道（1->CH0, 2->CH1...7->CH6）
//    switch (sensor_io)
//    {
//        case 1: adc_ch = ADC_Channel_0; break;
//        case 2: adc_ch = ADC_Channel_1; break;
//        case 3: adc_ch = ADC_Channel_2; break;
//        case 4: adc_ch = ADC_Channel_3; break;
//        case 5: adc_ch = ADC_Channel_4; break;
//        case 6: adc_ch = ADC_Channel_5; break;
//        case 7: adc_ch = ADC_Channel_6; break;
//        default: return false;  // 无效IO编号，返回默认值
//    }

//    adc_value = ADC_Read(adc_ch);  // 读取ADC原始值
//    return (adc_value > 10);  // 与原逻辑一致：值大于10判定为有触摸
//}