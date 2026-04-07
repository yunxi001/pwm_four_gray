#include "rs485_camera.h"
#include "servo_485.h"
/* 环形缓冲区 */
static uint8_t rxBuf[RX_BUF_SIZE];
static volatile uint16_t rxHead = 0, rxTail = 0;

/* 解析状态机变量 */
static enum {
    WAIT_FF1,
    WAIT_FF2,
    WAIT_A1,
    WAIT_VER,
    WAIT_LEN,
    WAIT_PAYLOAD
} state = WAIT_FF1;

static uint8_t rxFrame[32];          // 当前正在组装的帧
static uint8_t rxFrameIdx = 0;       // 当前帧的索引
static uint8_t payloadCnt = 0;        // 长度字段之后已接收的字节数
static uint8_t frameReady = 0;        // 完整帧就绪标志
static uint16_t frameLen = 0;          // 完整帧长度

extern UART_HandleTypeDef huart3;     // 您的UART3句柄

static uint8_t IsValidFrameId(uint8_t id)
{
    return (id >= RS485_FRAME_ID_MIN) && (id <= RS485_FRAME_ID_MAX);
}

static void StartFrameWithFF(void)
{
    state = WAIT_FF2;
    rxFrame[0] = FRAME_HEADER1;
    rxFrameIdx = 1;
    payloadCnt = 0;
}

static void ResetFrameParser(void)
{
    state = WAIT_FF1;
    rxFrameIdx = 0;
    payloadCnt = 0;
}

/* 初始化接收 */
void RS485_Rx_Init(void)
{
    rxHead = rxTail = 0;
    frameReady = 0;
    frameLen = 0;
    ResetFrameParser();

    // 启动第一个字节的接收（使能RXNE中断）
    HAL_UART_Receive_IT(&huart3, &rxBuf[rxHead], 1);
}

/* 从环形缓冲区取一个字节，返回1成功，0失败 */
static uint8_t GetByteFromRxBuf(uint8_t *pData)
{
    if (rxTail == rxHead) return 0;
    *pData = rxBuf[rxTail];
    rxTail = (rxTail + 1) % RX_BUF_SIZE;
    return 1;
}

/* 解析接收数据（在主循环中调用） */
void RS485_Rx_Process(void)
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
                // 长度位只做合法性校验，长度后始终只跟 3 个字节。
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
uint8_t RS485_GetFrame(uint8_t *buffer, uint16_t *length)
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

uint8_t RS485_WaitFrameById(uint8_t expectedId, uint8_t *buffer, uint16_t *length, uint32_t timeoutMs)
{
    uint32_t startTick = HAL_GetTick();
    uint8_t rxFrameLocal[32];
    uint16_t rxLenLocal;

    while ((HAL_GetTick() - startTick) < timeoutMs)
    {
        RS485_Rx_Process();

        if (RS485_GetFrame(rxFrameLocal, &rxLenLocal))
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

/* HAL库接收完成回调函数（在中断中调用） */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART3)
    {
        // 刚接收的字节已存入 rxBuf[rxHead-1]（因为启动接收时指定了该地址）
        // 更新 rxHead 指向下一个空闲位置
        uint16_t nextHead = (rxHead + 1) % RX_BUF_SIZE;
        if (nextHead != rxTail)  // 缓冲区未满
        {
            rxHead = nextHead;
        }
        else
        {
            // 缓冲区满，丢弃该字节（可选：设置溢出标志）
            // 为了简单，我们忽略溢出，不更新rxHead
        }

        // 继续接收下一个字节（再次使能中断）
        HAL_UART_Receive_IT(huart, &rxBuf[rxHead], 1);
    }
}
static uint8_t CalcChecksum(uint8_t *pData, uint8_t len)
{
    uint16_t sum = 0;
    for (uint8_t i = 0; i < len; i++) {
        sum += pData[i];
    }
    return (uint8_t)(~sum);
}
void SendFrame(uint8_t id,uint8_t cmd, uint8_t mode, uint8_t *data, uint8_t len)
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
    // 校验和：从A1到最后一个数据
    uint8_t checksum = CalcChecksum(&txBuf[2], idx - 2);  // idx-2 是从A1开始的字节数
    txBuf[idx++] = checksum;
		
        // 按实际组帧长度发送，避免把尾部无效字节一并发出
		S485_SendData(txBuf, idx);

    
}
//// 设备控制（写操作）
//void Camera_DeviceControl(uint8_t ctrl_item, uint8_t param)
//{
//	uint8_t rx_frame[16];
//    uint16_t rx_len;
//    uint8_t tx_data[2] = {ctrl_item, param};
//  SendFrame(CMD_READ,MODE_SPECIFY,tx_data,2);
//		
//		if (RS485_GetFrame(rx_frame, &rx_len))
//		{ 
//			for(int i=0;i<rx_len;i++)
//						printf("%X ",rx_frame[i]);
//			if( CalcChecksum(&rx_frame[2],rx_len-3)==rx_frame[rx_len-1])
//			{
//				if(rx_frame[rx_len-2]) printf("detected!!\n");
//				else printf("not detected!!\n");

//			}

//		}
//}
// 设备重启（写操作）
uint8_t Camera_Reset(uint8_t id)
{
	uint8_t rx_frame[16];
    uint16_t rx_len;
	
    uint8_t tx_data = CTRL_REBOOT;
    SendFrame(id,CMD_WRITE,MODE_DEVICE_CTRL,&tx_data,1);
        if (RS485_WaitFrameById(id, rx_frame, &rx_len, CAMERA_RX_TIMEOUT_MS))
        { 
            return(rx_frame[rx_len-2]);
        }
			return 0;
   // return CAMERA_OK;
}


// 模式切换
uint8_t Camera_SetMode(uint8_t id, uint8_t func)
{
		uint8_t rx_frame[16];
    uint16_t rx_len;
    uint8_t tx_data = func;
  
        SendFrame(id,CMD_WRITE,MODE_SWITCH,&tx_data,1);
    if (RS485_WaitFrameById(id, rx_frame, &rx_len, CAMERA_RX_TIMEOUT_MS))
		{ 			
            for(int i=0;i<rx_len;i++)
                    printf("%X ",rx_frame[i]);
                return(rx_frame[rx_len-2]);
		
			 
		}
			return 0;
}

// 设备控制
uint8_t Camera_DeviceCTRL(uint8_t id, uint8_t dat1,uint8_t dat2)
{
		uint8_t rx_frame[16];
    uint16_t rx_len;
    uint8_t tx_data[2] ={dat1,dat2};
  
        SendFrame(id,CMD_WRITE,MODE_DEVICE_CTRL,tx_data,2);
    if (RS485_WaitFrameById(id, rx_frame, &rx_len, CAMERA_RX_TIMEOUT_MS))
		{ 			
            for(int i=0;i<rx_len;i++)
                    printf("%X ",rx_frame[i]);
                return(rx_frame[rx_len-2]);
		
			 
		}
			return 0;
}
// 读指定颜色识别
uint8_t Camera_ReadColorSpec(uint8_t id, uint8_t color)
{
		uint8_t rx_frame[16];
    uint16_t rx_len;
    uint8_t tx_data[2] = {FUNC_COLOR, color};
        SendFrame(id,CMD_READ,MODE_SPECIFY,tx_data,2);
   if (RS485_WaitFrameById(id, rx_frame, &rx_len, CAMERA_RX_TIMEOUT_MS))
		{ 			
            // for(int i=0;i<rx_len;i++)
			// 		printf("%X ",rx_frame[i]);
            //printf("\n ");
			return(rx_frame[rx_len-2]);
			 
		}
			return 0;
}

// 读非指定颜色识别
uint8_t Camera_ReadColorNonSpec(uint8_t id, uint8_t *detected_color)
{
		uint8_t rx_frame[16];
    uint16_t rx_len;
    uint8_t tx_data = FUNC_COLOR;
   
        SendFrame(id,CMD_READ,MODE_NON_SPECIFY,&tx_data,1);
   if (RS485_WaitFrameById(id, rx_frame, &rx_len, CAMERA_RX_TIMEOUT_MS))
		{ 		
            for(int i=0;i<rx_len;i++)
					printf("%X ",rx_frame[i]);	
            {
                
					switch(rx_frame[rx_len-2])
					{
						case COLOR_RED    :   printf("RED   \n");break;  
						case COLOR_GREEN  :   printf("GREEN \n");break; 
						case COLOR_BLUE   :   printf("BLUE  \n");break; 
						case COLOR_YELLOW :   printf("YELLOW\n");break; 
						case COLOR_GRAY   :   printf("GRAY  \n");break; 
						case COLOR_PURPLE :   printf("PURPLE\n");break; 
						case COLOR_WHITE  :   printf("WHITE \n");break; 
						case COLOR_BLACK  :		printf("BLACK \n");break; 
					}
					return rx_frame[rx_len-2];
				}
			 
		}
			return 0;
}

// 读指定人脸识别
uint8_t Camera_ReadFaceSpec(uint8_t id, uint8_t face_id)
{	
		uint8_t rx_frame[16];
    uint16_t rx_len;
     uint8_t tx_data[2] = {FUNC_FACE, face_id};
   
        SendFrame(id,CMD_READ,MODE_SPECIFY,tx_data,2);
   if (RS485_WaitFrameById(id, rx_frame, &rx_len, CAMERA_RX_TIMEOUT_MS))
		{ 			
			return(rx_frame[rx_len-2]);
			 
		}
			return 0;
}

// 读非指定人脸识别
uint8_t Camera_ReadFaceNonSpec(uint8_t id, uint8_t *detected_id)
{
   uint8_t rx_frame[16];
    uint16_t rx_len;
     uint8_t tx_data = FUNC_FACE;
   
        SendFrame(id,CMD_READ,MODE_NON_SPECIFY,&tx_data,1);
   if (RS485_WaitFrameById(id, rx_frame, &rx_len, CAMERA_RX_TIMEOUT_MS))
		{ 			
			return(rx_frame[rx_len-2]);
			 
		}
			return 0;	
}

// 读指定数字识别
uint8_t Camera_ReadNumberSpec(uint8_t id, uint8_t num_param)
{		
		uint8_t rx_frame[16];
    uint16_t rx_len;
    uint8_t tx_data[2] = {FUNC_NUMBER, num_param};
   
        SendFrame(id,CMD_READ,MODE_SPECIFY,tx_data,2);
   if (RS485_WaitFrameById(id, rx_frame, &rx_len, CAMERA_RX_TIMEOUT_MS))
		{ 			
			return(rx_frame[rx_len-2]);
			 
		}
			return 0;
}

// 读非指定数字识别
uint8_t Camera_ReadNumberNonSpec(uint8_t id, uint8_t *number)
{
		uint8_t rx_frame[16];
    uint16_t rx_len;
    uint8_t tx_data = FUNC_NUMBER;
   
        SendFrame(id,CMD_READ,MODE_NON_SPECIFY,&tx_data,1);
   if (RS485_WaitFrameById(id, rx_frame, &rx_len, CAMERA_RX_TIMEOUT_MS))
		{ 			
            {
					for(int i=0;i<rx_len;i++)
						printf("%X ",rx_frame[i]);
				switch(rx_frame[rx_len-2])
					{
						case NUMBER_0  :   printf("NUMBER_0 \n");break;  
						case NUMBER_1  :   printf("NUMBER_1 \n");break; 
						case NUMBER_2  :   printf("NUMBER_2 \n");break; 
						case NUMBER_3  :   printf("NUMBER_3 \n");break; 
						case NUMBER_4  :   printf("NUMBER_4 \n");break; 
						case NUMBER_5  :   printf("NUMBER_5 \n");break; 
						case NUMBER_6  :   printf("NUMBER_6 \n");break; 
						case NUMBER_7  :	 printf("NUMBER_7 \n");break; 
					  case NUMBER_8  :	 printf("NUMBER_8 \n");break;
						case NUMBER_9  :   printf("NUMBER_9 \n");break;					
					}
	  			return  rx_frame[rx_len-2];
				}
		}
			return 0;
}

// 读指定标签识别
uint8_t Camera_ReadLabelSpec(uint8_t id, uint8_t label)
{
		uint8_t rx_frame[16];
    uint16_t rx_len;
    uint8_t tx_data[2] = {FUNC_LABEL, label};
   
        SendFrame(id,CMD_READ,MODE_SPECIFY,tx_data,2);
   if (RS485_WaitFrameById(id, rx_frame, &rx_len, CAMERA_RX_TIMEOUT_MS))
		{ 
				for(int i=0;i<rx_len;i++)
						printf("%X ",rx_frame[i]);			
			return(rx_frame[rx_len-2]);
			 
		}
			return 0;
}

// 读非指定标签识别
uint8_t Camera_ReadLabelNonSpec(uint8_t id, uint8_t *label)
{
    uint8_t tx_data = FUNC_LABEL;
    uint8_t rx_frame[16];
    uint16_t rx_len;
   
        SendFrame(id,CMD_READ,MODE_NON_SPECIFY,&tx_data,1);
   if (RS485_WaitFrameById(id, rx_frame, &rx_len, CAMERA_RX_TIMEOUT_MS))
		{ 			
			return(rx_frame[rx_len-2]);
			 
		}
			return 0;
}

/**
 * @brief 读取二维码数据
 * @param qr_buf   : 用于存放二维码数据的缓冲区（由调用者提供）
 * @param buf_len  : 输入时为缓冲区最大长度，输出时为实际接收到的数据长度
 * @return 状态码（CAMERA_OK 表示成功，其他为错误码）
 */
uint8_t Camera_ReadQR_Code(uint8_t id, uint8_t *qr_buf, uint8_t *buf_len)
{
 	
		uint8_t tx_data = FUNC_QRCODE;
    uint8_t rx_frame[16];
    uint16_t rx_len;
   
        SendFrame(id,CMD_READ,MODE_NON_SPECIFY,&tx_data,1);
   if (RS485_WaitFrameById(id, rx_frame, &rx_len, CAMERA_RX_TIMEOUT_MS))
		{ 			
			return(rx_frame[rx_len-2]);
			 
		}
			return 0;
}
