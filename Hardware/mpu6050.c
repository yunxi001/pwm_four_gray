#include "mpu6050.h"
#include "delay.h"
#include <stdio.h>

void MPU6050_IO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 1. 开启GPIOC时钟
    MPU6050_IO_CLK_ENABLE();
    
    // 2. 配置CLK和DIO引脚为开漏输出
    GPIO_InitStructure.GPIO_Pin = MPU6050_SCL_PIN | MPU6050_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;    // 开漏输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   // 50MHz输出速率
    GPIO_Init(GPIOC, &GPIO_InitStructure);   // 初始化引脚
}
void MPU6050_SDA_SetInput(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = MPU6050_SDA_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
    GPIO_Init(MPU6050_SDA_PORT, &GPIO_InitStruct);
}

void MPU6050_SDA_SetOutput(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = MPU6050_SDA_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD; // 开漏输出
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MPU6050_SDA_PORT, &GPIO_InitStruct);
}


// 延时约1us（8MHz时钟精准适配）
void MPU6050_DelayUs(uint16_t us)
{
    uint16_t i, j;
    for(i = 0; i < us; i++)
    {
        for(j = 0; j < 8; j++) // 8MHz下8个NOP≈1us
        {
            __NOP();
        }
    }
}

// 起始信号：CLK高电平时，SDA从高拉低
void MPU6050_Start(void)
{
    MPU6050_SDA_SetOutput(); // SDA设为输出
    MPU6050_SCL_HIGH();
    MPU6050_SDA_HIGH();
    MPU6050_DelayUs(10); // 延长延时，提升稳定性
    MPU6050_SDA_LOW();
    MPU6050_DelayUs(10);
    MPU6050_SCL_LOW();
    MPU6050_DelayUs(10);
}

// 停止信号：CLK高电平时，SDA从低拉高
void MPU6050_Stop(void)
{
    MPU6050_SDA_SetOutput(); // SDA设为输出
    MPU6050_SCL_LOW();
    MPU6050_SDA_LOW();
    MPU6050_DelayUs(10);
    MPU6050_SCL_HIGH();
    MPU6050_DelayUs(10);
    MPU6050_SDA_HIGH();
    MPU6050_DelayUs(10);
}

// 写1个字节（高位在前），返回应答状态（1=ACK，0=NACK）
uint8_t WriteByte(uint8_t byte)
{
    uint8_t i, ack = 0;
    
    MPU6050_SDA_SetOutput(); // SDA设为输出
    for(i = 0; i < 8; i++)
    {
        MPU6050_SCL_LOW();
        MPU6050_DelayUs(10);
        
        // 发送当前位（高位在前）
        if((byte & 0x80) != 0)
            MPU6050_SDA_HIGH();
        else
            MPU6050_SDA_LOW();
        
        byte <<= 1; // 左移，准备下一位
        MPU6050_DelayUs(10);
        
        MPU6050_SCL_HIGH(); // CLK上升沿锁存数据
        MPU6050_DelayUs(10);
    }
    
    // 读取应答信号
    MPU6050_SCL_LOW();
    MPU6050_SDA_SetInput(); // SDA切换为输入，读取ACK
    MPU6050_DelayUs(10);
    MPU6050_SCL_HIGH();
    MPU6050_DelayUs(10);
    
    if(MPU6050_SDA_READ() == 0) // SDA拉低=ACK
        ack = 1;
    else
        ack = 0;
    
    MPU6050_SCL_LOW();
    MPU6050_DelayUs(10);
    MPU6050_SDA_SetOutput(); // 恢复SDA为输出
    
    return ack;
}

// 读1个字节（高位在前），ack=1发送ACK，ack=0发送NACK
uint8_t ReadByte(uint8_t ack)
{
    uint8_t byte = 0;
    MPU6050_SDA_HIGH();
    MPU6050_SDA_SetInput(); // SDA切换为输入
		Delay_ms(100);
    for(uint8_t i = 0; i < 8; i++)
    {
        MPU6050_SCL_LOW(); // 先拉低SCL，准备读取
        MPU6050_DelayUs(10);
        MPU6050_SCL_HIGH(); // 拉高SCL，采样数据
        MPU6050_DelayUs(10);
        
        // 正确移位：高位在前，左移拼接
        byte = (byte << 1) | MPU6050_SDA_READ();
        
        MPU6050_SCL_LOW();
        MPU6050_DelayUs(10);
    }
    
    // 发送应答/非应答
    MPU6050_SDA_SetOutput(); // SDA切换为输出
    if(ack == 1)
        MPU6050_SDA_LOW(); // 发送ACK
    else
        MPU6050_SDA_HIGH(); // 发送NACK
    
    MPU6050_DelayUs(10);
    MPU6050_SCL_HIGH();
    MPU6050_DelayUs(10);
    MPU6050_SCL_LOW();
    MPU6050_DelayUs(10);
    MPU6050_SDA_HIGH(); // 释放SDA
    
    return byte;
}

void MPU6050_WriteReg(uint8_t RegAddress,uint8_t Data)
{
    MPU6050_Start();
    if(WriteByte(MPU6050_ADDRESS) == 0) // 发送从机地址（写），检查ACK
    {
        MPU6050_Stop();
        return; // ACK失败，直接退出
    }
    if(WriteByte(RegAddress) == 0) // 发送寄存器地址，检查ACK
    {
        MPU6050_Stop();
        return;
    }
    if(WriteByte(Data) == 0) // 发送数据，检查ACK
    {
        MPU6050_Stop();
        return;
    }
    MPU6050_Stop();
}

uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
    uint8_t Byte = 0xFF; // 默认无效值
    
    MPU6050_Start();
    if(WriteByte(MPU6050_ADDRESS) == 0) // 发送写地址，检查ACK
    {
        MPU6050_Stop();
        return 0xFF;
    }
    if(WriteByte(RegAddress) == 0) // 发送寄存器地址，检查ACK
    {
        MPU6050_Stop();
        return 0xFF;
    }
    
    MPU6050_Start(); // 重复起始信号
    if(WriteByte(MPU6050_ADDRESS | 0x01) == 0) // 发送读地址，检查ACK
    {
        MPU6050_Stop();
        return 0xFF;
    }
    
    Byte = ReadByte(0); // 读取1个字节，发送NACK（单字节读取）
    MPU6050_Stop();
    
    return Byte;
}

uint8_t MPU6050_GetID(void)
{
    return MPU6050_ReadReg(MPU6050_WHO_AM_I);
}

void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
						int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
{                                 //x,y,z加速度值和陀螺仪值
	uint8_t DataH, DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);//读取数据
	*AccX = (DataH << 8) | DataL;  //加速度x轴，16位数据，用指针返回数据
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
	*AccY = (DataH << 8) | DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
	*AccZ = (DataH << 8) | DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
	*GyroX = (DataH << 8) | DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
	*GyroY = (DataH << 8) | DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
	*GyroZ = (DataH << 8) | DataL;
}

void MPU6050_Init(void)
{
    MPU6050_IO_Init();
    
    MPU6050_SCL_HIGH();
    MPU6050_SDA_HIGH();
    MPU6050_DelayUs(1000); // 上电延时，确保芯片稳定
    
    // 唤醒MPU6050（关键：先写0x00关闭睡眠，再配置其他寄存器）
    MPU6050_WriteReg(MPU6050_PWR_MGMT_1,0x00); // 先唤醒（0x00=使用内部8MHz时钟）
    MPU6050_DelayUs(100);
    
    MPU6050_WriteReg(MPU6050_PWR_MGMT_1,0x01); // 选择X轴陀螺仪时钟
    MPU6050_WriteReg(MPU6050_PWR_MGMT_2,0x00); // 无分频，所有轴使能
    MPU6050_WriteReg(MPU6050_SMPLRT_DIV,0x09); // 采样率=1000/(1+9)=100Hz
    MPU6050_WriteReg(MPU6050_CONFIG,0x06);     // 低通滤波器截止频率5Hz
    MPU6050_WriteReg(MPU6050_GYRO_CONFIG,0x18); // 陀螺仪量程±2000°/s
    MPU6050_WriteReg(MPU6050_ACCEL_CONFIG,0x18);// 加速度量程±16g
}
