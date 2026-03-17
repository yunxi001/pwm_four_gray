#include "GrayPatrol.h"

// 使用extern声明外部变量和对象
extern GEncoderOnBoard enmotor_1;
extern GEncoderOnBoard enmotor_2;
extern GEncoderOnBoard enmotor_3;
extern GEncoderOnBoard enmotor_4;

extern const int grayscalePin1;
extern const int grayscalePin2;

// 定义变量（使用新名称）
unsigned char gray_patrol_gmega_dc_Port;
unsigned char gray_patrol_gPort;

/**************
Two_Gray_TwoDrive_PD_Patrol 函数为四驱二灰巡线PID增加的
***************** */
void Two_Gray_FourDrive_PD_Patrol(u8 map,u8 Speed, float kp, float kd)
{
    static double last_error = 0;
    static double error  = 0;
    static double output = 0;
    // 归一化传感器值（0-1范围，0=黑，1=白）
    float left_norm = (analogRead(grayscalePin1) - 120) / 430.0;
    float right_norm = (analogRead(grayscalePin2) - 120) / 430.0;
    if(map==1)
    {
         error = 510 * (right_norm - left_norm);
    }
    else if(map==2)
    {
         error = 510 * (left_norm - right_norm);
    }
     // 增加误差死区（阈值可根据实际情况调整，比如±5）
    const int ERROR_DEAD_ZONE = 5; 
    if (abs(error) < ERROR_DEAD_ZONE) {
        error = 0;  // 微小误差视为0，不调整
    }
    output = kp * error + kd * (error - last_error);

    enmotor_1.setMotorPwm((Speed - output));
    enmotor_2.setMotorPwm((Speed - output));
    enmotor_3.setMotorPwm(-(Speed + output));
    enmotor_4.setMotorPwm(-(Speed + output));
    
    last_error = error;
}

/**************
Two_Gray_TwoDrive_PD_Patrol 函数为四驱二灰巡线PID增加的
***************** */
void Two_Gray_FourDrive_PD_Patrol_Default(u8 map,u8 Speed)
{
    static double last_error = 0;
    static double error  = 0;
    static double output = 0;
    float kp =0;
    float kd =0;

    // 归一化传感器值（0-1范围，0=黑，1=白）
    float left_norm = (analogRead(grayscalePin1) - 120) / 430.0;
    float right_norm = (analogRead(grayscalePin2) - 120) / 430.0;

    if(map==1)
    {

        if(Speed<=64)
        {
            kp = 0.15;
            kd = 3;		
        }
        else if(Speed<=128)
        {
            kp = 0.3;
            kd = 6;		
        }
        else if(Speed<=255)
        {
            kp = 0.55;
            kd = 12;		
        }
        error = 510 * (right_norm - left_norm);
    }
    else if(map==2)
    {
        if(Speed<=64)
        {
            kp = 0.15;
            kd = 4;		
        }
        else if(Speed<=128)
        {
            kp = 0.3;
            kd = 9;		
        }
        else if(Speed<=255)
        {
            kp = 0.55;
            kd = 18;		
        }
        error = 510 * (left_norm - right_norm);
    }
     // 增加误差死区（阈值可根据实际情况调整，比如±5）
    const int ERROR_DEAD_ZONE = 2; 
    if (abs(error) < ERROR_DEAD_ZONE) {
        error = 0;  // 微小误差视为0，不调整
    }

    output = kp * error + kd * (error - last_error);

    enmotor_1.setMotorPwm((Speed - output));
    enmotor_2.setMotorPwm((Speed - output));
    enmotor_3.setMotorPwm(-(Speed + output));
    enmotor_4.setMotorPwm(-(Speed + output));
    
    last_error = error;
}


//四驱二灰左路口停
void FourDrive_Twogray_Leftintersection(u8 map,u8 Speed, float kp, float kd)
{
    u16 i;
    int confirm_count = 0;  
    const int CONFIRM_THRESHOLD = 5; 
    const unsigned long SAMPLING_INTERVAL = 2; // 采样间隔独立定义
    unsigned long last_sampling_time = millis(); // 采样时间戳初始化

    while(1)
    {
        i = analogRead(grayscalePin1);
        bool is_intersection = false;
        if(map==1)
        {
            if(i < 250) is_intersection = true;
        }
        if(map==2)
        {
            if(i > 400) is_intersection = true;
        }

        // 第一步：判断是否到达采样间隔（仅每10ms执行一次判断）
        unsigned long current_time = millis();
        if(current_time - last_sampling_time >= SAMPLING_INTERVAL)
        {
            last_sampling_time = current_time; // 更新采样时间戳


            if(is_intersection)
            {
                confirm_count++; 
            }
            else
            {
                confirm_count = 0; 
            }

            if(confirm_count >= CONFIRM_THRESHOLD)
            {
                break;
            }
        }

        Two_Gray_FourDrive_PD_Patrol(map,Speed,kp,kd);
    }

    // 停止所有电机
    enmotor_1.setMotorPwm(0);
    enmotor_2.setMotorPwm(0);
    enmotor_3.setMotorPwm(0);
    enmotor_4.setMotorPwm(0);
}

//四驱二灰默认左路口停
void FourDrive_Twogray_Leftintersection_Default(u8 map,u8 Speed)
{
    float kp =0;
    float kd =0;
    if(map==1)
    {
        if(Speed<=64)
        {
            kp = 0.15;
            kd = 3;		
        }
        else if(Speed<=128)
        {
            kp = 0.3;
            kd = 6;		
        }
        else if(Speed<=255)
        {
            kp = 0.6;
            kd = 12;		
        }
    }
    else if(map==2)
    {
        if(Speed<=64)
        {
            kp = 0.15;
            kd = 3;		
        }
        else if(Speed<=128)
        {
            kp = 0.3;
            kd = 6;		
        }
        else if(Speed<=255)
        {
            kp = 1.0;
            kd = 12;		
        }
    }

    u16 i;
    int confirm_count = 0;  
    const int CONFIRM_THRESHOLD = 5; 
    const unsigned long SAMPLING_INTERVAL = 2; // 采样间隔独立定义
    unsigned long last_sampling_time = millis(); // 采样时间戳初始化

    while(1)
    {
        i = analogRead(grayscalePin1);
        bool is_intersection = false;
        if(map==1)
        {
            if(i < 250) is_intersection = true;
        }
        if(map==2)
        {
            if(i > 400) is_intersection = true;
        }

        // 第一步：判断是否到达采样间隔（仅每10ms执行一次判断）
        unsigned long current_time = millis();
        if(current_time - last_sampling_time >= SAMPLING_INTERVAL)
        {
            last_sampling_time = current_time; // 更新采样时间戳


            if(is_intersection)
            {
                confirm_count++; 
            }
            else
            {
                confirm_count = 0; 
            }

            if(confirm_count >= CONFIRM_THRESHOLD)
            {
                break;
            }
        }

        Two_Gray_FourDrive_PD_Patrol(map,Speed,kp,kd);
    }

    // 停止所有电机
    enmotor_1.setMotorPwm(0);
    enmotor_2.setMotorPwm(0);
    enmotor_3.setMotorPwm(0);
    enmotor_4.setMotorPwm(0);
}


//四驱二灰右路口停
void FourDrive_Twogray_Rightintersection(u8 map,u8 Speed, float kp, float kd)
{
    u16 i;
    int confirm_count = 0;  
    const int CONFIRM_THRESHOLD = 5; 
    const unsigned long SAMPLING_INTERVAL = 2; // 采样间隔独立定义
    unsigned long last_sampling_time = millis(); // 采样时间戳初始化

    while(1)
    {
        i = analogRead(grayscalePin2);
        bool is_intersection = false;
        if(map==1)
        {
            if(i < 250) is_intersection = true;
        }
        if(map==2)
        {
            if(i > 400) is_intersection = true;
        }

        // 第一步：判断是否到达采样间隔（仅每10ms执行一次判断）
        unsigned long current_time = millis();
        if(current_time - last_sampling_time >= SAMPLING_INTERVAL)
        {
            last_sampling_time = current_time; // 更新采样时间戳


            if(is_intersection)
            {
                confirm_count++; 
            }
            else
            {
                confirm_count = 0; 
            }

            if(confirm_count >= CONFIRM_THRESHOLD)
            {
                break;
            }
        }

        Two_Gray_FourDrive_PD_Patrol(map,Speed,kp,kd);
    }

    // 停止所有电机
    enmotor_1.setMotorPwm(0);
    enmotor_2.setMotorPwm(0);
    enmotor_3.setMotorPwm(0);
    enmotor_4.setMotorPwm(0);
}

//四驱二灰默认右路口停
void FourDrive_Twogray_Rightintersection_Default(u8 map,u8 Speed)
{
    float kp =0;
    float kd =0;
    if(map==1)
    {
        if(Speed<=64)
        {
            kp = 0.15;
            kd = 3;		
        }
        else if(Speed<=128)
        {
            kp = 0.3;
            kd = 6;		
        }
        else if(Speed<=255)
        {
            kp = 0.6;
            kd = 12;		
        }
    }
    else if(map==2)
    {
        if(Speed<=64)
        {
            kp = 0.15;
            kd = 3;		
        }
        else if(Speed<=128)
        {
            kp = 0.3;
            kd = 6;		
        }
        else if(Speed<=255)
        {
            kp = 1.0;
            kd = 12;		
        }
    }

    u16 i;
    int confirm_count = 0;  
    const int CONFIRM_THRESHOLD = 5; 
    const unsigned long SAMPLING_INTERVAL = 2; // 采样间隔独立定义
    unsigned long last_sampling_time = millis(); // 采样时间戳初始化

    while(1)
    {
        i = analogRead(grayscalePin2);
        bool is_intersection = false;
        if(map==1)
        {
            if(i < 250) is_intersection = true;
        }
        if(map==2)
        {
            if(i > 400) is_intersection = true;
        }

        // 第一步：判断是否到达采样间隔（仅每10ms执行一次判断）
        unsigned long current_time = millis();
        if(current_time - last_sampling_time >= SAMPLING_INTERVAL)
        {
            last_sampling_time = current_time; // 更新采样时间戳


            if(is_intersection)
            {
                confirm_count++; 
            }
            else
            {
                confirm_count = 0; 
            }

            if(confirm_count >= CONFIRM_THRESHOLD)
            {
                break;
            }
        }

        Two_Gray_FourDrive_PD_Patrol(map,Speed,kp,kd);
    }

    // 停止所有电机
    enmotor_1.setMotorPwm(0);
    enmotor_2.setMotorPwm(0);
    enmotor_3.setMotorPwm(0);
    enmotor_4.setMotorPwm(0);
}


//四驱二灰十字路口停
void FourDrive_Twogray_Crossroads(u8 map,u8 Speed, float kp, float kd)
{
    u16 i, j;
    while(1)
    {
        i = analogRead(grayscalePin1);
        j = analogRead(grayscalePin2);
        if(map==1)
        {
            if(i < 330 && j< 330)
           {
               break;
           }
        }
        if(map==2)
        {
            if(i > 330 && j> 330)
           {
               break;
           }
        }
        Two_Gray_FourDrive_PD_Patrol(map,Speed,kp,kd);
    }
    enmotor_1.setMotorPwm(0);
    enmotor_2.setMotorPwm(0);
    enmotor_3.setMotorPwm(0);
    enmotor_4.setMotorPwm(0);
}

//四驱二灰默认十字路口停
void FourDrive_Twogray_Crossroads_Default(u8 map,u8 Speed)
{
    float kp =0;
    float kd =0;

    if(map==1)
    {

        if(Speed<=64)
        {
            kp = 0.15;
            kd = 3;		
        }
        else if(Speed<=128)
        {
            kp = 0.3;
            kd = 6;		
        }
        else if(Speed<=255)
        {
            kp = 0.55;
            kd = 14;		
        }

    }
    else if(map==2)
    {
        if(Speed<=64)
        {
            kp = 0.15;
            kd = 3;		
        }
        else if(Speed<=128)
        {
            kp = 0.3;
            kd = 6;		
        }
        else if(Speed<=255)
        {
            kp = 0.6;
            kd = 17;		
        }
    }
    u16 i, j;
    while(1)
    {
        i = analogRead(grayscalePin1);
        j = analogRead(grayscalePin2);
        if(map==1)
        {
            if(i < 330 && j<330)
           {
               break;
           }
        }
        if(map==2)
        {
            if(i > 330 && j> 330)
           {
               break;
           }
        }
        Two_Gray_FourDrive_PD_Patrol(map,Speed,kp,kd);
    }
    enmotor_1.setMotorPwm(0);
    enmotor_2.setMotorPwm(0);
    enmotor_3.setMotorPwm(0);
    enmotor_4.setMotorPwm(0);
}


//四驱二灰左转弯
void FourDrive_Twogray_Left(u8 map,u8 Speed)
{
    if(map==1)
    {
        while(analogRead(grayscalePin1) > 300)
        { 
            enmotor_1.setMotorPwm(-Speed);
            enmotor_2.setMotorPwm(-Speed);
            enmotor_3.setMotorPwm(-Speed);
            enmotor_4.setMotorPwm(-Speed);
        }
        
        while(analogRead(grayscalePin1) < 300)
        {
            enmotor_1.setMotorPwm(-Speed/2);
            enmotor_2.setMotorPwm(-Speed/2);
            enmotor_3.setMotorPwm(-Speed/2);
            enmotor_4.setMotorPwm(-Speed/2);
        }
    }
    else if(map==2)
    {
        while(analogRead(grayscalePin1) < 300)
        { 
            enmotor_1.setMotorPwm(-Speed);
            enmotor_2.setMotorPwm(-Speed);
            enmotor_3.setMotorPwm(-Speed);
            enmotor_4.setMotorPwm(-Speed);
        }

        while(analogRead(grayscalePin1) > 290)
        {
            enmotor_1.setMotorPwm(-Speed);
            enmotor_2.setMotorPwm(-Speed);
            enmotor_3.setMotorPwm(-Speed);
            enmotor_4.setMotorPwm(-Speed);
        }
    }
    enmotor_1.setMotorPwm(0);
    enmotor_2.setMotorPwm(0);
    enmotor_3.setMotorPwm(0);
    enmotor_4.setMotorPwm(0);
    delay(100);
}


//四驱二灰右转弯
void FourDrive_Twogray_Right(u8 map,u8 Speed)
{
    if(map==1)
    {
        while(analogRead(grayscalePin2) > 300)
        { 
            enmotor_1.setMotorPwm(Speed);
            enmotor_2.setMotorPwm(Speed);
            enmotor_3.setMotorPwm(Speed);
            enmotor_4.setMotorPwm(Speed);
        }
        while(analogRead(grayscalePin2) < 300)
        {
            enmotor_1.setMotorPwm(Speed/2);
            enmotor_2.setMotorPwm(Speed/2);
            enmotor_3.setMotorPwm(Speed/2);
            enmotor_4.setMotorPwm(Speed/2);
        }
    }
    else if(map==2)
    {
        while(analogRead(grayscalePin2)< 300)
        { 
            enmotor_1.setMotorPwm(Speed);
            enmotor_2.setMotorPwm(Speed);
            enmotor_3.setMotorPwm(Speed);
            enmotor_4.setMotorPwm(Speed);
        }
        while(analogRead(grayscalePin2)> 290)
        {
            enmotor_1.setMotorPwm(Speed/2);
            enmotor_2.setMotorPwm(Speed/2);
            enmotor_3.setMotorPwm(Speed/2);
            enmotor_4.setMotorPwm(Speed/2);
        }
    }
    enmotor_1.setMotorPwm(0);
    enmotor_2.setMotorPwm(0);
    enmotor_3.setMotorPwm(0);
    enmotor_4.setMotorPwm(0);
    delay(100);
}
    


/**************
Two_Gray_TwoDrive_PD_Patrol 函数为二驱二灰巡线PID增加的
***************** */
void Two_Gray_TwoDrive_PD_Patrol(u8 map,u8 Speed, float kp, float kd)
{
    static double last_error = 0;
    static double error  = 0;
    static double output = 0;
    // 归一化传感器值（0-1范围，0=黑，1=白）
    float left_norm = (analogRead(grayscalePin1) - 120) / 430.0;
    float right_norm = (analogRead(grayscalePin2) - 120) / 430.0;
    if(map==1)
    {
         error = 510 * (right_norm - left_norm);
    }
    else if(map==2)
    {
         error = 510 * (left_norm - right_norm);
    }
     // 增加误差死区（阈值可根据实际情况调整，比如±5）
    const int ERROR_DEAD_ZONE = 5; 
    if (abs(error) < ERROR_DEAD_ZONE) {
        error = 0;  // 微小误差视为0，不调整
    }
    output = kp * error + kd * (error - last_error);

    enmotor_1.setMotorPwm((Speed - output));
    enmotor_2.setMotorPwm(-(Speed + output));
    
    last_error = error;
}

/**************
Two_Gray_TwoDrive_PD_Patrol 函数为二驱二灰巡线PID增加的
***************** */
void Two_Gray_TwoDrive_PD_Patrol_Default(u8 map,u8 Speed)
{
    static double last_error = 0;
    static double error  = 0;
    static double output = 0;
    float kp =0;
    float kd =0;

    // 归一化传感器值（0-1范围，0=黑，1=白）
    float left_norm = (analogRead(grayscalePin1) - 120) / 430.0;
    float right_norm = (analogRead(grayscalePin2) - 120) / 430.0;

    if(map==1)
    {

        if(Speed<=64)
        {
            kp = 0.2;
            kd = 30;		
        }
        else if(Speed<=128)
        {
            kp = 0.425;
            kd = 60;		
        }
        
        else if(Speed<=255)
        {
            kp = 0.85;
            kd = 120;
            		
        }
        error = 510 * (right_norm - left_norm);
    }
    else if(map==2)
    {
        if(Speed<=64)
        {
            kp = 0.2;
            kd = 30;		
        }
        else if(Speed<=128)
        {
            kp = 0.425;
            kd = 60;		
        }
        
        else if(Speed<=255)
        {
            kp = 0.85;
            kd = 120;
            		
        }
        error = 510 * (left_norm - right_norm);
    }
     // 增加误差死区（阈值可根据实际情况调整，比如±5）
    const int ERROR_DEAD_ZONE = 2; 
    if (abs(error) < ERROR_DEAD_ZONE) {
        error = 0;  // 微小误差视为0，不调整
    }

    output = kp * error + kd * (error - last_error);

    enmotor_1.setMotorPwm((Speed - output));
    enmotor_2.setMotorPwm(-(Speed + output));
    
    last_error = error;
}


//二驱二灰左路口停
void TwoDrive_Twogray_Leftintersection(u8 map,u8 Speed, float kp, float kd)
{
    u16 i;
    int confirm_count = 0;  
    const int CONFIRM_THRESHOLD = 5; 
    const unsigned long SAMPLING_INTERVAL = 2; // 采样间隔独立定义
    unsigned long last_sampling_time = millis(); // 采样时间戳初始化

    while(1)
    {
        i = analogRead(grayscalePin1);
        bool is_intersection = false;
        if(map==1)
        {
            if(i < 250) is_intersection = true;
        }
        if(map==2)
        {
            if(i > 400) is_intersection = true;
        }

        // 第一步：判断是否到达采样间隔（仅每10ms执行一次判断）
        unsigned long current_time = millis();
        if(current_time - last_sampling_time >= SAMPLING_INTERVAL)
        {
            last_sampling_time = current_time; // 更新采样时间戳

            if(is_intersection)
            {
                confirm_count++; 
            }
            else
            {
                confirm_count = 0; 
            }

            if(confirm_count >= CONFIRM_THRESHOLD)
            {
                break;
            }
        }

        Two_Gray_TwoDrive_PD_Patrol(map,Speed,kp,kd);
    }

    // 停止所有电机
    enmotor_1.setMotorPwm(0);
    enmotor_2.setMotorPwm(0);
}

//二驱二灰默认左路口停
void TwoDrive_Twogray_Leftintersection_Default(u8 map,u8 Speed)
{
    float kp =0;
    float kd =0;
    if(map==1)
    {
        if(Speed<=64)
        {
            kp = 0.2;
            kd = 30;		
        }
        else if(Speed<=128)
        {
            kp = 0.425;
            kd = 60;		
        }
        
        else if(Speed<=255)
        {
            kp = 0.85;
            kd = 120;
            		
        }
    }
    else if(map==2)
    {
        if(Speed<=64)
        {
            kp = 0.2;
            kd = 30;		
        }
        else if(Speed<=128)
        {
            kp = 0.425;
            kd = 60;		
        }
        
        else if(Speed<=255)
        {
            kp = 0.85;
            kd = 120;
            		
        }
    }

    u16 i;
    int confirm_count = 0;  
    const int CONFIRM_THRESHOLD = 5; 
    const unsigned long SAMPLING_INTERVAL = 2; // 采样间隔独立定义
    unsigned long last_sampling_time = millis(); // 采样时间戳初始化

    while(1)
    {
        i = analogRead(grayscalePin1);
        bool is_intersection = false;
        if(map==1)
        {
            if(i < 250) is_intersection = true;
        }
        if(map==2)
        {
            if(i > 400) is_intersection = true;
        }

        // 第一步：判断是否到达采样间隔（仅每10ms执行一次判断）
        unsigned long current_time = millis();
        if(current_time - last_sampling_time >= SAMPLING_INTERVAL)
        {
            last_sampling_time = current_time; // 更新采样时间戳


            if(is_intersection)
            {
                confirm_count++; 
            }
            else
            {
                confirm_count = 0; 
            }

            if(confirm_count >= CONFIRM_THRESHOLD)
            {
                break;
            }
        }

        Two_Gray_TwoDrive_PD_Patrol(map,Speed,kp,kd);
    }

    // 停止所有电机
    enmotor_1.setMotorPwm(0);
    enmotor_2.setMotorPwm(0);
}


//二驱二灰右路口停
void TwoDrive_Twogray_Rightintersection(u8 map,u8 Speed, float kp, float kd)
{
    u16 i;
    int confirm_count = 0;  
    const int CONFIRM_THRESHOLD = 5; 
    const unsigned long SAMPLING_INTERVAL = 2; // 采样间隔独立定义
    unsigned long last_sampling_time = millis(); // 采样时间戳初始化

    while(1)
    {
        i = analogRead(grayscalePin2);
        bool is_intersection = false;
        if(map==1)
        {
            if(i < 250) is_intersection = true;
        }
        if(map==2)
        {
            if(i > 330) is_intersection = true;
        }

        unsigned long current_time = millis();
        if(current_time - last_sampling_time >= SAMPLING_INTERVAL)
        {
            last_sampling_time = current_time; // 更新采样时间戳


            if(is_intersection)
            {
                confirm_count++; 
            }
            else
            {
                confirm_count = 0; 
            }

            if(confirm_count >= CONFIRM_THRESHOLD)
            {
                break;
            }
        }

        Two_Gray_TwoDrive_PD_Patrol(map,Speed,kp,kd);
    }

    // 停止所有电机
    enmotor_1.setMotorPwm(0);
    enmotor_2.setMotorPwm(0);
}

//二驱二灰默认右路口停
void TwoDrive_Twogray_Rightintersection_Default(u8 map,u8 Speed)
{
    float kp =0;
    float kd =0;
    if(map==1)
    {
        if(Speed<=64)
        {
            kp = 0.2;
            kd = 30;		
        }
        else if(Speed<=128)
        {
            kp = 0.425;
            kd = 60;		
        }
        
        else if(Speed<=255)
        {
            kp = 0.85;
            kd = 120;
            		
        }
    }
    else if(map==2)
    {
        if(Speed<=64)
        {
            kp = 0.2;
            kd = 30;		
        }
        else if(Speed<=128)
        {
            kp = 0.425;
            kd = 60;		
        }
        
        else if(Speed<=255)
        {
            kp = 0.85;
            kd = 120;           		
        }
    }

    u16 i;
    int confirm_count = 0;  
    const int CONFIRM_THRESHOLD = 5; 
    const unsigned long SAMPLING_INTERVAL = 2; // 采样间隔独立定义
    unsigned long last_sampling_time = millis(); // 采样时间戳初始化

    while(1)
    {
        i = analogRead(grayscalePin2);
        bool is_intersection = false;
        if(map==1)
        {
            if(i < 250) is_intersection = true;
        }
        if(map==2)
        {
            if(i > 330) is_intersection = true;
        }

        // 第一步：判断是否到达采样间隔（仅每10ms执行一次判断）
        unsigned long current_time = millis();
        if(current_time - last_sampling_time >= SAMPLING_INTERVAL)
        {
            last_sampling_time = current_time; // 更新采样时间戳


            if(is_intersection)
            {
                confirm_count++; 
            }
            else
            {
                confirm_count = 0; 
            }

            if(confirm_count >= CONFIRM_THRESHOLD)
            {
                break;
            }
        }

        Two_Gray_TwoDrive_PD_Patrol(map,Speed,kp,kd);
    }

    // 停止所有电机
    enmotor_1.setMotorPwm(0);
    enmotor_2.setMotorPwm(0);
}


//二驱二灰十字路口停
void TwoDrive_Twogray_Crossroads(u8 map,u8 Speed, float kp, float kd)
{
    u16 i, j;
    while(1)
    {
        i = analogRead(grayscalePin1);
        j = analogRead(grayscalePin2);
        if(map==1)
        {
            if(i < 330 && j< 330)
           {
               break;
           }
        }
        if(map==2)
        {
            if(i > 330 && j> 330)
           {
               break;
           }
        }
        Two_Gray_TwoDrive_PD_Patrol(map,Speed,kp,kd);
    }
    enmotor_1.setMotorPwm(0);
    enmotor_2.setMotorPwm(0);
}

//二驱二灰默认十字路口停
void TwoDrive_Twogray_Crossroads_Default(u8 map,u8 Speed)
{
    float kp =0;
    float kd =0;

    if(map==1)
    {

        if(Speed<=64)
        {
            kp = 0.2;
            kd = 30;		
        }
        else if(Speed<=128)
        {
            kp = 0.425;
            kd = 60;		
        }
        
        else if(Speed<=255)
        {
            kp = 0.85;
            kd = 120;
            		
        }

    }
    else if(map==2)
    {
        if(Speed<=64)
        {
            kp = 0.2;
            kd = 30;		
        }
        else if(Speed<=128)
        {
            kp = 0.425;
            kd = 60;		
        }
        
        else if(Speed<=255)
        {
            kp = 0.85;
            kd = 120;           		
        }
    }
    u16 i, j;
    while(1)
    {
        i = analogRead(grayscalePin1);
        j = analogRead(grayscalePin2);
        if(map==1)
        {
            if(i < 330 && j<330)
           {
               break;
           }
        }
        if(map==2)
        {
            if(i > 330 && j> 330)
           {
               break;
           }
        }
        Two_Gray_TwoDrive_PD_Patrol(map,Speed,kp,kd);
    }
    enmotor_1.setMotorPwm(0);
    enmotor_2.setMotorPwm(0);

}


//二驱二灰左转弯
void TwoDrive_Twogray_Left(u8 map,u8 Speed)
{
    if(map==1)
    {
        while(analogRead(grayscalePin1) > 300)
        { 
            enmotor_1.setMotorPwm(-Speed);
            enmotor_2.setMotorPwm(-Speed);
        }
        
        while(analogRead(grayscalePin1) < 320)
        {
            enmotor_1.setMotorPwm(-Speed);
            enmotor_2.setMotorPwm(-Speed);
        }
    }
    else if(map==2)
    {
        while(analogRead(grayscalePin1) < 300)
        { 
            enmotor_1.setMotorPwm(-Speed);
            enmotor_2.setMotorPwm(-Speed);
        }

        while(analogRead(grayscalePin1) > 290)
        {
            enmotor_1.setMotorPwm(-Speed);
            enmotor_2.setMotorPwm(-Speed);
        }
    }
    enmotor_1.setMotorPwm(0);
    enmotor_2.setMotorPwm(0);
    delay(100);
}


//二驱二灰右转弯
void TwoDrive_Twogray_Right(u8 map,u8 Speed)
{
    if(map==1)
    {
        while(analogRead(grayscalePin2) > 300)
        { 
            enmotor_1.setMotorPwm(Speed);
            enmotor_2.setMotorPwm(Speed);
        }
        while(analogRead(grayscalePin2) < 320)
        {
            enmotor_1.setMotorPwm(Speed);
            enmotor_2.setMotorPwm(Speed);
        }
    }
    else if(map==2)
    {
        while(analogRead(grayscalePin2)< 300)
        { 
            enmotor_1.setMotorPwm(Speed);
            enmotor_2.setMotorPwm(Speed);
        }
        while(analogRead(grayscalePin2)> 290)
        {
            enmotor_1.setMotorPwm(Speed/2);
            enmotor_2.setMotorPwm(Speed/2);
        }
    }
    enmotor_1.setMotorPwm(0);
    enmotor_2.setMotorPwm(0);
    delay(100);
}