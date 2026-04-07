/**
 * \class   GLightSensor
 * \brief   GMega光线传感器模块驱动类
 * @file    GLightSensor.h
 * @author  GSTEM
 * @version V1.0.0
 * @date    2017/3/31
 * @brief   光线传感器驱动程序的头文件
 *
 * \par 功能描述
 * 该类实现了对GMega光线传感器模块的驱动，包括引脚配置、光线值读取、LED灯控制等功能
 *
 * \par 成员函数列表:
 *
 *    1. void    GLightSensor::setpin(uint8_t ledPin, uint8_t sensorPin) - 设置LED引脚和传感器引脚
 *    2. int16_t GLightSensor::read() - 读取光线传感器的模拟值
 *    3. void    GLightSensor::lightOn() - 打开传感器上的LED灯
 *    4. void    GLightSensor::lightOff() - 关闭传感器上的LED灯
 *
 * \par 版本历史:
 * <pre>
 * `<开发者>`         `<时间>`        `<版本号>`        `<描述>`
 * Geek Yao         2017/03/31     1.0.0              初始版本开发
 * </pre>
 *
 * @example GLightSensorTest.ino \n - 基础功能测试示例
 * @example GLightSensorTestSetpin.ino \n - 引脚设置测试示例
 * @example GLightSensorTestWithLEDon.ino - LED灯控制测试示例
 */

/* 私有定义 -------------------------------------------------------------------*/
#include "GLightSensor.h"

///* 公共函数 -------------------------------------------------------------------*/

///**
// * 重载构造函数：将光线传感器映射到指定的Arduino端口
// * \param[in]
// *   port - RJ25端口号，范围从PORT_1到M2
// */
//GLightSensor::GLightSensor(uint8_t sensorPin) 
//{
//    if(sensorPin ==1 )
//    {
//      _sensorPin = A0;
//      pinMode(_sensorPin, INPUT);    
//    }
//    else if(sensorPin == 2)
//    {
//      _sensorPin = A1;
//      pinMode(_sensorPin, INPUT);    
//    } 
//    else if(sensorPin == 3)
//    {
//      _sensorPin = A2;
//      pinMode(_sensorPin, INPUT);    
//    } 
//    else if(sensorPin == 4)
//    {
//      _sensorPin = A3;
//      pinMode(_sensorPin, INPUT);    
//    } 
//    else if(sensorPin == 5)
//    {
//      _sensorPin = A4;
//      pinMode(_sensorPin, INPUT);    
//    }
//    else if(sensorPin == 6)
//    {
//      _sensorPin = A5;
//      pinMode(_sensorPin, INPUT);    
//    } 
//    else if(sensorPin == 7)
//    {
//      _sensorPin = A6;
//      pinMode(_sensorPin, INPUT);    
//    }
//}

///**
// * 重载构造函数：自定义映射光线传感器和LED的Arduino端口
// * \param[in]
// *   ledPin - 连接LED的Arduino引脚
// * \param[in]
// *   sensorPin - 连接传感器数据读取的Arduino引脚
// */
//GLightSensor::GLightSensor(uint8_t ledPin, uint8_t sensorPin)
//{
//  _ledPin = ledPin;
//  _sensorPin = sensorPin;

//  pinMode(_ledPin, OUTPUT);   // 设置LED引脚为输出模式
//  pinMode(_sensorPin, INPUT); // 设置传感器引脚为输入模式

//  digitalWrite(_ledPin,LOW);  // 初始化LED为关闭状态
//}

///**
// * \par 函数名称
// *    setpin
// * \par 函数描述
// *    设置传感器引脚  \n
// * \param[in]
// *    sensorPin - 传感器连接的开发板引脚编号
// * \par 输出参数
// *    无
// * \par 返回值
// *    无
// */
//void GLightSensor::setpin(uint8_t sensorPin)
//{
//  _sensorPin = sensorPin;
//  pinMode(_sensorPin, INPUT); // 设置传感器引脚为输入模式
//}

///**
// * \par 函数名称
// *    setpin
// * \par 函数描述
// *    设置LED引脚和传感器引脚  \n
// * \param[in]
// *    ledPin - LED灯连接的开发板引脚编号
// * \param[in]
// *    sensorPin - 传感器连接的开发板引脚编号
// * \par 输出参数
// *    无
// * \par 返回值
// *    无
// * \par 其他说明
// *    设置全局变量_KeyPin和s2（注：原注释存在笔误，实际设置的是_ledPin和_sensorPin）
// */
//void GLightSensor::setpin(uint8_t ledPin, uint8_t sensorPin)
//{
//  _ledPin = ledPin;
//  _sensorPin = sensorPin;
//  pinMode(_ledPin, OUTPUT);   // 设置LED引脚为输出模式
//  pinMode(_sensorPin, INPUT); // 设置传感器引脚为输入模式
//}

///**
// * \par 函数名称
// *    read
// * \par 函数描述
// *    读取光线传感器的模拟值
// * \param[in]
// *    无
// * \par 输出参数
// *    无
// * \par 返回值
// *    analogRead(_sensorPin) - 当前光线传感器输出的模拟数值（0-1023）
// * \par 其他说明
// *    该值越小表示光线越强，值越大表示光线越弱
// */
//int16_t GLightSensor::read(void)
//{
//	return(analogRead(_sensorPin));
//}

///**
// * \par 函数名称
// *    lightOn
// * \par 函数描述
// *    打开光线传感器上的LED灯
// * \param[in]
// *    无
// * \par 输出参数
// *    无
// * \par 返回值
// *    无
// * \par 其他说明
// *    该LED灯为低电平触发点亮
// */
//void GLightSensor::lightOn(void)
//{
//  digitalWrite(_ledPin,LOW);
//}

///**
// * \par 函数名称
// *    lightOff
// * \par 函数描述
// *    关闭光线传感器上的LED灯
// * \param[in]
// *    无
// * \par 输出参数
// *    无
// * \par 返回值
// *    无
// * \par 其他说明
// *    该LED灯为高电平触发熄灭
// */
//void GLightSensor::lightOff(void)
//{
//  digitalWrite(_ledPin,HIGH);
//}