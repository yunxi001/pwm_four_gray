#include "GServo.h"
#include "Serial.h"

// 接收缓冲区
uint8_t rx_buffer[RX_BUFFER_SIZE];
uint8_t rx_count = 0;
uint8_t rx_flag = 0;

// 舵机指令定义
uint8_t Servo_play[] = {
    0xFF, 0xFF,       // 帧头
    0x00,             // 舵机 ID（0xFE 代表选中所有舵机）
    0x07, 0x03, 0x1E, // 舵机运动指令
    0x00, 0x00,       // 舵机位置设置（十六进制 0-1024，低位在前）
    0x00, 0x00,       // 舵机运动速度（0x01 最小，0x64 最大，低位在前）
    0x00              // 校验位，除帧头外其余位全部相加，再使用 0xFF 减去低八位得到的值
};

/**
 * @brief 舵机初始化函数
 */
void GServo_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 使能时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    S485_IO_CLK_ENABLE();  // 使能串口IO时钟
    S485_DIR_CLK_ENABLE(); // 使能方向控制IO时钟

    // 配置 USART2 TX (PA2) 为推挽复用输出
    GPIO_InitStructure.GPIO_Pin = S485_TX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(S485_IO_PORT, &GPIO_InitStructure);

    // 配置 USART2 RX (PA3) 为浮空输入
    GPIO_InitStructure.GPIO_Pin = S485_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(S485_IO_PORT, &GPIO_InitStructure);

    // 配置 RS485 方向控制引脚 (PC4)
    GPIO_InitStructure.GPIO_Pin = S485_DIR_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(S485_DIR_PORT, &GPIO_InitStructure);

    // 初始化为接收模式
    GPIO_ResetBits(S485_DIR_PORT, S485_DIR_PIN);

    // 配置 USART2
    USART_InitStructure.USART_BaudRate = 1000000;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART2, &USART_InitStructure);

    // 配置 USART2 中断
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // 使能 USART2 接收中断
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

    // 使能 USART2
    USART_Cmd(USART2, ENABLE);
}

/**
 * @brief 串口 2 中断服务函数（修改为接收数据）
 */
void Gservo_IRQHandler(void)
{
    // 读取接收到的数据字节
    uint8_t rxdata;
    // 从串口寄存器中读取接收到的数据
    rxdata = USART_ReceiveData(USART2);

    // 数据回显功能
    S485_Dir_Control(1); // 切换到发送模式
    USART_SendData(USART2, rxdata);
    Delay_ms(1);
    S485_Dir_Control(0); // 切换回接收模式

    // 将数据存储到接收缓冲区
    if (rx_count < RX_BUFFER_SIZE)
    {
        rx_buffer[rx_count++] = rxdata;

        // 检测到帧头 0xFF 0xFF 时，标记开始接收
        if (rx_count >= 2 && rx_buffer[0] == 0xFF && rx_buffer[1] == 0xFF)
        {
            // 如果接收到的数据长度等于协议规定的长度，设置接收完成标志
            if (rx_count >= 6) // 最小数据包长度
            {
                uint8_t length = rx_buffer[3];
                if (rx_count >= (4 + length + 1)) // 头部 2 + ID1 + Length1 + Data + Checksum1
                {
                    rx_flag = 1; // 接收完成
                }
            }
        }
    }
}

/**
 * @brief 发送数据函数
 * @param data: 要发送的数据指针
 * @param length: 数据长度
 */
void S485_Senddata(uint8_t *data, uint8_t length)
{
    //    for(uint8_t i = 0; i < length; i++)
    //    {
    //        // 发送数据
    //        USART_SendData(USART2, data[i]);
    //        // 判断上一次发送是否完成
    //        while(USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET);
    //    }
    // 发送数据
    for (uint8_t i = 0; i < length; i++)
    {
        USART_SendData(USART2, data[i]);
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
            ;
    }
    // 等待最后一个字节发送完成
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
        ;
}

/**
 * @brief 接收一个字节的数据（阻塞式）
 * @param 无
 * @return 接收到的字节
 */
uint8_t S485_ReceiveByte(void)
{
    while (!(USART2->SR & USART_FLAG_RXNE))
        ; // 等待接收完成
    return USART_ReceiveData(USART2);
}

/**
 * @brief 计算校验位
 * @param data: 数据指针
 * @return 校验位
 */
uint8_t Parity_Check(uint8_t *data)
{
    // 定义一个u8类型的变量，这样累加所有数据后只会保留低八位
    uint8_t checksum = 0;
    for (uint8_t i = 2; i < 10; i++)
    {
        checksum += data[i];
    }
    // 对checksum按位取反相当于用FF减去checksum
    checksum = ~checksum;
    return checksum;
}

/**
 * @brief 设置舵机位置
 * @param id: 舵机ID
 * @param position: 舵机位置 (0-1024)
 * @param speed: 舵机速度 (0x01-0x64)
 */
void Set_Servo_position(uint8_t id, uint16_t position, uint8_t speed)
{
    // 提取舵机位置的低八位和高八位
    uint8_t position_L = (uint8_t)position;
    uint8_t position_H = (uint8_t)(position >> 8);

    // 设置舵机id
    Servo_play[2] = id;

    // 设置舵机位置
    Servo_play[6] = position_L;
    Servo_play[7] = position_H;

    // 设置舵机速度
    // 注意：根据 DYNAMIXEL 协议 1.0，速度参数应为 2 字节（16 位）
    // 但当前实现仅使用低 8 位，高 8 位固定为 0x00
    // 如果实际使用中舵机能正常工作，则无需修改此部分
    Servo_play[8] = speed;
    Servo_play[9] = 0x00; // 速度高字节，暂时保持为 0

    // 计算校验位
    Servo_play[10] = Parity_Check(Servo_play);

    // 切换到发送模式
    S485_Dir_Control(1);

    // 发送数据
    for (uint8_t i = 0; i < 11; i++)
    {
        USART_SendData(USART2, Servo_play[i]);
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
            ;
    }
    // 等待最后一个字节发送完成
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
        ;

    // 切回接收模式
    S485_Dir_Control(0);

    ////    // 拉高485方向引脚，切换到发送模式
    ////    S485_Dir_Control(1);
    ////
    ////    // 发送舵机运动指令
    ////    S485_Senddata(Servo_play, 11);
    ////
    ////    for(uint8_t i = 0; i < 255; i++)
    ////    {
    ////        __NOP();
    ////    }
    ////
    ////    // 发送完毕，切换回接收模式
    ////    S485_Dir_Control(0);
}

// ========== 以下是原有函数的 STM32 适配版本 ==========

/**
 * @brief 读取舵机位置
 * @param id: 舵机ID
 * @return 舵机角度值
 */
uint16_t SERVO_ReadPosition(uint8_t id)
{
    // 构建读取指令
    uint8_t readCmd[8] = {
        0xFF, 0xFF, // 帧头
        0x00,       // id
        0x04,
        0x02,
        0x24,
        0x02,
        0x00 // 校验位
    };

    readCmd[2] = id;
    uint8_t checkSum = 0;
    for (int i = 2; i < 7; i++)
    {
        checkSum += readCmd[i];
    }
    readCmd[7] = 0xFF - (checkSum & 0xFF);

    // 发送指令
    S485_Dir_Control(1);
    S485_Senddata(readCmd, 8);
    S485_Dir_Control(0);

    // 接收响应（等待8字节数据）
    uint8_t response[8];
    int bytesRead = 0;
    uint32_t timeout = 500000; // 超时计数器

    while (bytesRead < 8 && timeout > 0)
    {
        if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == SET)
        {
            response[bytesRead++] = USART_ReceiveData(USART2);
        }
        timeout--;
    }

    if (timeout == 0)
    {
        response[6] = 0;
        response[5] = 0;
    }

    // 提取位置值（低字节在前）
    uint16_t position = (response[6] << 8) | response[5];
    return position;
}

/**
 * @brief 设置舵机位置（角度模式）
 * @param id: 舵机ID
 * @param angle: 设置的角度（0-320°）
 */
void SERVO_SetPosition_angle(uint8_t id, uint16_t angle)
{
    uint8_t txBuffer[11] = {
        0xFF, 0xFF, // 帧头
        0x00,       // ID号
        0x07,       // 除帧头和校验的总长度
        0x03,       // 写入指令
        0x1E,       // 旋转状态地址
        0x00, 0x00, // 对应角度，两个字节，低字节在前
        0x64, 0x00, // 旋转速度，两个字节，低字节在前
        0x00        // 校验和
    };

    txBuffer[2] = id; // ID
    txBuffer[6] = (uint8_t)(angle & 0xFF);
    txBuffer[7] = (uint8_t)((angle >> 8) & 0xFF);

    // 计算校验和
    uint8_t checkSum = 0;
    for (uint8_t i = 2; i < 10; i++)
    {
        checkSum += txBuffer[i];
    }
    txBuffer[10] = 0xFF - (checkSum & 0xFF);

    // 串口发送数据
    S485_Dir_Control(1);
    S485_Senddata(txBuffer, 11);
    Delay_ms(10);
    S485_Dir_Control(0);
}

/**
 * @brief 设置舵机位置
 * @param id: 舵机ID
 * @param position_L: 位置低字节
 * @param position_H: 位置高字节
 */
void SERVO_SetPosition(uint8_t id, uint16_t position_L, uint16_t position_H)
{
    uint8_t txBuffer[11] = {
        0xFF, 0xFF, // 帧头
        0x00,       // ID号
        0x07,       // 除帧头和校验的总长度
        0x03,       // 写入指令
        0x1E,       // 旋转状态地址
        0x00, 0x00, // 对应角度，两个字节，低字节在前
        0x64, 0x00, // 旋转速度，两个字节，低字节在前
        0x00        // 校验和
    };

    // 填充发送缓冲区
    txBuffer[2] = id; // ID
    txBuffer[6] = position_L;
    txBuffer[7] = position_H;

    // 计算校验和
    uint8_t checkSum = 0;
    for (uint8_t i = 2; i < 10; i++)
    {
        checkSum += txBuffer[i];
    }
    txBuffer[10] = 0xFF - (checkSum & 0xFF);

    // 串口发送数据
    S485_Dir_Control(1);
    S485_Senddata(txBuffer, 11);
    Delay_ms(10);
    S485_Dir_Control(0);
}

/**
 * @brief 读取舵机ID
 * @return 舵机ID，如果读取失败返回0xFF
 */
uint8_t SERVO_ReadID(void)
{
    uint8_t txBuffer[6] = {
        0xFF, 0xFF, // 帧头
        0xFE,       // 广播ID，用于读取所有舵机
        0x02,       // 数据长度
        0x01,       // 读取指令
        0xFB        // 校验和
    };

    // 清空接收缓冲区
    rx_count = 0;
    rx_flag = 0;

    // 发送读取ID指令
    S485_Dir_Control(1);
    S485_Senddata(txBuffer, 6);
    Delay_ms(10);
    S485_Dir_Control(0);

    // 等待响应数据
    uint32_t timeout = 100000; // 超时计数器
    while (rx_flag == 0 && timeout > 0)
    {
        timeout--;
    }

    // 检查是否接收到有效响应
    if (rx_flag == 1 && rx_count >= 6)
    {
        // 验证帧头
        if (rx_buffer[0] == 0xFF && rx_buffer[1] == 0xFF)
        {
            // 返回舵机ID
            return rx_buffer[2];
        }
    }

    return 0xFF; // 读取失败
}

/**
 * @brief 修改舵机ID
 * @param id: 舵机当前ID
 * @param tar_id: 要设置的ID
 */
void SERVO_SetID(uint8_t id, uint8_t tar_id)
{
    uint8_t txBuffer[8] = {
        0xFF, 0xFF, // 帧头
        0x00,       // 当前ID号
        0x04,
        0x03,
        0x03,
        0x00, // 目标ID号
        0x00  // 校验和
    };

    // 填充发送缓冲区
    txBuffer[2] = id;     // 当前ID
    txBuffer[6] = tar_id; // 目标ID

    // 计算校验和
    uint8_t checkSum = 0;
    for (uint8_t i = 2; i < 7; i++)
    { // 修正循环范围
        checkSum += txBuffer[i];
    }
    txBuffer[7] = 0xFF - (checkSum & 0xFF);

    // 串口发送数据
    S485_Dir_Control(1);
    S485_Senddata(txBuffer, 8);
    Delay_ms(10);
    S485_Dir_Control(0);
}

/**
 * @brief 扫描舵机ID（扫描ID范围1-253）
 * @param found_ids: 存储找到的舵机ID数组
 * @param max_count: 数组最大容量
 * @return 找到的舵机数量
 */
uint8_t SERVO_ScanIDs(uint8_t *found_ids, uint8_t max_count)
{
    uint8_t count = 0;
    uint8_t ping_cmd[6];

    // 构建PING指令模板
    ping_cmd[0] = 0xFF; // 帧头
    ping_cmd[1] = 0xFF; // 帧头
    ping_cmd[2] = 0x00; // ID (将在循环中设置)
    ping_cmd[3] = 0x02; // 数据长度
    ping_cmd[4] = 0x01; // PING指令
    ping_cmd[5] = 0x00; // 校验和 (将在循环中计算)

    // 扫描ID范围 1-253 (0xFE是广播地址，0xFF无效)
    for (uint8_t id = 1; id <= 253 && count < max_count; id++)
    {
        // 设置当前扫描的ID
        ping_cmd[2] = id;

        // 计算校验和
        uint8_t checksum = 0;
        for (uint8_t i = 2; i < 5; i++)
        {
            checksum += ping_cmd[i];
        }
        ping_cmd[5] = 0xFF - (checksum & 0xFF);

        // 清空接收缓冲区
        rx_count = 0;
        rx_flag = 0;

        // 发送PING指令
        S485_Dir_Control(1);
        S485_Senddata(ping_cmd, 6);
        Delay_ms(5);
        S485_Dir_Control(0);

        // 等待响应 (较短的超时时间)
        uint32_t timeout = 50000;
        while (rx_flag == 0 && timeout > 0)
        {
            timeout--;
        }

        // 检查是否收到响应
        if (rx_flag == 1 && rx_count >= 6)
        {
            // 验证响应帧头和ID
            if (rx_buffer[0] == 0xFF && rx_buffer[1] == 0xFF && rx_buffer[2] == id)
            {
                found_ids[count] = id;
                count++;
            }
        }

        // 短暂延时，避免总线冲突
        Delay_ms(10);
    }

    return count;
}

/**
 * @brief 读取指定ID舵机的当前位置
 * @param id: 舵机ID
 * @return 舵机位置值，读取失败返回0xFFFF
 */
uint16_t SERVO_ReadPosition_Enhanced(uint8_t id)
{
    uint8_t readCmd[8] = {
        0xFF, 0xFF, // 帧头
        0x00,       // id (将设置为传入的id)
        0x04,       // 数据长度
        0x02,       // 读取指令
        0x24,       // 位置寄存器地址
        0x02,       // 读取字节数
        0x00        // 校验位
    };

    readCmd[2] = id;
    uint8_t checkSum = 0;
    for (int i = 2; i < 7; i++)
    {
        checkSum += readCmd[i];
    }
    readCmd[7] = 0xFF - (checkSum & 0xFF);

    // 清空接收缓冲区
    rx_count = 0;
    rx_flag = 0;

    // 发送指令
    S485_Dir_Control(1);
    S485_Senddata(readCmd, 8);
    Delay_ms(10);
    S485_Dir_Control(0);

    // 等待响应
    uint32_t timeout = 100000;
    while (rx_flag == 0 && timeout > 0)
    {
        timeout--;
    }

    if (rx_flag == 1 && rx_count >= 8)
    {
        // 验证响应帧头和ID
        if (rx_buffer[0] == 0xFF && rx_buffer[1] == 0xFF && rx_buffer[2] == id)
        {
            // 提取位置值（低字节在前）
            uint16_t position = (rx_buffer[6] << 8) | rx_buffer[5];
            return position;
        }
    }

    return 0xFFFF; // 读取失败
}
