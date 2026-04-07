#ifndef __RS485_RECEIVER_H
#define __RS485_RECEIVER_H

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdint.h>

/* 环形缓冲区大小（根据实际数据量调整） */
#define RX_BUF_SIZE         128
#define TX_BUF_SIZE         64
/* 帧头定义（根据您的协议） */
#define FRAME_HEADER1       0xFF
#define FRAME_HEADER2       0xFF
#define FRAME_HEADER3       0xA1
#define RS485_FRAME_ID_MIN   0x01
#define RS485_FRAME_ID_MAX   0xFE

#define CAMERA_RX_TIMEOUT_MS 100

/* 命令定义 */
#define CMD_READ            0x02    // 读命令
#define CMD_WRITE           0x03    // 写命令

/* 模式定义（MODE字段） */
#define MODE_SPECIFY        0x01    // 指定模式（读操作）
#define MODE_NON_SPECIFY    0x02    // 非指定模式（读操作）
#define MODE_SWITCH         0x01    // 模式切换（写操作）
#define MODE_DEVICE_CTRL    0x02    // 设备控制（写操作）

/* 功能码（根据Excel表格整理，部分列举，其余可自行添加） */
// 识别类型（用于模式切换、读指定/非指定）
#define FUNC_COLOR         	0x01    // 颜色识别
#define FUNC_BlOCK          0x02    // 色块识别
#define FUNC_FACE          	0x03    // 人脸识别
#define FUNC_QRCODE         0x04    // 二维码识别
#define FUNC_NUMBER         0x05    // 数字识别
#define FUNC_LABEL          0x06    // 标签识别
#define FUNC_20CLASS        0x07    // 20类识别
#define FUNC_DEEPLEARN      0x08    // 深度学习
#define FUNC_CAM_CAPTURE    0x09    // 摄像头捕获（模式切换的一种）

// 设备控制项
#define CTRL_REBOOT         0x01    // 重启
#define CTRL_BRIGHTNESS     0x02    // 亮度
#define CTRL_CONTRAST       0x03    // 对比度
#define CTRL_EXPOSURE       0x04    // 曝光
#define CTRL_WB             0x05    // 白平衡
#define CTRL_SATURATION     0x06    // 饱和度
#define CTRL_SHARPEN        0x07    // 锐化
#define CTRL_LANG           0x11    // 系统语言
#define CTRL_OUTPUT_MODE    0x12    // 输出模式
#define CTRL_BORDER         0x13    // 边框显示
#define CTRL_SWITCH3        0x14    // 开关3
#define CTRL_SWITCH4        0x15    // 开关4
#define CTRL_BAUDRATE       0x16    // 串口波特率
#define CTRL_COLOR_AREA     0x21    // 颜色识别区域
#define CTRL_COLOR_VALUE    0x22    // 颜色识别颜色
#define CTRL_BLOCK_NUM      0x23    // 色块识别数量
#define CTRL_BLOCK_AREA     0x24    // 色块识别区域
#define CTRL_LINE_NUM       0x25    // 线条识别数量
#define CTRL_NUMBER_AREA    0x26    // 数字识别区域

// 颜色值
#define COLOR_RED           0x01
#define COLOR_GREEN         0x02
#define COLOR_BLUE          0x03
#define COLOR_YELLOW        0x04
#define COLOR_GRAY          0x05
#define COLOR_PURPLE        0x06
#define COLOR_WHITE         0x07
#define COLOR_BLACK         0x08

// 数字识别参数
#define NUMBER_0            0x10
#define NUMBER_1            0x01
#define NUMBER_2            0x02
#define NUMBER_3            0x03
#define NUMBER_4            0x04
#define NUMBER_5            0x05
#define NUMBER_6            0x06
#define NUMBER_7            0x07
#define NUMBER_8            0x08
#define NUMBER_9            0x09

// 标签识别参数
#define LABEL_1             0x01
#define LABEL_2             0x02
// ... 可继续定义到 LABEL_20 (0x14)

// 20类识别物品参数
#define ITEM_1              0x01
#define ITEM_2              0x02
// ... 可继续定义到 ITEM_20 (0x14)

// 结果值
#define RESULT_SUCCESS      0x01
#define RESULT_FAIL         0x00
#define RESULT_YES          0x01
#define RESULT_NO           0x02

/* 返回状态 */
#define CAMERA_OK           0
#define CAMERA_TIMEOUT      1
#define CAMERA_CHECKSUM_ERR 2
#define CAMERA_RESP_ERR     3

/* 函数声明 */
void RS485_Rx_Init(void);           // 初始化接收（包括启动第一次接收）
void RS485_Rx_Process(void);        // 主循环中调用，解析帧
uint8_t RS485_GetFrame(uint8_t *buffer, uint16_t *length); // 获取完整帧
static uint8_t CalcChecksum(uint8_t *pData, uint8_t len);
void  SendFrame(uint8_t id,uint8_t cmd, uint8_t mode, uint8_t *data, uint8_t len);
uint8_t RS485_WaitFrameById(uint8_t expectedId, uint8_t *buffer, uint16_t *length, uint32_t timeoutMs);
uint8_t Camera_DeviceCTRL(uint8_t id,uint8_t dat1,uint8_t dat2);
void Camera_DeviceControl(int8_t ctrl_item, uint8_t param);
uint8_t Camera_Reset(uint8_t id);
uint8_t Camera_SetMode(uint8_t id, uint8_t func);
uint8_t Camera_ReadColorSpec(uint8_t id, uint8_t color);
uint8_t Camera_ReadColorNonSpec(uint8_t id, uint8_t *detected_color);
uint8_t Camera_ReadFaceSpec(uint8_t id,uint8_t face_id);
uint8_t Camera_ReadFaceNonSpec(uint8_t id, uint8_t *detected_id);
uint8_t Camera_ReadNumberSpec(uint8_t id, uint8_t num_param);
uint8_t Camera_ReadNumberNonSpec(uint8_t id, uint8_t *number);
uint8_t Camera_ReadLabelSpec(uint8_t id, uint8_t label);
uint8_t Camera_ReadLabelNonSpec(uint8_t id, uint8_t *label);
uint8_t Camera_ReadQR_Code(uint8_t id, uint8_t *qr_buf, uint8_t *buf_len);
#endif
