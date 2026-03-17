//#include "GfiveGray_line_following.h"

//extern GraySensorArray graySensor;
//// 使用extern声明外部变量和对象
//extern GEncoderOnBoard enmotor_1;
//extern GEncoderOnBoard enmotor_2;
//extern GEncoderOnBoard enmotor_3;
//extern GEncoderOnBoard enmotor_4;

///**************
//Five_Gray_FourDrive_PD_Patrol 函数为四驱五灰巡线PID增加
//***************** */
//void Five_Gray_FourDrive_PD_Patrol(u8 map,u8 speed, float kp, float kd)
//{
//	static float last_error = 0;
//	static float error      = 0;
//	static float output     = 0;
//	graySensor_read(&graySensor);
//	int16_t left1 = graySensor_getValue(&graySensor, 0); 
//	int16_t left2 = graySensor_getValue(&graySensor, 1);
//	int16_t mid = graySensor_getValue(&graySensor, 2);  
//	int16_t right1 = graySensor_getValue(&graySensor, 3); 
//	int16_t right2 = graySensor_getValue(&graySensor, 4);
//    // 归一化传感器值（0-1范围，0=黑，1=白）
//    float left1_norm = (left1- 280) / 600.0;
//    float left2_norm = (left2- 280) / 600.0;
//    float right1_norm = (right1 - 280) / 600.0;
//    float right2_norm = (right2 - 280) / 600.0;
//    //error = 510 * (right_norm - left_norm);
//    float w = 0;

//    if(map == 1) {
//        // 白底黑线：mid看到白底（偏离）时w应该大
//         w = 1.0 + abs(mid - 280) *1.0 / 1550;
//         //Serial.println(w);
//    } else {
//        // 黑底白线：mid看到黑底（偏离）时w应该大  
//        w = 0.8 + abs(800-mid) *1.0 / 1550;
//    }

//    if(map==1)
//    {
//        error = -(((float)(left2_norm *(-1)) + (float)(right1_norm * 1)+(float)(left1_norm *(-2)) + (float)(right2_norm * 2) ) *w *250);
//    }
//    else if(map==2)
//    {
//        error = -(((float)(left2_norm *1) + (float)(right1_norm * (-1))+(float)(left1_norm *2) + (float)(right2_norm *(-2) ) ) *w *250);
//    }
//    const int ERROR_DEAD_ZONE = 5; 
//    if (abs(error) < ERROR_DEAD_ZONE) 
//    {
//        error = 0;  // 微小误差视为0，不调整;
//    }
//    //Serial.println(error);
//	  output = kp * error + kd * (error - last_error);

//    //Serial.println(output);

//    enmotor_1.setMotorPwm((speed + output));
//    enmotor_2.setMotorPwm((speed + output));
//    enmotor_3.setMotorPwm(-(speed - output));
//    enmotor_4.setMotorPwm(-(speed - output));

//	  last_error = error;
//}


////自定义巡线到十字路口
//void FourDrive_Fivegray_Crossroads(u8 map,u8 speed, float kp, float kd)
//{
//  speed = constrain(speed, 0, 255);
//	while(1)
//	{
//     graySensor_read(&graySensor);
//	   u16 left1 = graySensor_getValue(&graySensor, 0); 
//	   u16 right2 = graySensor_getValue(&graySensor, 4);
//       if(map==1)
//       {
//          if(left1 < 450 && right2 < 450)
//          {
//            break;
//          }
//       }
//        else if(map==2)
//       {
//            if(left1 > 450 && right2 > 450)
//          {
//            break;
//          }
//       }
//        Five_Gray_FourDrive_PD_Patrol(map,speed,kp,kd);
//	}
//    enmotor_1.setMotorPwm(0);
//    enmotor_2.setMotorPwm(0);
//    enmotor_3.setMotorPwm(0);
//    enmotor_4.setMotorPwm(0);
//}

////自定义巡线到左路口
//void FourDrive_Fivegray_Leftintersection(u8 map,u8 speed, float kp, float kd)
//{
//	while(1)
//	{
//        graySensor_read(&graySensor);
//	    u16 left1 = graySensor_getValue(&graySensor, 0);
//      u16 left2 = graySensor_getValue(&graySensor, 1);
//        if(map==1)
//        {
//            if(left1 < 450 && left2 < 450)
//          {
//            break;
//          }
//        } 
//        else if(map==2)
//        {
//            if(left1 > 450 && left2 > 450)
//          {
//            break;
//          }
//        }
//        Five_Gray_FourDrive_PD_Patrol(map,speed,kp,kd);
//	}
//    enmotor_1.setMotorPwm(0);
//    enmotor_2.setMotorPwm(0);
//    enmotor_3.setMotorPwm(0);
//    enmotor_4.setMotorPwm(0);
//}

////自定义巡线到右路口
//void FourDrive_Fivegray_Rightintersection(u8 map,u8 speed, float kp, float kd)
//{
//	while(1)
//	{
//     graySensor_read(&graySensor);
//	   u16 right2 = graySensor_getValue(&graySensor, 4);
//     u16 right1 = graySensor_getValue(&graySensor, 3);
//       if(map==1)
//        {
//            if(right2 < 450 && right1 < 450)
//          {
//            break;
//          }
//        } 
//        else if(map==2)
//        {
//            if(right2 > 450 && right1 > 450)
//          {
//            break;
//          }
//        }
//        Five_Gray_FourDrive_PD_Patrol(map,speed,kp,kd);
//	}
//    enmotor_1.setMotorPwm(0);
//    enmotor_2.setMotorPwm(0);
//    enmotor_3.setMotorPwm(0);
//    enmotor_4.setMotorPwm(0);
//}



////默认巡线
//void Five_Gray_FourDrive_PD_Patrol_Default(u8 map,u8 speed)
//{
//	static float last_error = 0;
//	static float error      = 0;
//	static float output     = 0;
//  float kp = 0;
//  float kd = 0;
//	graySensor_read(&graySensor);
//	int16_t left1 = graySensor_getValue(&graySensor, 0); 
//	int16_t left2 = graySensor_getValue(&graySensor, 1);
//	int16_t mid = graySensor_getValue(&graySensor, 2);  
//	int16_t right1 = graySensor_getValue(&graySensor, 3); 
//	int16_t right2 = graySensor_getValue(&graySensor, 4);
//    // 归一化传感器值（0-1范围，0=黑，1=白）
//    float left1_norm = (left1- 280) / 600.0;
//    float left2_norm = (left2- 280) / 600.0;
//    float right1_norm = (right1 - 280) / 600.0;
//    float right2_norm = (right2 - 280) / 600.0;
//    //error = 510 * (right_norm - left_norm);
//    float w = 0.8 + abs(mid - 280) *1.0 / 1550;

//    /*
//    Serial.print("left1_norm:");
//    Serial.print(left1_norm);
//    Serial.print(" left2_norm:");
//    Serial.print(left2_norm); 
//    Serial.print(" w:");
//    Serial.print(w);
//    Serial.print(" right1_norm:");
//    Serial.print(right1_norm);
//    Serial.print(" right2_norm:");
//    Serial.println(right2_norm);
//    */      
//    speed = constrain(speed, 0, 255);
//    if(map==1)
//    {
//      if(speed<=125)
//      {
//        kp = 0.75;
//        kd = 1.4;
//      }
//      else if(speed<=175)
//      {
//        kp = 1.1;
//        kd = 2.1;      
//      }
//      else if(speed<=255)
//      {
//        kp = 1.5;
//        kd = 2.8;       
//      }      
//    }
//    else if(map==2)
//    {
//      if(speed<=125)
//      {
//        kp = 0.75;
//        kd = 0.35;
//      }
//      else if(speed<=175)
//      {
//        kp = 1;
//        kd = 0.46;      
//      }
//      else if(speed<=255)
//      {
//        kp = 1.5;
//        kd = 0.7;      
//      }      
//    }

//    if(map == 1) {
//        // 白底黑线：mid看到白底（偏离）时w应该大
//         w = 0.8 + abs(mid - 280) *1.0 / 1550;
//    } else {
//        // 黑底白线：mid看到黑底（偏离）时w应该大  
//        w = 0.8 + abs(800-mid) *1.0 / 1550;
//    }

//    if(map==1)
//    {
//        error = -(((float)(left2_norm *(-1)) + (float)(right1_norm * 1)+(float)(left1_norm *(-2)) + (float)(right2_norm * 2) ) *w *250);
//    }
//    else if(map==2)
//    {
//        error = -(((float)(left2_norm *1) + (float)(right1_norm * (-1))+(float)(left1_norm *2) + (float)(right2_norm *(-2) ) ) *w *250);
//    }
//    const int ERROR_DEAD_ZONE = 5; 
//    if (abs(error) < ERROR_DEAD_ZONE) 
//    {
//        error = 0;  // 微小误差视为0，不调整;
//    }
//    //Serial.println(error);
//	  output = kp * error + kd * (error - last_error);
//    
//    enmotor_1.setMotorPwm((speed + output));
//    enmotor_2.setMotorPwm((speed + output));
//    enmotor_3.setMotorPwm(-(speed - output));
//    enmotor_4.setMotorPwm(-(speed - output));

//	  last_error = error;
//}

////默认巡线到十字路口
//void FourDrive_Fivegray_Crossroads_Default(u8 map,u8 speed)
//{
//  float kp = 0;
//  float kd = 0;
//  speed = constrain(speed, 0, 255);
//    if(map==1)
//    {
//      if(speed<=125)
//      {
//        kp = 0.75;
//        kd = 1.4;
//      }
//      else if(speed<=175)
//      {
//        kp = 1.1;
//        kd = 2.1;      
//      }
//      else if(speed<=255)
//      {
//        kp = 1.5;
//        kd = 2.8;       
//      }      
//    }
//    else if(map==2)
//    {
//      if(speed<=125)
//      {
//        kp = 0.75;
//        kd = 0.35;
//      }
//      else if(speed<=175)
//      {
//        kp = 1;
//        kd = 0.46;      
//      }
//      else if(speed<=255)
//      {
//        kp = 1.5;
//        kd = 0.7;      
//      }      
//    }
//	while(1)
//	{
//     graySensor_read(&graySensor);
//	   u16 left1 = graySensor_getValue(&graySensor, 0); 
//	   u16 right2 = graySensor_getValue(&graySensor, 4);
//       if(map==1)
//       {
//          if(left1 < 450 && right2 < 450)
//          {
//            break;
//          }
//       }
//        else if(map==2)
//       {
//            if(left1 > 450 && right2 > 450)
//          {
//            break;
//          }
//       }
//        Five_Gray_FourDrive_PD_Patrol(map,speed,kp,kd);
//	}
//    enmotor_1.setMotorPwm(0);
//    enmotor_2.setMotorPwm(0);
//    enmotor_3.setMotorPwm(0);
//    enmotor_4.setMotorPwm(0);
//}

////默认巡线到左路口
//void FourDrive_Fivegray_Leftintersection_Default(u8 map,u8 speed)
//{
//  float kp = 0;
//  float kd = 0;
//  speed = constrain(speed, 0, 255);
//    if(map==1)
//    {
//      if(speed<=125)
//      {
//        kp = 0.75;
//        kd = 1.4;
//      }
//      else if(speed<=175)
//      {
//        kp = 1.1;
//        kd = 2.1;      
//      }
//      else if(speed<=255)
//      {
//        kp = 1.5;
//        kd = 2.8;       
//      }      
//    }
//    else if(map==2)
//    {
//      if(speed<=125)
//      {
//        kp = 0.75;
//        kd = 0.35;
//      }
//      else if(speed<=175)
//      {
//        kp = 1;
//        kd = 0.46;      
//      }
//      else if(speed<=255)
//      {
//        kp = 1.5;
//        kd = 0.7;      
//      }      
//    }
//	while(1)
//	{
//        graySensor_read(&graySensor);
//	    u16 left1 = graySensor_getValue(&graySensor, 0);
//      u16 left2 = graySensor_getValue(&graySensor, 1);
//        if(map==1)
//        {
//            if(left1 < 450 && left2 < 450)
//          {
//            break;
//          }
//        } 
//        else if(map==2)
//        {
//            if(left1 > 450 && left2 > 450)
//          {
//            break;
//          }
//        }
//        Five_Gray_FourDrive_PD_Patrol(map,speed,kp,kd);
//	}
//    enmotor_1.setMotorPwm(0);
//    enmotor_2.setMotorPwm(0);
//    enmotor_3.setMotorPwm(0);
//    enmotor_4.setMotorPwm(0);
//}

////默认巡线到右路口
//void FourDrive_Fivegray_Rightintersection_Default(u8 map,u8 speed)
//{
//  float kp = 0;
//  float kd = 0;
//  speed = constrain(speed, 0, 255);
//    if(map==1)
//    {
//      if(speed<=125)
//      {
//        kp = 0.75;
//        kd = 1.4;
//      }
//      else if(speed<=175)
//      {
//        kp = 1.1;
//        kd = 2.1;      
//      }
//      else if(speed<=255)
//      {
//        kp = 1.5;
//        kd = 2.8;       
//      }      
//    }
//    else if(map==2)
//    {
//      if(speed<=125)
//      {
//        kp = 0.75;
//        kd = 0.35;
//      }
//      else if(speed<=175)
//      {
//        kp = 1;
//        kd = 0.46;      
//      }
//      else if(speed<=255)
//      {
//        kp = 1.5;
//        kd = 0.7;      
//      }      
//    }  
//	while(1)
//	{
//     graySensor_read(&graySensor);
//	   u16 right2 = graySensor_getValue(&graySensor, 4);
//     u16 right1 = graySensor_getValue(&graySensor, 3);
//       if(map==1)
//        {
//            if(right2 < 450 && right1 < 450)
//          {
//            break;
//          }
//        } 
//        else if(map==2)
//        {
//            if(right2 > 450 && right1 > 450)
//          {
//            break;
//          }
//        }
//        Five_Gray_FourDrive_PD_Patrol(map,speed,kp,kd);
//	}
//    enmotor_1.setMotorPwm(0);
//    enmotor_2.setMotorPwm(0);
//    enmotor_3.setMotorPwm(0);
//    enmotor_4.setMotorPwm(0);
//}

////四驱五灰左转弯
//void FourDrive_Fivegray_Left(u8 map,u8 speed)
//{

//  if(map==1)
//  {
//    graySensor_read(&graySensor);
//    while(graySensor_getValue(&graySensor, 1) > 550)
//  {
//    enmotor_1.setMotorPwm(-speed);
//    enmotor_2.setMotorPwm(-speed);
//    enmotor_3.setMotorPwm(-speed);
//    enmotor_4.setMotorPwm(-speed);

//    delay(10);//连续读取灰度传感器时需要加15ms以上延迟，否则数据会乱，导致小车提前停止
//    graySensor_read(&graySensor);
//  }
//  }

//  else if(map==2)
//  {
//    
//    graySensor_read(&graySensor);
//    while(graySensor_getValue(&graySensor, 1) < 550)
//  {

//    enmotor_1.setMotorPwm(-speed);
//    enmotor_2.setMotorPwm(-speed);
//    enmotor_3.setMotorPwm(-speed);
//    enmotor_4.setMotorPwm(-speed);
//    delay(15);
//	  graySensor_read(&graySensor);
//    
//  }

//  }
//    enmotor_1.setMotorPwm(0);
//    enmotor_2.setMotorPwm(0);
//    enmotor_3.setMotorPwm(0);
//    enmotor_4.setMotorPwm(0);
//}
////四驱五灰右转弯
//void FourDrive_Fivegray_Right(u8 map,u8 speed)
//{

//  if(map==1)
//  {
//    graySensor_read(&graySensor);
//    while(graySensor_getValue(&graySensor, 3) > 550)
//  {
//    enmotor_1.setMotorPwm(speed);
//    enmotor_2.setMotorPwm(speed);
//    enmotor_3.setMotorPwm(speed);
//    enmotor_4.setMotorPwm(speed);

//    delay(10);//连续读取灰度传感器时需要加15ms以上延迟，否则数据会乱，导致小车提前停止
//    graySensor_read(&graySensor);
//  }
//  }

//  else if(map==2)
//  {
//    
//    graySensor_read(&graySensor);
//    while(graySensor_getValue(&graySensor, 3) < 550)
//  {

//    enmotor_1.setMotorPwm(speed);
//    enmotor_2.setMotorPwm(speed);
//    enmotor_3.setMotorPwm(speed);
//    enmotor_4.setMotorPwm(speed);
//    delay(15);
//	  graySensor_read(&graySensor);
//    
//  }

//  }
//    enmotor_1.setMotorPwm(0);
//    enmotor_2.setMotorPwm(0);
//    enmotor_3.setMotorPwm(0);
//    enmotor_4.setMotorPwm(0);
//}




///**************
//Five_Gray_TwoDrive_PD_Patrol 函数为二驱五灰巡线PID增加
//***************** */
//void Five_Gray_TwoDrive_PD_Patrol(u8 map,u8 speed, float kp, float kd)
//{
//	static float last_error = 0;
//	static float error      = 0;
//	static float output     = 0;
//	graySensor_read(&graySensor);
//	int16_t left1 = graySensor_getValue(&graySensor, 0); 
//	int16_t left2 = graySensor_getValue(&graySensor, 1);
//	int16_t mid = graySensor_getValue(&graySensor, 2);  
//	int16_t right1 = graySensor_getValue(&graySensor, 3); 
//	int16_t right2 = graySensor_getValue(&graySensor, 4);
//    // 归一化传感器值（0-1范围，0=黑，1=白）
//    float left1_norm = (left1- 280) / 600.0;
//    float left2_norm = (left2- 280) / 600.0;
//    float right1_norm = (right1 - 280) / 600.0;
//    float right2_norm = (right2 - 280) / 600.0;

//    //error = 510 * (right_norm - left_norm);
//    float w = 0;

//    if(map == 1) {
//        // 白底黑线：mid看到白底（偏离）时w应该大
//        w = 1.0 + abs(mid - 280) *1.0 / 1550;
//    } else {
//        // 黑底白线：mid看到黑底（偏离）时w应该大  
//        w = 0.8 + abs(800-mid) *1.0 / 1550;
//    }

//    if(map==1)
//    {
//        error = -(((float)(left2_norm *(-1)) + (float)(right1_norm * 1)+(float)(left1_norm *(-2)) + (float)(right2_norm * 2) ) *w *250);
//    }
//    else if(map==2)
//    {
//        error = -(((float)(left2_norm *1) + (float)(right1_norm * (-1))+(float)(left1_norm *2) + (float)(right2_norm *(-2) ) ) *w *250);
//    }
//    const int ERROR_DEAD_ZONE = 10; 
//    if (abs(error) < ERROR_DEAD_ZONE) 
//    {
//        error = 0;  // 微小误差视为0，不调整;
//    }
//    //Serial.println(error);
//	  output = kp * error + kd * (error - last_error);

//    enmotor_1.setMotorPwm((speed + output));
//    enmotor_2.setMotorPwm(-(speed - output));
//    
//	  last_error = error;


//}


////自定义巡线到十字路口
//void TwoDrive_Fivegray_Crossroads(u8 map,u8 speed, float kp, float kd)
//{
//  speed = constrain(speed, 0, 255);
//	while(1)
//	{
//     graySensor_read(&graySensor);
//	   u16 left1 = graySensor_getValue(&graySensor, 0); 
//	   u16 right2 = graySensor_getValue(&graySensor, 4);
//       if(map==1)
//       {
//          if(left1 < 450 && right2 < 450)
//          {
//            break;
//          }
//       }
//        else if(map==2)
//       {
//            if(left1 > 450 && right2 > 450)
//          {
//            break;
//          }
//       }
//        Five_Gray_TwoDrive_PD_Patrol(map,speed,kp,kd);
//	}
//    enmotor_1.setMotorPwm(0);
//    enmotor_2.setMotorPwm(0);

//}

////自定义巡线到左路口
//void TwoDrive_Fivegray_Leftintersection(u8 map,u8 speed, float kp, float kd)
//{
//	while(1)
//	{
//        graySensor_read(&graySensor);
//	    u16 left1 = graySensor_getValue(&graySensor, 0);
//      u16 left2 = graySensor_getValue(&graySensor, 1);
//        if(map==1)
//        {
//            if(left1 < 450 && left2 < 450)
//          {
//            break;
//          }
//        } 
//        else if(map==2)
//        {
//            if(left1 > 450 && left2 > 450)
//          {
//            break;
//          }
//        }
//        Five_Gray_TwoDrive_PD_Patrol(map,speed,kp,kd);
//	}
//    enmotor_1.setMotorPwm(0);
//    enmotor_2.setMotorPwm(0);
//}

////自定义巡线到右路口
//void TwoDrive_Fivegray_Rightintersection(u8 map,u8 speed, float kp, float kd)
//{
//	while(1)
//	{
//     graySensor_read(&graySensor);
//	   u16 right2 = graySensor_getValue(&graySensor, 4);
//     u16 right1 = graySensor_getValue(&graySensor, 3);
//       if(map==1)
//        {
//            if(right2 < 450 && right1 < 450)
//          {
//            break;
//          }
//        } 
//        else if(map==2)
//        {
//            if(right2 > 450 && right1 > 450)
//          {
//            break;
//          }
//        }
//        Five_Gray_TwoDrive_PD_Patrol(map,speed,kp,kd);
//	}
//    enmotor_1.setMotorPwm(0);
//    enmotor_2.setMotorPwm(0);

//}



////默认巡线
//void Five_Gray_TwoDrive_PD_Patrol_Default(u8 map,u8 speed)
//{
//	static float last_error = 0;
//	static float error      = 0;
//	static float output     = 0;
//  float kp = 0;
//  float kd = 0;
//	graySensor_read(&graySensor);
//	int16_t left1 = graySensor_getValue(&graySensor, 0); 
//	int16_t left2 = graySensor_getValue(&graySensor, 1);
//	int16_t mid = graySensor_getValue(&graySensor, 2);  
//	int16_t right1 = graySensor_getValue(&graySensor, 3); 
//	int16_t right2 = graySensor_getValue(&graySensor, 4);
//    // 归一化传感器值（0-1范围，0=黑，1=白）
//    float left1_norm = (left1- 280) / 600.0;
//    float left2_norm = (left2- 280) / 600.0;
//    float right1_norm = (right1 - 280) / 600.0;
//    float right2_norm = (right2 - 280) / 600.0;
//    //error = 510 * (right_norm - left_norm);
//    float w = 0.8 + abs(mid - 280) *1.0 / 1550;

//    speed = constrain(speed, 0, 255);
//    if(map==1)
//    {
//      if(speed<=125)
//      {
//        kp = 0.7;
//        kd = 3.5;
//      }
//      else if(speed<=175)
//      {
//        kp = 0.9;
//        kd = 5;      
//      }
//      else if(speed<=255)
//      {
//        kp = 1.5;
//        kd = 7;       
//      }      
//    }
//    else if(map==2)
//    {
//      if(speed<=125)
//      {
//        kp = 0.8;
//        kd = 3.8;
//      }
//      else if(speed<=175)
//      {
//        kp = 0.95;
//        kd = 5.5;      
//      }
//      else if(speed<=255)
//      {
//        kp = 1.6;
//        kd = 7.5;       
//      }        
//    }

//    if(map == 1) {
//        // 白底黑线：mid看到白底（偏离）时w应该大
//         w = 0.8 + abs(mid - 280) *1.0 / 1550;
//    } else {
//        // 黑底白线：mid看到黑底（偏离）时w应该大  
//        w = 0.8 + abs(800-mid) *1.0 / 1550;
//    }

//    if(map==1)
//    {
//        error = -(((float)(left2_norm *(-1)) + (float)(right1_norm * 1)+(float)(left1_norm *(-2)) + (float)(right2_norm * 2) ) *w *250);
//    }
//    else if(map==2)
//    {
//        error = -(((float)(left2_norm *1) + (float)(right1_norm * (-1))+(float)(left1_norm *2) + (float)(right2_norm *(-2) ) ) *w *250);
//    }
//    const int ERROR_DEAD_ZONE = 5; 
//    if (abs(error) < ERROR_DEAD_ZONE) 
//    {
//        error = 0;  // 微小误差视为0，不调整;
//    }
//    //Serial.println(error);
//	  output = kp * error + kd * (error - last_error);
//    
//    enmotor_1.setMotorPwm((speed + output));
//    enmotor_2.setMotorPwm(-(speed - output));

//	  last_error = error;
//}

////默认巡线到十字路口
//void TwoDrive_Fivegray_Crossroads_Default(u8 map,u8 speed)
//{
//  float kp = 0;
//  float kd = 0;
//    speed = constrain(speed, 0, 255);
//    if(map==1)
//    {
//      if(speed<=125)
//      {
//        kp = 0.7;
//        kd = 3.5;
//      }
//      else if(speed<=175)
//      {
//        kp = 0.9;
//        kd = 5;      
//      }
//      else if(speed<=255)
//      {
//        kp = 1.5;
//        kd = 7;       
//      }      
//    }
//    else if(map==2)
//    {
//      if(speed<=125)
//      {
//        kp = 0.75;
//        kd = 0.35;
//      }
//      else if(speed<=175)
//      {
//        kp = 1;
//        kd = 0.46;      
//      }
//      else if(speed<=255)
//      {
//        kp = 1.5;
//        kd = 0.7;      
//      }      
//    }
//	while(1)
//	{
//     graySensor_read(&graySensor);
//	   u16 left1 = graySensor_getValue(&graySensor, 0); 
//	   u16 right2 = graySensor_getValue(&graySensor, 4);
//       if(map==1)
//       {
//          if(left1 < 450 && right2 < 450)
//          {
//            break;
//          }
//       }
//        else if(map==2)
//       {
//            if(left1 > 450 && right2 > 450)
//          {
//            break;
//          }
//       }
//        Five_Gray_TwoDrive_PD_Patrol(map,speed,kp,kd);
//	}
//    enmotor_1.setMotorPwm(0);
//    enmotor_2.setMotorPwm(0);
//}

////默认巡线到左路口
//void TwoDrive_Fivegray_Leftintersection_Default(u8 map,u8 speed)
//{
//  float kp = 0;
//  float kd = 0;
//  speed = constrain(speed, 0, 255);
//    if(map==1)
//    {
//      if(speed<=125)
//      {
//        kp = 0.75;
//        kd = 1.4;
//      }
//      else if(speed<=175)
//      {
//        kp = 1.1;
//        kd = 2.1;      
//      }
//      else if(speed<=255)
//      {
//        kp = 1.5;
//        kd = 2.8;       
//      }      
//    }
//    else if(map==2)
//    {
//      if(speed<=125)
//      {
//        kp = 0.75;
//        kd = 0.35;
//      }
//      else if(speed<=175)
//      {
//        kp = 1;
//        kd = 0.46;      
//      }
//      else if(speed<=255)
//      {
//        kp = 1.5;
//        kd = 0.7;      
//      }      
//    }
//	while(1)
//	{
//        graySensor_read(&graySensor);
//	    u16 left1 = graySensor_getValue(&graySensor, 0);
//      u16 left2 = graySensor_getValue(&graySensor, 1);
//        if(map==1)
//        {
//            if(left1 < 450 && left2 < 450)
//          {
//            break;
//          }
//        } 
//        else if(map==2)
//        {
//            if(left1 > 450 && left2 > 450)
//          {
//            break;
//          }
//        }
//        Five_Gray_TwoDrive_PD_Patrol(map,speed,kp,kd);
//	}
//    enmotor_1.setMotorPwm(0);
//    enmotor_2.setMotorPwm(0);
//}

////默认巡线到右路口
//void TwoDrive_Fivegray_Rightintersection_Default(u8 map,u8 speed)
//{
//  float kp = 0;
//  float kd = 0;
//  speed = constrain(speed, 0, 255);
//    if(map==1)
//    {
//      if(speed<=125)
//      {
//        kp = 0.75;
//        kd = 1.4;
//      }
//      else if(speed<=175)
//      {
//        kp = 1.1;
//        kd = 2.1;      
//      }
//      else if(speed<=255)
//      {
//        kp = 1.5;
//        kd = 2.8;       
//      }      
//    }
//    else if(map==2)
//    {
//      if(speed<=125)
//      {
//        kp = 0.75;
//        kd = 0.35;
//      }
//      else if(speed<=175)
//      {
//        kp = 1;
//        kd = 0.46;      
//      }
//      else if(speed<=255)
//      {
//        kp = 1.5;
//        kd = 0.7;      
//      }      
//    }  
//	while(1)
//	{
//     graySensor_read(&graySensor);
//	   u16 right2 = graySensor_getValue(&graySensor, 4);
//     u16 right1 = graySensor_getValue(&graySensor, 3);
//       if(map==1)
//        {
//            if(right2 < 450 && right1 < 450)
//          {
//            break;
//          }
//        } 
//        else if(map==2)
//        {
//            if(right2 > 450 && right1 > 450)
//          {
//            break;
//          }
//        }
//        Five_Gray_TwoDrive_PD_Patrol(map,speed,kp,kd);
//	}
//    enmotor_1.setMotorPwm(0);
//    enmotor_2.setMotorPwm(0);
//}

////二驱五灰左转弯
//void TwoDrive_Fivegray_Left(u8 map,u8 speed)
//{

//  if(map==1)
//  {
//    graySensor_read(&graySensor);
//    while(graySensor_getValue(&graySensor, 1) > 550)
//  {
//    enmotor_1.setMotorPwm(-speed);
//    enmotor_2.setMotorPwm(-speed);

//    delay(10);//连续读取灰度传感器时需要加15ms以上延迟，否则数据会乱，导致小车提前停止
//    graySensor_read(&graySensor);
//  }
//  }

//  else if(map==2)
//  {
//    
//    graySensor_read(&graySensor);
//    while(graySensor_getValue(&graySensor, 1) < 550)
//  {

//    enmotor_1.setMotorPwm(-speed);
//    enmotor_2.setMotorPwm(-speed);
//    delay(15);
//	  graySensor_read(&graySensor);
//    
//  }

//  }
//    enmotor_1.setMotorPwm(0);
//    enmotor_2.setMotorPwm(0);
//}
////二驱五灰右转弯
//void TwoDrive_Fivegray_Right(u8 map,u8 speed)
//{

//  if(map==1)
//  {
//    graySensor_read(&graySensor);
//    while(graySensor_getValue(&graySensor, 3) > 550)
//  {
//    enmotor_1.setMotorPwm(speed);
//    enmotor_2.setMotorPwm(speed);

//    delay(10);//连续读取灰度传感器时需要加15ms以上延迟，否则数据会乱，导致小车提前停止
//    graySensor_read(&graySensor);
//  }
//  }

//  else if(map==2)
//  {
//    
//    graySensor_read(&graySensor);
//    while(graySensor_getValue(&graySensor, 3) < 550)
//  {

//    enmotor_1.setMotorPwm(speed);
//    enmotor_2.setMotorPwm(speed);
//    delay(15);
//	  graySensor_read(&graySensor);
//    
//  }

//  }
//    enmotor_1.setMotorPwm(0);
//    enmotor_2.setMotorPwm(0);
//}
