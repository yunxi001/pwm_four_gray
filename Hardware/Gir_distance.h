#ifndef __GIR_DISTANCE_H
#define __GIR_DISTANCE_H


#include "stm32f10x.h"
#include "Delay.h"
#include "stdint.h"
#include "string.h"
#include <stdio.h>

#define Distance_IO_CLK_ENABLE()	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE)
#define Distance_SCL_PORT    			GPIOA
#define Distance_SCL_PIN     			GPIO_Pin_6
#define Distance_SDA_PORT    			GPIOA
#define Distance_SDA_PIN    			  GPIO_Pin_7
#define GIr_distance_sensor_DEF_I2C_ADDR 0x29

// 寄存器地址定义
#define GIr_distance_sensor_REG_IDENTIFICATION_MODEL_ID       0x00c0
#define GIr_distance_sensor_REG_IDENTIFICATION_REVISION_ID    0x00c2
#define GIr_distance_sensor_REG_PRE_RANGE_CONFIG_VCSEL_PERIOD 0x0050
#define GIr_distance_sensor_REG_FINAL_RANGE_CONFIG_VCSEL_PERIOD 0x0070
#define GIr_distance_sensor_REG_SYSRANGE_START                0x0000
#define GIr_distance_sensor_REG_RESULT_INTERRUPT_STATUS       0x0013
#define GIr_distance_sensor_REG_RESULT_RANGE_STATUS           0x0014
#define GIr_distance_sensor_REG_I2C_SLAVE_DEVICE_ADDRESS      0x008a
#define GIr_distance_sensor_REG_SYSTEM_RANGE_CONFIG           0x0009
#define GIr_distance_sensor_REG_VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV 0x0089
#define GIr_distance_sensor_REG_SYSRANGE_MODE_SINGLESHOT      0x0000
#define GIr_distance_sensor_REG_SYSRANGE_MODE_START_STOP      0x0001
#define GIr_distance_sensor_REG_SYSRANGE_MODE_BACKTOBACK      0x0002
#define GIr_distance_sensor_REG_SYSRANGE_MODE_TIMED           0x0004

// 设备模式定义
#define GIr_distance_sensor_DEVICEMODE_SINGLE_RANGING         0
#define GIr_distance_sensor_DEVICEMODE_CONTINUOUS_RANGING     1
#define GIr_distance_sensor_DEVICEMODE_CONTINUOUS_TIMED_RANGING 3
#define GIr_distance_sensor_DEFAULT_MAX_LOOP                  200

#define ESD_2V8

// 功能状态枚举
typedef enum {
    edisable = 0,
    eENABLE = 1
} eFunctionalState;

// 精度状态枚举
typedef enum {
    eHigh = 0,
    eLow = 1
} ePrecisionState;

// 模式状态枚举
typedef enum {
    eSingle = 0,
    eContinuous = 1
} eModeState;

// 传感器详细数据结构体
typedef struct {
    uint8_t i2cDevAddr;
    uint8_t mode;
    uint8_t precision;
    uint8_t originalData[16];
    uint16_t ambientCount;
    uint16_t signalCount;
    uint16_t distance;
    uint8_t status;
} sGIr_distance_sensor_DetailedData_t;

// 函数声明
void Distance_DelayUs(uint16_t us);      
void Distance_IO_Init(void);             
void Distance_SDA_SetInput(void);         
void Distance_SDA_SetOutput(void);        
void delay_ms(uint32_t ms);               

void Distance_IO_Init(void);
void Distance_SDA_SetInput(void);
void Distance_SDA_SetOutput(void);

// 对外传感器函数
void Gir_distance_sensor_init(void);
void Gir_setMode(ePrecisionState precision);
float getDistance(void);

#endif

