/**
 * @file main_camera_simple_test.c
 * @brief RS485相机简单测试程序
 * @note 快速验证相机通信是否正常
 */

#include "stm32f10x.h"
#include "Delay.h"
#include "Serial.h"
#include "camera_protocol.h"
#include "LED.h"
#include <stdio.h>

// 相机设备ID
#define CAMERA_ID 0x01

int main(void)
{
    // 系统初始化
    Systick_Init();
    LED_Init();

    // 串口初始化（波特率115200）
    S485_Init(115200);

    // 相机协议初始化
    Camera_Init();

    printf("\n========================================\n");
    printf("  RS485相机简单测试\n");
    printf("  相机ID: 0x%02X\n", CAMERA_ID);
    printf("========================================\n\n");

    Delay_ms(1000);

    // 测试1: 切换到颜色识别模式
    printf("1. 切换到颜色识别模式...\n");
    uint8_t result = Camera_SetMode(CAMERA_ID, FUNC_COLOR);
    if (result == RESULT_SUCCESS)
    {
        printf("   ✓ 成功\n");
    }
    else
    {
        printf("   ✗ 失败 (返回: 0x%02X)\n", result);
    }
    Delay_ms(1000);

    // 主循环：持续读取颜色
    printf("\n2. 开始颜色识别...\n");
    printf("   (每秒读取一次)\n\n");

    uint8_t count = 0;
    while (1)
    {
        // 必须定期调用Camera_Process()处理接收数据
        Camera_Process();

        // 每秒读取一次
        if (count % 10 == 0)
        {
            uint8_t detected_color = 0;
            result = Camera_ReadColorNonSpec(CAMERA_ID, &detected_color);

            printf("[%03d] ", count / 10);

            if (result != 0)
            {
                // LED指示：检测到颜色时闪烁
                LED1_ON();

                switch (detected_color)
                {
                case COLOR_RED:
                    printf("检测到: 红色\n");
                    break;
                case COLOR_GREEN:
                    printf("检测到: 绿色\n");
                    break;
                case COLOR_BLUE:
                    printf("检测到: 蓝色\n");
                    break;
                case COLOR_YELLOW:
                    printf("检测到: 黄色\n");
                    break;
                case COLOR_GRAY:
                    printf("检测到: 灰色\n");
                    break;
                case COLOR_PURPLE:
                    printf("检测到: 紫色\n");
                    break;
                case COLOR_WHITE:
                    printf("检测到: 白色\n");
                    break;
                case COLOR_BLACK:
                    printf("检测到: 黑色\n");
                    break;
                default:
                    printf("检测到: 未知颜色(0x%02X)\n", detected_color);
                }

                Delay_ms(100);
                LED1_OFF();
            }
            else
            {
                printf("未检测到颜色\n");
            }
        }
        count++;
        Delay_ms(100);
    }
}
