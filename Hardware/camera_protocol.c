#include "camera_protocol.h"
#include "Serial.h"
#include "Delay.h"
#include "stdio.h"

/* 外部RS485方向控制函数 */
extern void S485_Dir_Control(uint8_t dir);  // dir: 0=接收, 1=发送

/* 环形接收缓冲区 */
static uint8_t rxBuf[RX_BUF_SIZE];
static volatile uint16_t rxHead = 0, rxTail = 0;

/* 当前正在接收的帧 */
static uint8_t rxFrame[32];
static uint8_t rxFrameIdx = 0;
static uint8_t frameReady = 0;
static uint16_t frameLen = 0;

/* 接收状态机枚举 */
static enum {
    WAIT_FF1,
    WAIT_FF2,
    WAIT_A1,
    WAIT_VER,
    WAIT_LEN,
    WAIT_PAYLOAD
} state = WAIT_FF1;

static uint8_t payloadCnt = 0;

/* 校验和计算：从A1开始到最后一个数据，累加后取反 */
static uint8_t CalcChecksum(uint8_t *pData, uint8_t len)
{
    uint16_t sum = 0;
    for (uint8_t i = 0; i < len; i++) {
        sum += pData[i];
    }
    return (uint8_t)(~sum);
}

/* 判断ID是否有效 */
static uint8_t IsValidFrameId(uint8_t id)
{
    return (id >= RS485_FRAME_ID_MIN) && (id <= RS485_FRAME_ID_MAX);
}

/* 重置帧解析器 */
static void ResetFrameParser(void)
{
    state = WAIT_FF1;
    rxFrameIdx = 0;
    payloadCnt = 0;
}

/* 开始新帧（收到第一个FF） */
static void StartFrameWithFF(void)
{
    state = WAIT_FF2;
    rxFrame[0] = FRAME_HEADER1;
    rxFrameIdx = 1;
    payloadCnt = 0;
}

/* 发送一帧数据 */
static void SendFrame(uint8_t id, uint8_t cmd, uint8_t mode, uint8_t *data, uint8_t len)
{
    uint8_t txBuf[TX_BUF_SIZE];
    uint8_t idx = 0;
    
    txBuf[idx++] = FRAME_HEADER1;
    txBuf[idx++] = FRAME_HEADER2;
    txBuf[idx++] = FRAME_HEADER3;
    txBuf[idx++] = id;
    txBuf[idx++] = cmd;
    txBuf[idx++] = mode;
    txBuf[idx++] = len;
    for (uint8_t i = 0; i < len; i++) {
        txBuf[idx++] = data[i];
    }
    // 校验和：从A1到最后数据
    uint8_t checksum = CalcChecksum(&txBuf[2], idx - 2);
    txBuf[idx++] = checksum;
    
    // 切换到发送模式
    S485_Dir_Control(1);
    
    // 发送数据
    for (uint8_t i = 0; i < idx; i++) {
        USART_SendData(USART2, txBuf[i]);
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    }
    // 等待最后一个字节发送完成
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
    
    // 切回接收模式
    S485_Dir_Control(0);
}

/* 相机中断处理函数：由Serial.c的USART2_IRQHandler调用 */
void Camera_IRQHandler(void)
{
    uint8_t data = USART_ReceiveData(USART2);
    // 存入环形缓冲区
    uint16_t nextHead = (rxHead + 1) % RX_BUF_SIZE;
    if (nextHead != rxTail) {   // 未满
        rxBuf[rxHead] = data;
        rxHead = nextHead;
    }
}

/* 从环形缓冲区取一个字节，返回1成功，0失败 */
static uint8_t GetByteFromRxBuf(uint8_t *pData)
{
    if (rxTail == rxHead) return 0;
    *pData = rxBuf[rxTail];
    rxTail = (rxTail + 1) % RX_BUF_SIZE;
    return 1;
}

/* 解析接收数据（在循环中调用） */
void Camera_Process(void)
{
    uint8_t byte;
    
    while (GetByteFromRxBuf(&byte))
    {
        switch (state)
        {
            case WAIT_FF1:
                if (byte == FRAME_HEADER1)
                {
                    StartFrameWithFF();
                }
                break;

            case WAIT_FF2:
                if (byte == FRAME_HEADER2)
                {
                    state = WAIT_A1;
                    rxFrame[rxFrameIdx++] = byte;
                }
                else
                {
                    if (byte == FRAME_HEADER1)
                    {
                        StartFrameWithFF();
                    }
                    else
                    {
                        ResetFrameParser();
                    }
                }
                break;

            case WAIT_A1:
                if (byte == FRAME_HEADER3)
                {
                    state = WAIT_VER;
                    rxFrame[rxFrameIdx++] = byte;
                }
                else
                {
                    ResetFrameParser();
                }
                break;

            case WAIT_VER:
                if (IsValidFrameId(byte))
                {
                    state = WAIT_LEN;
                    rxFrame[rxFrameIdx++] = byte;
                }
                else
                {
                    ResetFrameParser();
                }
                break;

            case WAIT_LEN:
                // 回包格式固定为：FF FF A1 ID LEN DATA1 DATA2 CHECK
                // 长度位只包含后续（数据+校验），最少后续只有 3 个字节
                rxFrame[rxFrameIdx++] = byte;
                if (byte >= 3 && byte <= 5)
                {
                    payloadCnt = 0;
                    state = WAIT_PAYLOAD;
                }
                else
                {
                    ResetFrameParser();
                }
                break;

            case WAIT_PAYLOAD:
                rxFrame[rxFrameIdx++] = byte;
                payloadCnt++;

                if (payloadCnt >= 3)
                {
                    if (CalcChecksum(&rxFrame[2], rxFrameIdx - 3) == rxFrame[rxFrameIdx - 1])
                    {
                        frameReady = 1;
                        frameLen = rxFrameIdx;
                        ResetFrameParser();
                        return;
                    }

                    ResetFrameParser();
                }
                break;
        }
    }
}

/* 获取已接收的完整帧，返回1成功，0无帧 */
uint8_t Camera_GetFrame(uint8_t *buffer, uint16_t *length)
{
    if (frameReady)
    {
        memcpy(buffer, rxFrame, frameLen);
        *length = frameLen;

        // 清除标志，准备接收下一帧
        frameReady = 0;
        ResetFrameParser();
        return 1;
    }
    return 0;
}

/* 等待指定ID的帧响应 */
uint8_t Camera_WaitFrameById(uint8_t expectedId, uint8_t *buffer, uint16_t *length, uint32_t timeoutMs)
{
    uint32_t startTick = sys_tick_counter;
    uint8_t rxFrameLocal[32];
    uint16_t rxLenLocal;

    while ((sys_tick_counter - startTick) < timeoutMs)
    {
        Camera_Process();

        if (Camera_GetFrame(rxFrameLocal, &rxLenLocal))
        {
            if ((rxLenLocal == 8) &&
                (rxFrameLocal[3] == expectedId) &&
                (CalcChecksum(&rxFrameLocal[2], rxLenLocal - 3) == rxFrameLocal[rxLenLocal - 1]))
            {
                memcpy(buffer, rxFrameLocal, rxLenLocal);
                *length = rxLenLocal;
                return 1;
            }
        }
    }

    return 0;
}

/* 初始化 */
void Camera_Init(void)
{
    rxHead = rxTail = 0;
    frameReady = 0;
    frameLen = 0;
    ResetFrameParser();
    
    // 确保串口已初始化并使能接收中断
    // USART2 初始化在外部完成，这里只做协议初始化
}

/* ========== 高层封装函数 ========== */

// 设备重启（写命令）
uint8_t Camera_Reset(uint8_t id)
{
    uint8_t rx_frame[16];
    uint16_t rx_len;
    
    uint8_t tx_data = CTRL_REBOOT;
    SendFrame(id, CMD_WRITE, MODE_DEVICE_CTRL, &tx_data, 1);
    
    if (Camera_WaitFrameById(id, rx_frame, &rx_len, CAMERA_RX_TIMEOUT_MS))
    { 
        return rx_frame[rx_len - 2];
    }
    return 0;
}

// 模式切换
uint8_t Camera_SetMode(uint8_t id, uint8_t func)
{
    uint8_t rx_frame[16];
    uint16_t rx_len;
    uint8_t tx_data = func;
    
    SendFrame(id, CMD_WRITE, MODE_SWITCH, &tx_data, 1);
    
//    if (Camera_WaitFrameById(id, rx_frame, &rx_len, CAMERA_RX_TIMEOUT_MS))
//    { 
//        return rx_frame[rx_len - 2];
//    }
    return 0;
}

// 设备控制
uint8_t Camera_DeviceControl(uint8_t id, uint8_t dat1, uint8_t dat2)
{
    uint8_t rx_frame[16];
    uint16_t rx_len;
    uint8_t tx_data[2] = {dat1, dat2};
    
    SendFrame(id, CMD_WRITE, MODE_DEVICE_CTRL, tx_data, 2);
    
    if (Camera_WaitFrameById(id, rx_frame, &rx_len, CAMERA_RX_TIMEOUT_MS))
    { 
        return rx_frame[rx_len - 2];
    }
    return 0;
}

// 读指定颜色识别
uint8_t Camera_ReadColorSpec(uint8_t id, uint8_t color)
{
    uint8_t rx_frame[16];
    uint16_t rx_len;
    uint8_t tx_data[2] = {FUNC_COLOR, color};
    
    SendFrame(id, CMD_READ, MODE_SPECIFY, tx_data, 2);
    
    if (Camera_WaitFrameById(id, rx_frame, &rx_len, CAMERA_RX_TIMEOUT_MS))
    { 
        return rx_frame[rx_len - 2];
    }
    return 0;
}

// 读非指定颜色识别
uint8_t Camera_ReadColorNonSpec(uint8_t id, uint8_t *detected_color)
{
    uint8_t rx_frame[16];
    uint16_t rx_len;
    uint8_t tx_data = FUNC_COLOR;
    
    SendFrame(id, CMD_READ, MODE_NON_SPECIFY, &tx_data, 1);
    
    if (Camera_WaitFrameById(id, rx_frame, &rx_len, CAMERA_RX_TIMEOUT_MS))
    { 
        *detected_color = rx_frame[rx_len - 2];
        
        // 可选：打印颜色名称
        switch(rx_frame[rx_len - 2])
        {
            case COLOR_RED:    printf("RED\n");    break;  
            case COLOR_GREEN:  printf("GREEN\n");  break; 
            case COLOR_BLUE:   printf("BLUE\n");   break; 
            case COLOR_YELLOW: printf("YELLOW\n"); break; 
            case COLOR_GRAY:   printf("GRAY\n");   break; 
            case COLOR_PURPLE: printf("PURPLE\n"); break; 
            case COLOR_WHITE:  printf("WHITE\n");  break; 
            case COLOR_BLACK:  printf("BLACK\n");  break; 
        }
        return rx_frame[rx_len - 2];
    }
    return 0;
}

// 读指定人脸识别
uint8_t Camera_ReadFaceSpec(uint8_t id, uint8_t face_id)
{
    uint8_t rx_frame[16];
    uint16_t rx_len;
    uint8_t tx_data[2] = {FUNC_FACE, face_id};
    
    SendFrame(id, CMD_READ, MODE_SPECIFY, tx_data, 2);
    
    if (Camera_WaitFrameById(id, rx_frame, &rx_len, CAMERA_RX_TIMEOUT_MS))
    { 
        return rx_frame[rx_len - 2];
    }
    return 0;
}

// 读非指定人脸识别
uint8_t Camera_ReadFaceNonSpec(uint8_t id, uint8_t *detected_id)
{
    uint8_t rx_frame[16];
    uint16_t rx_len;
    uint8_t tx_data = FUNC_FACE;
    
    SendFrame(id, CMD_READ, MODE_NON_SPECIFY, &tx_data, 1);
    
    if (Camera_WaitFrameById(id, rx_frame, &rx_len, CAMERA_RX_TIMEOUT_MS))
    { 
        *detected_id = rx_frame[rx_len - 2];
        return rx_frame[rx_len - 2];
    }
    return 0;
}

// 读指定数字识别
uint8_t Camera_ReadNumberSpec(uint8_t id, uint8_t num_param)
{
    uint8_t rx_frame[16];
    uint16_t rx_len;
    uint8_t tx_data[2] = {FUNC_NUMBER, num_param};
    
    SendFrame(id, CMD_READ, MODE_SPECIFY, tx_data, 2);
    
    if (Camera_WaitFrameById(id, rx_frame, &rx_len, CAMERA_RX_TIMEOUT_MS))
    { 
        return rx_frame[rx_len - 2];
    }
    return 0;
}

// 读非指定数字识别
uint8_t Camera_ReadNumberNonSpec(uint8_t id, uint8_t *number)
{
    uint8_t rx_frame[16];
    uint16_t rx_len;
    uint8_t tx_data = FUNC_NUMBER;
    
    SendFrame(id, CMD_READ, MODE_NON_SPECIFY, &tx_data, 1);
    
    if (Camera_WaitFrameById(id, rx_frame, &rx_len, CAMERA_RX_TIMEOUT_MS))
    { 
        *number = rx_frame[rx_len - 2];
        
        // 可选：打印数字
        switch(rx_frame[rx_len - 2])
        {
            case NUMBER_0: printf("NUMBER_0\n"); break;  
            case NUMBER_1: printf("NUMBER_1\n"); break; 
            case NUMBER_2: printf("NUMBER_2\n"); break; 
            case NUMBER_3: printf("NUMBER_3\n"); break; 
            case NUMBER_4: printf("NUMBER_4\n"); break; 
            case NUMBER_5: printf("NUMBER_5\n"); break; 
            case NUMBER_6: printf("NUMBER_6\n"); break; 
            case NUMBER_7: printf("NUMBER_7\n"); break; 
            case NUMBER_8: printf("NUMBER_8\n"); break;
            case NUMBER_9: printf("NUMBER_9\n"); break;
        }
        return rx_frame[rx_len - 2];
    }
    return 0;
}

// 读指定标签识别
uint8_t Camera_ReadLabelSpec(uint8_t id, uint8_t label)
{
    uint8_t rx_frame[16];
    uint16_t rx_len;
    uint8_t tx_data[2] = {FUNC_LABEL, label};
    
    SendFrame(id, CMD_READ, MODE_SPECIFY, tx_data, 2);
    
    if (Camera_WaitFrameById(id, rx_frame, &rx_len, CAMERA_RX_TIMEOUT_MS))
    { 
        return rx_frame[rx_len - 2];
    }
    return 0;
}

// 读非指定标签识别
uint8_t Camera_ReadLabelNonSpec(uint8_t id, uint8_t *label)
{
    uint8_t tx_data = FUNC_LABEL;
    uint8_t rx_frame[16];
    uint16_t rx_len;
    
    SendFrame(id, CMD_READ, MODE_NON_SPECIFY, &tx_data, 1);
    
    if (Camera_WaitFrameById(id, rx_frame, &rx_len, CAMERA_RX_TIMEOUT_MS))
    { 
        *label = rx_frame[rx_len - 2];
        return rx_frame[rx_len - 2];
    }
    return 0;
}

/**
 * @brief 读取二维码数据
 * @param id       : 相机设备ID
 * @param qr_buf   : 用于存储二维码数据的缓冲区（由调用者提供）
 * @param buf_len  : 输入时为缓冲区最大长度，输出时为实际接收到的数据长度
 * @return 状态码（CAMERA_OK 表示成功，其他为错误码）
 */
uint8_t Camera_ReadQR_Code(uint8_t id, uint8_t *qr_buf, uint8_t *buf_len)
{
    uint8_t tx_data = FUNC_QRCODE;
    uint8_t rx_frame[16];
    uint16_t rx_len;
    
    SendFrame(id, CMD_READ, MODE_NON_SPECIFY, &tx_data, 1);
    
    if (Camera_WaitFrameById(id, rx_frame, &rx_len, CAMERA_RX_TIMEOUT_MS))
    { 
        return rx_frame[rx_len - 2];
    }
    return 0;
}
