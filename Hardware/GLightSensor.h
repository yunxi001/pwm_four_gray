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
 * 该类实现了对GMega光线传感器模块的驱动，包括引脚配置、光线值读取、LED灯控制等核心功能
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
 * Geek Yao         2017/03/31     1.0.0              完成初始版本开发
 * </pre>
 *
 * @example GLightSensorTest.ino \n - 基础功能测试示例
 * @example GLightSensorTestSetpin.ino \n - 引脚设置测试示例
 * @example GLightSensorTestWithLEDon.ino - LED灯控制测试示例
 */


/* 定义防止递归包含 -----------------------------------------------------------*/
#ifndef GLightSensor_H
#define GLightSensor_H

/* 包含头文件 ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
//#include "GConfig.h"

//#ifdef G_PORT_DEFINED
//#include "GPort.h"
//#endif // G_PORT_DEFINED

///**
// * Class: GLightSensor
// * \par 类描述
// * GLightSensor类的声明，用于驱动GMega光线传感器模块
// */

//class GLightSensor
//{
//public:

///**
// * 重载构造函数：将光线传感器映射到指定的开发板端口
// * \param[in]
// *   sensorPin - RJ25端口号，范围从PORT_1到M2
// */
//  GLightSensor(uint8_t sensorPin) ;

///**
// * 重载构造函数：自定义映射光线传感器和LED的Arduino端口
// * \param[in]
// *   ledPin - 连接LED灯的Arduino引脚编号
// * \param[in]
// *   sensorPin - 连接传感器数据读取的Arduino引脚编号
// */
//  GLightSensor(uint8_t ledPin, uint8_t sensorPin);

///**
// * \par 函数名称
// *    setpin
// * \par 函数描述
// *    设置光线传感器的引脚  \n
// * \param[in]
// *    sensorPin - 开发板上连接光线传感器的引脚编号
// * \par 输出参数
// *    无
// * \par 返回值
// *    无
// * \par 其他说明
// *    修正原错误注释：原注释错误描述为I2C七段数码管相关配置
// */
//  void setpin(uint8_t sensorPin);

///**
// * \par 函数名称
// *    setpin
// * \par 函数描述
// *    设置LED灯引脚和光线传感器引脚  \n
// * \param[in]
// *    ledPin - 开发板上连接传感器LED灯的引脚编号
// * \param[in]
// *    sensorPin - 开发板上连接光线传感器的引脚编号
// * \par 输出参数
// *    无
// * \par 返回值
// *    无
// * \par 其他说明
// *    修正原错误注释：原注释错误描述为I2C七段数码管相关配置；
// *    原注释中_KeyPin和s2为笔误，实际设置的是_ledPin和_sensorPin
// */
//  void setpin(uint8_t ledPin, uint8_t sensorPin);

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
// *    GPort::aRead2() - 当前光线传感器输出的模拟数值（DAC值）
// * \par 其他说明
// *    返回值范围通常为0-1023，值越小表示光线越强，值越大表示光线越弱
// */
//  int16_t read(void);

///**
// * \par 函数名称
// *    lightOn
// * \par 函数描述
// *    打开光线传感器模块上的LED灯
// * \param[in]
// *    无
// * \par 输出参数
// *    无
// * \par 返回值
// *    无
// * \par 其他说明
// *    LED灯通常为低电平触发点亮
// */
//  void lightOn(void);

///**
// * \par 函数名称
// *    lightOff
// * \par 函数描述
// *    关闭光线传感器模块上的LED灯
// * \param[in]
// *    无
// * \par 输出参数
// *    无
// * \par 返回值
// *    无
// * \par 其他说明
// *    LED灯通常为高电平触发熄灭
// */
//  void lightOff(void);

//private:
//  uint8_t _ledPin;   // LED灯引脚编号
//  uint8_t _sensorPin;// 光线传感器引脚编号
//};

#endif // GLightSensor_H