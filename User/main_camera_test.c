/**
 * @file main_camera_test.c
 * @brief RS485相机通信测试程序
 * @note 测试新版本相机协议的各项功能
 */

#include "stm32f10x.h"
#include "Delay.h"
#include "Serial.h"
#include "camera_protocol.h"
#include "LED.h"
#include <stdio.h>

// 相机设备ID（根据实际硬件设置）
#define CAMERA_ID 0x01

/**
 * @brief 测试相机模式切换
 */
void Test_CameraMode(void)
{
    printf("\n========== 测试模式切换 ==========\n");

    // 切换到颜色识别模式
    printf("切换到颜色识别模式...\n");
    uint8_t result = Camera_SetMode(CAMERA_ID, FUNC_COLOR);
    if (result == RESULT_SUCCESS)
    {
        printf("✓ 模式切换成功\n");
    }
    else
    {
        printf("✗ 模式切换失败: 0x%02X\n", result);
    }

    Delay_ms(500);
}

/**
 * @brief 测试颜色识别（指定模式）
 */
void Test_ColorRecognition_Specify(void)
{
    printf("\n========== 测试指定颜色识别 ==========\n");

    // 测试识别红色
    printf("检测红色...\n");
    uint8_t result = Camera_ReadColorSpec(CAMERA_ID, COLOR_RED);
    if (result == RESULT_YES)
    {
        printf("✓ 检测到红色\n");
    }
    else if (result == RESULT_NO)
    {
        printf("○ 未检测到红色\n");
    }
    else
    {
        printf("✗ 识别失败: 0x%02X\n", result);
    }

    Delay_ms(500);

    // 测试识别蓝色
    printf("检测蓝色...\n");
    result = Camera_ReadColorSpec(CAMERA_ID, COLOR_BLUE);
    if (result == RESULT_YES)
    {
        printf("✓ 检测到蓝色\n");
    }
    else if (result == RESULT_NO)
    {
        printf("○ 未检测到蓝色\n");
    }
    else
    {
        printf("✗ 识别失败: 0x%02X\n", result);
    }

    Delay_ms(500);
}

/**
 * @brief 测试颜色识别（非指定模式）
 */
void Test_ColorRecognition_NonSpecify(void)
{
    printf("\n========== 测试非指定颜色识别 ==========\n");

    uint8_t detected_color = 0;
    uint8_t result = Camera_ReadColorNonSpec(CAMERA_ID, &detected_color);

    if (result != 0)
    {
        printf("✓ 检测到颜色: ");
        switch (detected_color)
        {
        case COLOR_RED:
            printf("红色\n");
            break;
        case COLOR_GREEN:
            printf("绿色\n");
            break;
        case COLOR_BLUE:
            printf("蓝色\n");
            break;
        case COLOR_YELLOW:
            printf("黄色\n");
            break;
        case COLOR_GRAY:
            printf("灰色\n");
            break;
        case COLOR_PURPLE:
            printf("紫色\n");
            break;
        case COLOR_WHITE:
            printf("白色\n");
            break;
        case COLOR_BLACK:
            printf("黑色\n");
            break;
        default:
            printf("未知颜色(0x%02X)\n", detected_color);
        }
    }
    else
    {
        printf("○ 未检测到颜色\n");
    }

    Delay_ms(500);
}

/**
 * @brief 测试数字识别
 */
void Test_NumberRecognition(void)
{
    printf("\n========== 测试数字识别 ==========\n");

    // 切换到数字识别模式
    printf("切换到数字识别模式...\n");
    if (Camera_SetMode(CAMERA_ID, FUNC_NUMBER) == RESULT_SUCCESS)
    {
        printf("✓ 模式切换成功\n");
        Delay_ms(500);

        // 读取识别到的数字
        uint8_t number = 0;
        uint8_t result = Camera_ReadNumberNonSpec(CAMERA_ID, &number);

        if (result != 0)
        {
            printf("✓ 检测到数字: %d\n", number);
        }
        else
        {
            printf("○ 未检测到数字\n");
        }
    }
    else
    {
        printf("✗ 模式切换失败\n");
    }

    Delay_ms(500);
}

/**
 * @brief 测试人脸识别
 */
void Test_FaceRecognition(void)
{
    printf("\n========== 测试人脸识别 ==========\n");

    // 切换到人脸识别模式
    printf("切换到人脸识别模式...\n");
    if (Camera_SetMode(CAMERA_ID, FUNC_FACE) == RESULT_SUCCESS)
    {
        printf("✓ 模式切换成功\n");
        Delay_ms(500);

        // 读取识别到的人脸ID
        uint8_t face_id = 0;
        uint8_t result = Camera_ReadFaceNonSpec(CAMERA_ID, &face_id);

        if (result != 0)
        {
            printf("✓ 检测到人脸 ID: %d\n", face_id);
        }
        else
        {
            printf("○ 未检测到人脸\n");
        }
    }
    else
    {
        printf("✗ 模式切换失败\n");
    }

    Delay_ms(500);
}

/**
 * @brief 测试标签识别
 */
void Test_LabelRecognition(void)
{
    printf("\n========== 测试标签识别 ==========\n");

    // 切换到标签识别模式
    printf("切换到标签识别模式...\n");
    if (Camera_SetMode(CAMERA_ID, FUNC_LABEL) == RESULT_SUCCESS)
    {
        printf("✓ 模式切换成功\n");
        Delay_ms(500);

        // 读取识别到的标签
        uint8_t label = 0;
        uint8_t result = Camera_ReadLabelNonSpec(CAMERA_ID, &label);

        if (result != 0)
        {
            printf("✓ 检测到标签: %d\n", label);
        }
        else
        {
            printf("○ 未检测到标签\n");
        }
    }
    else
    {
        printf("✗ 模式切换失败\n");
    }

    Delay_ms(500);
}

/**
 * @brief 测试设备控制
 */
void Test_DeviceControl(void)
{
    printf("\n========== 测试设备控制 ==========\n");

    // 测试设置亮度
    printf("设置亮度为50%%...\n");
    uint8_t result = Camera_DeviceControl(CAMERA_ID, CTRL_BRIGHTNESS, 50);
    if (result == RESULT_SUCCESS)
    {
        printf("✓ 亮度设置成功\n");
    }
    else
    {
        printf("✗ 亮度设置失败: 0x%02X\n", result);
    }

    Delay_ms(500);
}

/**
 * @brief 连续监测模式（实时显示识别结果）
 */
void ContinuousMonitor_Color(void)
{
    printf("\n========== 连续颜色监测模式 ==========\n");
    printf("按任意键退出...\n\n");

    // 切换到颜色识别模式
    Camera_SetMode(CAMERA_ID, FUNC_COLOR);
    Delay_ms(500);

    uint8_t count = 0;
    while (count < 20)
    { // 监测20次
        uint8_t detected_color = 0;
        uint8_t result = Camera_ReadColorNonSpec(CAMERA_ID, &detected_color);

        if (result != 0)
        {
            printf("[%02d] 颜色: ", count);
            switch (detected_color)
            {
            case COLOR_RED:
                printf("红色   ");
                break;
            case COLOR_GREEN:
                printf("绿色   ");
                break;
            case COLOR_BLUE:
                printf("蓝色   ");
                break;
            case COLOR_YELLOW:
                printf("黄色   ");
                break;
            case COLOR_GRAY:
                printf("灰色   ");
                break;
            case COLOR_PURPLE:
                printf("紫色   ");
                break;
            case COLOR_WHITE:
                printf("白色   ");
                break;
            case COLOR_BLACK:
                printf("黑色   ");
                break;
            default:
                printf("未知(0x%02X)", detected_color);
            }
            printf("\n");
        }
        else
        {
            printf("[%02d] 未检测到颜色\n", count);
        }
        count++;
        Delay_ms(200);
    }
    printf("\n监测结束\n");
}

/**
 * @brief 主函数
 */
int main(void)
{
    // 系统初始化
    Systick_Init();
    LED_Init();

    // 串口初始化（波特率根据相机设置）
    S485_Init(115200);

    // 相机协议初始化
    Camera_Init();

    printf("\n");
    printf("========================================\n");
    printf("    RS485相机通信测试程序\n");
    printf("    相机ID: 0x%02X\n", CAMERA_ID);
    printf("========================================\n");

    Delay_ms(1000);

    // 运行测试
    while (1)
    {
        // 测试1: 模式切换
        Test_CameraMode();
        Delay_ms(1000);

        // 测试2: 指定颜色识别
        Test_ColorRecognition_Specify();
        Delay_ms(1000);

        // 测试3: 非指定颜色识别
        Test_ColorRecognition_NonSpecify();
        Delay_ms(1000);

        // 测试4: 数字识别
        Test_NumberRecognition();
        Delay_ms(1000);

        // 测试5: 人脸识别
        Test_FaceRecognition();
        Delay_ms(1000);

        // 测试6: 标签识别
        Test_LabelRecognition();
        Delay_ms(1000);

        // 测试7: 设备控制
        Test_DeviceControl();
        Delay_ms(1000);

        // 测试8: 连续监测
        ContinuousMonitor_Color();
        Delay_ms(2000);

        printf("\n========== 所有测试完成 ==========\n");
        printf("5秒后重新开始...\n\n");
        Delay_ms(5000);
    }
}

/**
 * @brief 在主循环中必须定期调用Camera_Process()
 * @note 如果使用RTOS，可以在任务中调用
 */
void SysTick_Handler(void)
{
    sys_tick_counter++;

    // 定期处理相机接收数据
    Camera_Process();
}
