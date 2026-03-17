#ifndef GRAY_PATROL_H
#define GRAY_PATROL_H

#include <stdint.h>
#include <Arduino.h>
#include <GMega.h>  // 包含GBot.h以获取完整定义

// 使用extern声明外部变量
extern GEncoderOnBoard enmotor_1;
extern GEncoderOnBoard enmotor_2;
extern GEncoderOnBoard enmotor_3;
extern GEncoderOnBoard enmotor_4;

extern const int grayscalePin1;
extern const int grayscalePin2;

// 四驱巡线函数声明
void Two_Gray_FourDrive_PD_Patrol(u8 map,u8 Speed, float kp, float kd);
void FourDrive_Twogray_Leftintersection(u8 map,u8 speed, float kp, float kd);
void FourDrive_Twogray_Rightintersection(u8 map,u8 speed, float kp, float kd);
void FourDrive_Twogray_Crossroads(u8 map,u8 speed, float kp, float kd);
void FourDrive_Twogray_Left(u8 map,u8 speed);
void FourDrive_Twogray_Right(u8 map,u8 speed);
void Two_Gray_FourDrive_PD_Patrol_Default(u8 map,u8 Speed);
void FourDrive_Twogray_Leftintersection_Default(u8 map,u8 speed);
void FourDrive_Twogray_Rightintersection_Default(u8 map,u8 speed);
void FourDrive_Twogray_Crossroads_Default(u8 map,u8 speed);

// 二驱巡线函数声明Two
void Two_Gray_TwoDrive_PD_Patrol(u8 map,u8 Speed, float kp, float kd);
void TwoDrive_Twogray_Leftintersection(u8 map,u8 speed, float kp, float kd);
void TwoDrive_Twogray_Rightintersection(u8 map,u8 speed, float kp, float kd);
void TwoDrive_Twogray_Crossroads(u8 map,u8 speed, float kp, float kd);
void TwoDrive_Twogray_Left(u8 map,u8 speed);
void TwoDrive_Twogray_Right(u8 map,u8 speed);
void Two_Gray_TwoDrive_PD_Patrol_Default(u8 map,u8 Speed);
void TwoDrive_Twogray_Leftintersection_Default(u8 map,u8 speed);
void TwoDrive_Twogray_Rightintersection_Default(u8 map,u8 speed);
void TwoDrive_Twogray_Crossroads_Default(u8 map,u8 speed);

#endif // GRAY_PATROL_H