#ifndef __CAMERA_PROTOCOL_H
#define __CAMERA_PROTOCOL_H

#include "stm32f10x.h"
#include <string.h>

/* 协议固定帧头 */
#define FRAME_HEADER1       0xFF
#define FRAME_HEADER2       0xFF
#define FRAME_HEADER3       0xA1
#define RS485_FRAME_ID_MIN  0x01
#define RS485_FRAME_ID_MAX  0xFE

/* ����� */
#define CMD_READ            0x02    // ������
#define CMD_WRITE           0x03    // д����

/* ģʽ���壨MODE�ֶΣ� */
#define MODE_SPECIFY        0x01    // ָ��ģʽ����������
#define MODE_NON_SPECIFY    0x02    // ��ָ��ģʽ����������
#define MODE_SWITCH         0x01    // ģʽ�л���д������
#define MODE_DEVICE_CTRL    0x02    // �豸���ƣ�д������

/* 功能码（根据Excel文档，以下是常用功能举例，可根据需要添加） */
// 识别类型（用于模式切换或指定/非指定）
#define FUNC_COLOR          0x01    // 颜色识别
#define FUNC_BlOCK          0x02    // 色块识别
#define FUNC_FACE           0x03    // 人脸识别
#define FUNC_QRCODE         0x04    // 二维码识别
#define FUNC_NUMBER         0x05    // 数字识别
#define FUNC_LABEL          0x06    // 标签识别
#define FUNC_20CLASS        0x07    // 20类识别
#define FUNC_DEEPLEARN      0x08    // 深度学习
#define FUNC_CAM_CAPTURE    0x09    // 摄像头捕获（模式切换的一种）

// �豸������
#define CTRL_REBOOT         0x01    // ����
#define CTRL_BRIGHTNESS     0x02    // ����
#define CTRL_CONTRAST       0x03    // �Աȶ�
#define CTRL_EXPOSURE       0x04    // �ع�
#define CTRL_WB             0x05    // ��ƽ��
#define CTRL_SATURATION     0x06    // ���Ͷ�
#define CTRL_SHARPEN        0x07    // ��
#define CTRL_LANG           0x11    // ϵͳ����
#define CTRL_OUTPUT_MODE    0x12    // ���ģʽ
#define CTRL_BORDER         0x13    // �߿���ʾ
#define CTRL_SWITCH3        0x14    // ����3
#define CTRL_SWITCH4        0x15    // ����4
#define CTRL_BAUDRATE       0x16    // ���ڲ�����
#define CTRL_COLOR_AREA     0x21    // ��ɫʶ������
#define CTRL_COLOR_VALUE    0x22    // ��ɫʶ����ɫ
#define CTRL_BLOCK_NUM      0x23    // ɫ��ʶ������
#define CTRL_BLOCK_AREA     0x24    // ɫ��ʶ������
#define CTRL_LINE_NUM       0x25    // ����ʶ������
#define CTRL_NUMBER_AREA    0x26    // ����ʶ������

// ��ɫֵ
#define COLOR_RED           0x01
#define COLOR_GREEN         0x02
#define COLOR_BLUE          0x03
#define COLOR_YELLOW        0x04
#define COLOR_GRAY          0x05
#define COLOR_PURPLE        0x06
#define COLOR_WHITE         0x07
#define COLOR_BLACK         0x08

// ����ʶ�����
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

// ��ǩʶ�����
#define LABEL_1             0x01
#define LABEL_2             0x02
// ... �ɼ������嵽 LABEL_20 (0x14)

// 20��ʶ����Ʒ����
#define ITEM_1              0x01
#define ITEM_2              0x02
// ... �ɼ������嵽 ITEM_20 (0x14)

// ���ֵ
#define RESULT_SUCCESS      0x01
#define RESULT_FAIL         0x00
#define RESULT_YES          0x01
#define RESULT_NO           0x02

/* ����״̬ */
#define CAMERA_OK           0
#define CAMERA_TIMEOUT      1
#define CAMERA_CHECKSUM_ERR 2
#define CAMERA_RESP_ERR     3

/* 缓冲区大小 */
#define RX_BUF_SIZE         128
#define TX_BUF_SIZE         64
#define MAX_DATA_LEN        32

/* 超时设置 */
#define CAMERA_RX_TIMEOUT_MS 100

/* 初始化 */
void Camera_Init(void);

/* 相机中断处理函数（由Serial.c的USART2_IRQHandler调用） */
void Camera_IRQHandler(void);

/* 在主程序循环中调用，解析接收帧 */
void Camera_Process(void);

/* 获取已接收的完整帧，返回1成功，0无帧 */
uint8_t Camera_GetFrame(uint8_t *buffer, uint16_t *length);

/* 等待指定ID的帧响应 */
uint8_t Camera_WaitFrameById(uint8_t expectedId, uint8_t *buffer, uint16_t *length, uint32_t timeoutMs);

/* ========== 高层封装函数 ========== */

// 设备重启（写命令）
uint8_t Camera_Reset(uint8_t id);

// 模式切换（写命令）
uint8_t Camera_SetMode(uint8_t id, uint8_t func);   // func如 FUNC_COLOR, FUNC_BLOCK...

// 设备控制（写命令，返回成功/失败）
uint8_t Camera_DeviceControl(uint8_t id, uint8_t dat1, uint8_t dat2);

// 读指定颜色识别（返回识别结果）
uint8_t Camera_ReadColorSpec(uint8_t id, uint8_t color);

// 读非指定颜色识别（返回检测到的颜色，是否识别到）
uint8_t Camera_ReadColorNonSpec(uint8_t id, uint8_t *detected_color);

// 读指定人脸识别（指定ID）
uint8_t Camera_ReadFaceSpec(uint8_t id, uint8_t face_id);

// 读非指定人脸识别（返回识别到的ID，是否识别到）
uint8_t Camera_ReadFaceNonSpec(uint8_t id, uint8_t *detected_id);

// 读指定数字识别
uint8_t Camera_ReadNumberSpec(uint8_t id, uint8_t num_param);

// 读非指定数字识别（返回识别数字和结果）
uint8_t Camera_ReadNumberNonSpec(uint8_t id, uint8_t *number);

// 读指定标签识别
uint8_t Camera_ReadLabelSpec(uint8_t id, uint8_t label);

// 读非指定标签识别（返回标签和结果）
uint8_t Camera_ReadLabelNonSpec(uint8_t id, uint8_t *label);

// 读取二维码数据
uint8_t Camera_ReadQR_Code(uint8_t id, uint8_t *qr_buf, uint8_t *buf_len);

#endif
