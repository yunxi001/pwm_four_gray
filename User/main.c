#include "stm32f10x.h"
#include "misc.h"
#include <stdio.h>
#include "Delay.h"
#include "Encoder.h"
#include "Motor.h"
#include "Serial.h"
#include "Gservo.h"
#include "Timer.h" // 定时器模块，用于高频传感器检测
#include "Two_Gray_Line_Following.h"
#include "Two_gray_sensor.h"   // 两路灰度传感器模块
#include "TM1637.h"            // 数码管显示模块
#include "DHT11.h"             // 温湿度传感器模块（S2）
#include "Digital.h"           // 数字输入模块（S4D4）
#include "Gir_distance.h"      // 红外测距传感器模块（S6D6）
#include "brightness_sensor.h" // 光照传感器模块（S1）
#include "camera_protocol.h"

/* 串口发送缓冲区 - 用于与外部设备通信 */
uint8_t USART_TX_BUF[27] = {0XAA, 0X55, 0X04, 0X17, 0X04, 0X04, 0X01, 0X00, 0X00, 0X00, 0X00, 0X02, 0X00, 0X00, 0X00, 0X00, 0X03, 0X00, 0X00, 0X00, 0X00, 0X04, 0X00, 0X00, 0X00, 0X00, 0X00};

/* PWM控制相关全局变量 */
volatile uint16_t pwm_duty1 = 0, pwm_duty2 = 0, pwm_duty3 = 0, pwm_duty4 = 0;    // 四路PWM占空比变量
volatile uint8_t motor_dir1 = 0, motor_dir2 = 0, motor_dir3 = 0, motor_dir4 = 0; // 四路电机方向变量

#define CAMERA_ID 0x02
/* 编码器数据缓冲区 */
int encoder_buf[4];
extern volatile uint16_t Analog_data[3]; // 外部声明的模拟量数据数组(由灰度传感器模块提供)
extern volatile uint8_t sensor_read;
// 局部变量定义
uint8_t Digital_data[4] = {0}; // 数字输入数据缓冲区

/* 数码管轮换显示相关 */
// 显示通道枚举：0=光照(S1), 1=数字传感器(S4D4), 2=温湿度(S2), 3=红外测距(S6D6)
typedef enum
{
    DISP_LIGHT = 0,
    DISP_DIGITAL,
    DISP_DHT11,
    DISP_DISTANCE,
    DISP_MAX
} DispChannel_t;
static DispChannel_t disp_channel = DISP_LIGHT; // 当前显示通道
static uint8_t disp_sec_count = 0;              // 秒计数，每5秒切换一次

/* 函数声明 */
void Motor_Position_Test(void);   // 电机位置和方向测试函数
void Display_Sensor_Update(void); // 数码管传感器轮换显示更新
/*
 * 主函数
 * 功能：系统初始化并执行主循环任务
 */
int main(void)
{
    // 系统基础初始化
    SystemInit();                                   // STM32系统初始化
    SystemClock_Config();                           // 配置系统时钟至72MHz
    Systick_Init();                                 // 初始化滴答定时器，用于软件延时
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 配置NVIC中断优先级分组

    // 硬件模块初始化
    Serial_Init();          // 串口通信模块初始化
    S485_Init(1000000);     // RS485通信模块初始化(已禁用)
    Camera_Init();          // 摄像头模块初始化(已禁用)
    PWM_Motors_Init();      // PWM电机控制模块初始化
    Two_gray_sensor_Init(); // 两路灰度传感器初始化,s3d3和s1
    TM1637_Init();          // 数码管显示模块初始化,s5ds
    Digital_Init();         // 数字输入模块初始化,s4d4
    Delay_ms(500);
    Gir_distance_sensor_init(); // 红外测距传感器初始化,s6d6
    Delay_ms(500);              // 延时等待传感器稳定
    Gir_setMode(0);             // 设置红外测距传感器模式:0-高精度模式,1-低精度模式
    Delay_ms(500);              // 延时等待模式设置生效

    Timer_Init(); // 定时器模块初始化,1MS

    // // 可选功能：根据需要取消注释执行相应功能
    // Motor_Position_Test();               // 电机位置和方向测试
    // Calibrate_Gray_Sensors();            // 灰度传感器校准
    // Line_Following_With_Turns(700, 8.0, 10.0); // 巡线带转弯检测功能
        uint16_t aaa = 1099;
        TM1637_DisplayNumber(aaa);
    /* 主循环 - 执行传感器数据采集与处理 */
    while (1)
    {
        // 传感器数据采集（每1秒触发一次，在巡线 Delay_ms 间隙处理）
        if (sensor_read == 1)
        {
            sensor_read = 0;

            // 红外测距：I2C 读取较快，影响较小
            getDistance();

            // 数字传感器：GPIO 读取，几乎无延时
            Read_Digital(Digital_data);

            // 数码管轮换显示：每5次（5秒）切换一个通道
            disp_sec_count++;
            if (disp_sec_count >= 5)
            {
                disp_sec_count = 0;
                disp_channel = (DispChannel_t)((disp_channel + 1) % DISP_MAX);
            }

            // DHT11 读取有阻塞延时（约20ms），只在非 DISTANCE 通道时读取
            // 且每5秒才切换一次，所以只在切换到 DHT11 通道时读一次
            if (disp_channel == DISP_DHT11)
            {
                // 暂停巡线，读取 DHT11（约20ms阻塞）
                Car_Stop();
                DHT_Read();
            }

            Display_Sensor_Update();
        }

        Line_Following_With_Turns(999, 8.0, 12.0);
    }
}

//    S485_Dir_Control(1);
//    uint8_t result = Camera_SetMode(CAMERA_ID, FUNC_COLOR);
//    Delay_ms(500);
//		Car_Move_Forward(999);
//		S485_Dir_Control(1);
//		Set_Servo_position(0xfe,200,64);
//		Delay_ms(1000);
//
//        Car_Move_Backward(999);
//        S485_Dir_Control(1);
//		Set_Servo_position(0xfe,800,64);
//		Delay_ms(1000);

/*
 * 函数：Display_Sensor_Update
 * 功能：根据当前显示通道，将对应传感器数据显示到数码管
 *
 * 显示格式：
 *   DISP_LIGHT   (S1)   : 光照ADC原始值，0~4095
 *   DISP_DIGITAL (S4D4) : 两路数字值拼成4位，格式 "d1d2__"（高2位=D1,D2状态）
 *   DISP_DHT11   (S2)   : 温湿度拼显，格式 "TTHHxx"（前2位温度，后2位湿度）
 *   DISP_DISTANCE(S6D6) : 红外测距，单位0.1cm，最大9999（即999.9cm）
 */
void Display_Sensor_Update(void)
{
    uint16_t disp_val = 0;

    switch (disp_channel)
    {
    case DISP_LIGHT:
        // S1 光照传感器：显示ADC原始值（0~4095）
        disp_val = Analog_data[2];
        TM1637_DisplayNumber(disp_val);
//        printf("[DISP] Light(S1): %d\r\n", disp_val);
        break;

    case DISP_DIGITAL:
        // S4D4 两路数字传感器：D1显示千位，D2显示百位
        // 例如 D1=1,D2=0 -> 显示 1000
        disp_val = (uint16_t)(Digital_data[1] & 0x01) * 1000 + (uint16_t)(Digital_data[2] & 0x01) * 100;
        TM1637_DisplayNumber(disp_val);
//        printf("[DISP] Digital(S4D4): D1=%d D2=%d\r\n",
//               Digital_data[1], Digital_data[2]);
        break;

    case DISP_DHT11:
        // S2 DHT11：前2位=温度，后2位=湿度，例如温度25°C、湿度60% -> 显示 2560
        disp_val = (uint16_t)dht11_temp * 100 + (uint16_t)dht11_humi;
        TM1637_DisplayNumber(disp_val);
//        printf("[DISP] DHT11(S2): Temp=%d Humi=%d\r\n", dht11_temp, dht11_humi);
        break;

    case DISP_DISTANCE:
    {
        // S6D6 红外测距：单位cm，乘10后取整显示（保留1位小数）
        // 例如 12.3cm -> 显示 0123
        float dist_cm = getDistance();
        if (dist_cm < 0)
            dist_cm = 0;
        disp_val = (uint16_t)(dist_cm * 10 + 0.5f);
        if (disp_val > 9999)
            disp_val = 9999;
        TM1637_DisplayNumber(disp_val);
//        printf("[DISP] Distance(S6D6): %.1fcm\r\n", dist_cm);
        break;
    }

    default:
        break;
    }
}
