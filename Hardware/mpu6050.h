#ifndef MPU6050_H
#define MPU6050_H
#include "stm32f10x.h"
#include <stdint.h>
#include <stdbool.h>

#define MPU6050_IO_CLK_ENABLE()	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE)
#define MPU6050_SCL_PORT    			GPIOC
#define MPU6050_SCL_PIN     			GPIO_Pin_2
#define MPU6050_SDA_PORT    			GPIOC
#define MPU6050_SDA_PIN    			  GPIO_Pin_3

#define MPU6050_ADDRESS  0xD0
#define	MPU6050_SMPLRT_DIV		0x19
#define	MPU6050_CONFIG			0x1A
#define	MPU6050_GYRO_CONFIG		0x1B
#define	MPU6050_ACCEL_CONFIG	0x1C

#define	MPU6050_ACCEL_XOUT_H	0x3B
#define	MPU6050_ACCEL_XOUT_L	0x3C
#define	MPU6050_ACCEL_YOUT_H	0x3D
#define	MPU6050_ACCEL_YOUT_L	0x3E
#define	MPU6050_ACCEL_ZOUT_H	0x3F
#define	MPU6050_ACCEL_ZOUT_L	0x40
#define	MPU6050_TEMP_OUT_H		0x41
#define	MPU6050_TEMP_OUT_L		0x42
#define	MPU6050_GYRO_XOUT_H		0x43
#define	MPU6050_GYRO_XOUT_L		0x44
#define	MPU6050_GYRO_YOUT_H		0x45
#define	MPU6050_GYRO_YOUT_L		0x46
#define	MPU6050_GYRO_ZOUT_H		0x47
#define	MPU6050_GYRO_ZOUT_L		0x48

#define	MPU6050_PWR_MGMT_1		0x6B  //电源管理
#define	MPU6050_PWR_MGMT_2		0x6C
#define	MPU6050_WHO_AM_I		0x75

// 引脚电平操作宏（基于标准库兼容的寄存器操作，简化时序代码）
#define MPU6050_SCL_HIGH()  GPIO_SetBits(MPU6050_SCL_PORT, MPU6050_SCL_PIN)
#define MPU6050_SCL_LOW()   GPIO_ResetBits(MPU6050_SCL_PORT, MPU6050_SCL_PIN)
#define MPU6050_SDA_HIGH()  GPIO_SetBits(MPU6050_SDA_PORT, MPU6050_SDA_PIN)
#define MPU6050_SDA_LOW()   GPIO_ResetBits(MPU6050_SDA_PORT, MPU6050_SDA_PIN)
#define MPU6050_SDA_READ()  GPIO_ReadInputDataBit(MPU6050_SDA_PORT, MPU6050_SDA_PIN)


void MPU6050_IO_Init(void);
void MPU6050_SDA_SetInput(void);
void MPU6050_SDA_SetOutput(void);
void MPU6050_DelayUs(uint16_t us);
void MPU6050_Start(void);
void MPU6050_Stop(void);
uint8_t WriteByte(uint8_t byte);
uint8_t ReadByte(uint8_t ack);
void MPU6050_WriteReg(uint8_t RegAddress,uint8_t Data);
uint8_t MPU6050_ReadReg(uint8_t RegAddress);
uint8_t MPU6050_GetID(void);
void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ,int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ);
void MPU6050_Init(void);


#endif
