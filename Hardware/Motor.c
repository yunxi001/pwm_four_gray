#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "Motor.h"
#include "Serial.h"
#include "Delay.h"

// 通用PWM参数配置（所有定时器统一1kHz频率）
#define PWM_ARR 999 // 自动重装载值
#define PWM_PSC 71  // 预分频器：72MHz/(71+1)=1MHz → PWM频率=1MHz/1000=1kHz

// 电机1（TIM3）初始化（保持不变）
static void Motor1_TIM3_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    // 1. 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    // 2. 配置PC7(CH2)、PC8(CH3)为复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // 3. TIM3完全重映射（CH2=PC7、CH3=PC8）
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);

    // 4. TIM3时基配置
    TIM_TimeBaseStructure.TIM_Period = PWM_ARR;
    TIM_TimeBaseStructure.TIM_Prescaler = PWM_PSC;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // 5. PWM模式配置（CH2：M-，CH3：M+）
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    // CH2（PC7/M-）初始化
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

    // CH3（PC8/M+）初始化
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

    // 6. 启动TIM3
    TIM_ARRPreloadConfig(TIM3, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
}

// 电机2+3+4（TIM1+TIM8+TIM4）初始化 - 修改电机2使用PB9(TIM4_CH4)作为M-
static void Motor234_TIM1_TIM8_TIM4_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_BDTRInitTypeDef TIM_BDTRInitStructure;

    // -------------------- 第一步：使能时钟 --------------------
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                               RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO |
                               RCC_APB2Periph_TIM1 | RCC_APB2Periph_TIM8,
                           ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); // 新增TIM4时钟

    // -------------------- 第二步：GPIO配置 --------------------
    // 电机2：PB9(TIM4_CH4/M-)、PB14(TIM1_CH2N/M+)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 电机3：PA8(CH1/M+)、PC9(CH4/M-)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // 电机4：PC6(CH1/M+)、PB15(CH3N/M-)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // -------------------- 第三步：TIM1时基配置 --------------------
    TIM_TimeBaseStructure.TIM_Period = PWM_ARR;
    TIM_TimeBaseStructure.TIM_Prescaler = PWM_PSC;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    // -------------------- 第四步：TIM1 PWM配置 --------------------
    TIM_OCStructInit(&TIM_OCInitStructure);

    // ========== 电机2：TIM1_CH2N (PB14/M+) 配置 ==========
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC2Init(TIM1, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);

    // ========== 电机3：TIM1_CH1 (PA8/M+) 配置 ==========
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);

    // ========== 电机4：TIM1_CH3N (PB15/M-) 配置 ==========
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC3Init(TIM1, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);

    // -------------------- 第五步：TIM8配置 --------------------
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

    // 电机4：TIM8_CH1 (PC6/M+)
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputState_Disable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC1Init(TIM8, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);

    // 电机3：TIM8_CH4 (PC9/M-)
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC4Init(TIM8, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(TIM8, TIM_OCPreload_Enable);

    // -------------------- 第六步：TIM4配置（电机2的M-引脚） --------------------
    TIM_TimeBaseStructure.TIM_Period = PWM_ARR;
    TIM_TimeBaseStructure.TIM_Prescaler = PWM_PSC;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    // 电机2：TIM4_CH4 (PB9/M-)
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputState_Disable; // TIM4没有互补输出
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC4Init(TIM4, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);

    // -------------------- 第七步：高级定时器关键配置 --------------------
    TIM_BDTRStructInit(&TIM_BDTRInitStructure);
    TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
    TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
    TIM_BDTRInitStructure.TIM_DeadTime = 0;
    TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
    TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;
    TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
    TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);
    TIM_BDTRConfig(TIM8, &TIM_BDTRInitStructure);

    // 高级定时器必须使能主输出（MOE位）
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM8, ENABLE);

    // -------------------- 第八步：启动定时器 --------------------
    TIM_ARRPreloadConfig(TIM1, ENABLE);
    TIM_Cmd(TIM1, ENABLE);

    TIM_ARRPreloadConfig(TIM8, ENABLE);
    TIM_Cmd(TIM8, ENABLE);

    TIM_ARRPreloadConfig(TIM4, ENABLE);
    TIM_Cmd(TIM4, ENABLE);
}

// 所有电机PWM初始化入口函数
void PWM_Motors_Init(void)
{
    Motor1_TIM3_Init();
    Motor234_TIM1_TIM8_TIM4_Init(); // 修改函数名
}

// 设置指定电机的速度和方向 - 修改电机2的控制逻辑
void Motor_SetSpeed(Motor_ID motor_id, int16_t speed)
{
    // 边界保护：限制speed在±999范围内（匹配ARR=999）
    speed = (speed > 1000) ? 1000 : (speed < -1000) ? -1000
                                                    : speed;

    switch (motor_id)
    {
    case MOTOR1: // 电机1：TIM3_CH3(M+)、TIM3_CH2(M-)
        if (speed >= 0)
        {
            TIM_SetCompare3(TIM3, 0);
            TIM_SetCompare2(TIM3, speed);
        }
        else
        {
            TIM_SetCompare3(TIM3, -speed);
            TIM_SetCompare2(TIM3, 0);
        }
        break;

    case MOTOR2: // 电机2：TIM1_CH2N(M+)、TIM4_CH4(M-)
        if (speed >= 0)
        {
            TIM_SetCompare2(TIM1, 0);     // PB14输出PWM
            TIM_SetCompare4(TIM4, speed); // PB9输出0
        }
        else
        {
            TIM_SetCompare2(TIM1, -speed); // PB14输出0
            TIM_SetCompare4(TIM4, 0);      // PB9输出PWM
        }
        break;

    case MOTOR3: // 电机3：TIM1_CH1(M+)、TIM8_CH4(M-)
        if (speed >= 0)
        {
            TIM_SetCompare1(TIM1, speed);
            TIM_SetCompare4(TIM8, 0);
        }
        else
        {
            TIM_SetCompare1(TIM1, 0);
            TIM_SetCompare4(TIM8, -speed);
        }
        break;

    case MOTOR4: // 电机4：TIM8_CH1(M+)、TIM1_CH3N(M-)
        if (speed >= 0)
        {
            TIM_SetCompare1(TIM8, speed);
            TIM_SetCompare3(TIM1, 0);
        }
        else
        {
            TIM_SetCompare1(TIM8, 0);
            TIM_SetCompare3(TIM1, -speed);
        }
        break;

    default:
        break;
    }
}

// 硬件测试函数
void Test_Motor_Hardware(void)
{
    // Serial_Printf("=== 硬件直接驱动测试开始 ===\r\n");

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

    // 测试电机2
    Serial_Printf("测试电机2(PB14+, PB13-)...\r\n");
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_14;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_SetBits(GPIOB, GPIO_Pin_9);    // M+ = 高
    GPIO_ResetBits(GPIOB, GPIO_Pin_14); // M- = 低
    Serial_Printf("电机2正转测试\r\n");
    Delay_ms(2000);

    GPIO_ResetBits(GPIOB, GPIO_Pin_9);
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);
    Serial_Printf("电机2停止\r\n");
    Delay_ms(1000);

    // 测试电机4
    Serial_Printf("测试电机4(PC6+, PB15-)...\r\n");
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_SetBits(GPIOC, GPIO_Pin_6);    // M+ = 高
    GPIO_ResetBits(GPIOB, GPIO_Pin_15); // M- = 低
    Serial_Printf("电机4正转测试\r\n");
    Delay_ms(2000);

    GPIO_ResetBits(GPIOC, GPIO_Pin_6);
    GPIO_ResetBits(GPIOB, GPIO_Pin_15);
    Serial_Printf("电机4停止\r\n");
    Delay_ms(1000);

    // 测试电机1
    Serial_Printf("测试电机1(PC8+, PC7-)...\r\n");
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_7;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_SetBits(GPIOC, GPIO_Pin_8);   // M+ = 高
    GPIO_ResetBits(GPIOC, GPIO_Pin_7); // M- = 低
    Serial_Printf("电机1正转测试\r\n");
    Delay_ms(2000);

    GPIO_ResetBits(GPIOC, GPIO_Pin_8);
    GPIO_ResetBits(GPIOC, GPIO_Pin_7);
    Serial_Printf("电机1停止\r\n");
    Delay_ms(1000);

    // 测试电机3
    Serial_Printf("测试电机3(PA8+, PC9-)...\r\n");
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_SetBits(GPIOA, GPIO_Pin_8);   // M+ = 高
    GPIO_ResetBits(GPIOC, GPIO_Pin_9); // M- = 低
    Serial_Printf("电机3正转测试\r\n");
    Delay_ms(2000);

    GPIO_ResetBits(GPIOA, GPIO_Pin_8);
    GPIO_ResetBits(GPIOC, GPIO_Pin_9);
    Serial_Printf("电机3停止\r\n");
    Delay_ms(1000);

    Serial_Printf("=== 硬件测试结束 ===\r\n");
}

// 新的车轮控制接口实现
// 根据实际电机配置和速度方向映射：
// 电机1正转 → 右前反转，电机2正转 → 左前正转
// 电机3正转 → 右后正转，电机4正转 → 左后反转
void Set_Wheel_Speed(Wheel_Position wheel, int16_t speed)
{
    switch (wheel)
    {
    case WHEEL_FRONT_LEFT: // 左前轮 → MOTOR2（正转=正向）
        Motor_SetSpeed(MOTOR4, -speed);
        break;
    case WHEEL_FRONT_RIGHT: // 右前轮 → MOTOR1（正转=反向，需要反向映射）
        Motor_SetSpeed(MOTOR1, -speed);
        break;
    case WHEEL_REAR_LEFT: // 左后轮 → MOTOR4（正转=反向，需要反向映射）
        Motor_SetSpeed(MOTOR3, -speed);
        break;
    case WHEEL_REAR_RIGHT: // 右后轮 → MOTOR3（正转=正向）
        Motor_SetSpeed(MOTOR2, -speed);
        break;
        //        case WHEEL_FRONT_LEFT:   // 左前轮 → MOTOR2（正转=正向）
        //            Motor_SetSpeed(MOTOR2, speed);
        //            break;
        //
        //        case WHEEL_FRONT_RIGHT:  // 右前轮 → MOTOR1（正转=反向，需要反向映射）
        //            Motor_SetSpeed(MOTOR1, -speed);
        //            break;
        //        case WHEEL_REAR_LEFT:    // 左后轮 → MOTOR4（正转=反向，需要反向映射）
        //            Motor_SetSpeed(MOTOR4, -speed);
        //            break;
        //        case WHEEL_REAR_RIGHT:   // 右后轮 → MOTOR3（正转=正向）
        //            Motor_SetSpeed(MOTOR3, speed);
        //            break;
    default:
        break;
    }
}

// 四轮统一控制接口
void Set_All_Wheels(int16_t front_left, int16_t front_right, int16_t rear_left, int16_t rear_right)
{
    //    Set_Wheel_Speed(WHEEL_FRONT_LEFT, (int16_t)1.2*front_left);
    Set_Wheel_Speed(WHEEL_FRONT_LEFT, front_left);
    Set_Wheel_Speed(WHEEL_FRONT_RIGHT, front_right);
    Set_Wheel_Speed(WHEEL_REAR_LEFT, rear_left);
    Set_Wheel_Speed(WHEEL_REAR_RIGHT, rear_right);
}

// 车辆运动控制接口
void Car_Move_Forward(int16_t speed)
{
    // 前进：所有车轮正转
    Set_All_Wheels(speed, speed, speed, speed);
}

void Car_Move_Backward(int16_t speed)
{
    // 后退：所有车轮反转
    Set_All_Wheels(-speed, -speed, -speed, -speed);
}

void Car_Turn_Left(int16_t speed)
{
    // 左转：左轮反转，右轮正转
    Set_All_Wheels(-speed, speed, -speed, speed);
}

void Car_Turn_Right(int16_t speed)
{
    // 右转：左轮正转，右轮反转
    Set_All_Wheels(speed, -speed, speed, -speed);
}

void Car_Stop(void)
{
    // 停止：所有车轮停止
    Set_All_Wheels(0, 0, 0, 0);
}

/**
 * 电机位置和方向测试函数
 * 功能：逐一测试四个电机的位置和旋转方向，便于确认电机接线是否正确
 * 使用方法：通过观察每个电机在正负速度下的转动方向来判断接线是否正确
 */
void Motor_Position_Test(void)
{
    printf("=== 电机位置和方向测试开始 ===\r\n");
    printf("逐个测试每个电机的旋转方向...\r\n");
    printf("请观察每个轮子的转动方向并记录结果\r\n");
    Delay_ms(2000);

    // 测试电机1
    printf("\r\n--- 测试 MOTOR1 ---\r\n");
    printf("MOTOR1 速度 +800 (正转)\r\n");
    Motor_SetSpeed(MOTOR1, 800);
    Delay_ms(3000);
    Motor_SetSpeed(MOTOR1, 0);
    Delay_ms(1000);

    printf("MOTOR1 速度 -800 (反转)\r\n");
    Motor_SetSpeed(MOTOR1, -800);
    Delay_ms(3000);
    Motor_SetSpeed(MOTOR1, 0);
    Delay_ms(2000);

    // 测试电机2
    printf("\r\n--- 测试 MOTOR2 ---\r\n");
    printf("MOTOR2 速度 +800 (正转)\r\n");
    Motor_SetSpeed(MOTOR2, 800);
    Delay_ms(3000);
    Motor_SetSpeed(MOTOR2, 0);
    Delay_ms(1000);

    printf("MOTOR2 速度 -800 (反转)\r\n");
    Motor_SetSpeed(MOTOR2, -800);
    Delay_ms(3000);
    Motor_SetSpeed(MOTOR2, 0);
    Delay_ms(2000);

    // 测试电机3
    printf("\r\n--- 测试 MOTOR3 ---\r\n");
    printf("MOTOR3 速度 +800 (正转)\r\n");
    Motor_SetSpeed(MOTOR3, 800);
    Delay_ms(3000);
    Motor_SetSpeed(MOTOR3, 0);
    Delay_ms(1000);

    printf("MOTOR3 速度 -800 (反转)\r\n");
    Motor_SetSpeed(MOTOR3, -800);
    Delay_ms(3000);
    Motor_SetSpeed(MOTOR3, 0);
    Delay_ms(2000);

    // 测试电机4
    printf("\r\n--- 测试 MOTOR4 ---\r\n");
    printf("MOTOR4 速度 +800 (正转)\r\n");
    Motor_SetSpeed(MOTOR4, 800);
    Delay_ms(3000);
    Motor_SetSpeed(MOTOR4, 0);
    Delay_ms(1000);

    printf("MOTOR4 速度 -800 (反转)\r\n");
    Motor_SetSpeed(MOTOR4, -800);
    Delay_ms(3000);
    Motor_SetSpeed(MOTOR4, 0);
    Delay_ms(2000);

    printf("\r\n=== 电机测试完成 ===\r\n");
    printf("请按以下格式记录测试结果:\r\n");
    printf("MOTOR1: 位置=[前左/前右/后左/后右], +800=[正转/反转]\r\n");
    printf("MOTOR2: 位置=[前左/前右/后左/后右], +800=[正转/反转]\r\n");
    printf("MOTOR3: 位置=[前左/前右/后左/后右], +800=[正转/反转]\r\n");
    printf("MOTOR4: 位置=[前左/前右/后左/后右], +800=[正转/反转]\r\n");
}