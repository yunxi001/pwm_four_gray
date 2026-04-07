#include "Gir_distance.h"

// 全局变量定义
static sGIr_distance_sensor_DetailedData_t _detailedData;
static uint16_t _distance;

// 内部函数声明
static void writeByteData(uint8_t Reg, uint8_t byte);
static uint8_t readByteData(uint8_t Reg);
static void writeData(uint8_t Reg, uint8_t *buf, uint8_t Num);
static void readData(uint8_t Reg, uint8_t Num);
static void setDeviceAddress(uint8_t newAddr);
static void highPrecisionEnable(eFunctionalState NewState);
static void dataInit(void);
static void readGIr_distance_sensor(void);
static void start(void);
static void stop(void);


void Distance_IO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 1. 开启GPIOC时钟
    Distance_IO_CLK_ENABLE();
    
    // 2. 配置CLK和DIO引脚为开漏输出
    GPIO_InitStructure.GPIO_Pin = Distance_SCL_PIN | Distance_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;    // 开漏输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   // 50MHz输出速率
    GPIO_Init(Distance_SDA_PORT, &GPIO_InitStructure);   // 初始化引脚
}
void Distance_SDA_SetInput(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = Distance_SDA_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
    GPIO_Init(Distance_SDA_PORT, &GPIO_InitStruct);
}

void Distance_SDA_SetOutput(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = Distance_SDA_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD; // 开漏输出
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(Distance_SDA_PORT, &GPIO_InitStruct);
}

// SCL置高
static void scl_high(void) {
    GPIO_SetBits(Distance_SCL_PORT, Distance_SCL_PIN);
    Delay_us(15); 
}

// SCL置低
static void scl_low(void) {
    GPIO_ResetBits(Distance_SCL_PORT, Distance_SCL_PIN);
    Delay_us(15);
}

// SDA置高
static void sda_high(void) {
    GPIO_SetBits(Distance_SDA_PORT, Distance_SDA_PIN);
    Delay_us(15); 
}
// SDA置低
static void sda_low(void) {
    GPIO_ResetBits(Distance_SDA_PORT, Distance_SDA_PIN);
    Delay_us(15);
}

// 读取SDA电平
static uint8_t sda_read(void){
    return GPIO_ReadInputDataBit(Distance_SDA_PORT, Distance_SDA_PIN);
}



static void i2c_start(void) {
    Distance_SDA_SetOutput();
    sda_high();
    scl_high();
    sda_low();  // SCL高时SDA拉低=起始
    scl_low();
}

static void i2c_stop(void) {
    Distance_SDA_SetOutput();
    scl_low();
    sda_low();
    scl_high(); // SCL高时SDA拉高=停止
    sda_high();
    Delay_us(15);
}

static uint8_t i2c_write_byte(uint8_t data) {
    Distance_SDA_SetOutput();
    for (uint8_t i = 0; i < 8; i++) { // 高位先送
        if (data & 0x80) {
            sda_high();
        } else {
            sda_low();
        }
        data <<= 1;
        scl_high();
        scl_low();
    }
    Distance_SDA_SetInput(); // 切换为输入读ACK
    scl_high();
    uint8_t ack = sda_read(); // 0=ACK，1=NACK
    scl_low();
    return ack;
}

static uint8_t i2c_read_byte(uint8_t ack) {
    Distance_SDA_SetInput();
    uint8_t data = 0;
    for (uint8_t i = 0; i < 8; i++) { // 高位先读
        data <<= 1;
        scl_high();
        if (sda_read()) {
            data |= 0x01;
        }
        scl_low();
    }
    Distance_SDA_SetOutput(); // 切换为输出发ACK
    if (ack) {
        sda_low(); // 发送ACK
    } else {
        sda_high(); // 发送NACK
    }
    scl_high();
    scl_low();
    return data;
}

static void writeByteData(uint8_t Reg, uint8_t byte) {
    i2c_start();
    i2c_write_byte(_detailedData.i2cDevAddr << 1);
    i2c_write_byte(Reg);
    i2c_write_byte(byte);
    i2c_stop();
}

static uint8_t readByteData(uint8_t Reg) {
    uint8_t data;
    i2c_start();
    i2c_write_byte(_detailedData.i2cDevAddr << 1);
    i2c_write_byte(Reg);
    i2c_start();
    i2c_write_byte((_detailedData.i2cDevAddr << 1) | 0x01);
    data = i2c_read_byte(0);
    i2c_stop();
    return data;
}

static void writeData(uint8_t Reg, uint8_t *buf, uint8_t Num) {
    for (uint8_t i = 0; i < Num; i++) {
        writeByteData(Reg++, buf[i]);
    }
}

static void readData(uint8_t Reg, uint8_t Num) {
    i2c_start();
    i2c_write_byte(_detailedData.i2cDevAddr << 1);
    i2c_write_byte(Reg);
    i2c_start();
    i2c_write_byte((_detailedData.i2cDevAddr << 1) | 0x01);

    for (int i = 0; i < Num; i++) {
        _detailedData.originalData[i] = i2c_read_byte(i < (Num - 1) ? 1 : 0);
    }
    i2c_stop();
}

static void setDeviceAddress(uint8_t newAddr) {
    newAddr &= 0x7F;
    writeByteData(GIr_distance_sensor_REG_I2C_SLAVE_DEVICE_ADDRESS, newAddr);
    _detailedData.i2cDevAddr = newAddr;
}

static void highPrecisionEnable(eFunctionalState NewState) {
    writeByteData(GIr_distance_sensor_REG_SYSTEM_RANGE_CONFIG, NewState);
}

static void dataInit(void) {
    uint8_t data;
#ifdef ESD_2V8
    data = readByteData(GIr_distance_sensor_REG_VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV);
    data = (data & 0xFE) | 0x01;
    writeByteData(GIr_distance_sensor_REG_VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV, data);
#endif
    writeByteData(0x88, 0x00);
    writeByteData(0x80, 0x01);
    writeByteData(0xFF, 0x01);
    writeByteData(0x00, 0x00);
    readByteData(0x91);
    writeByteData(0x91, 0x3c);
    writeByteData(0x00, 0x01);
    writeByteData(0xFF, 0x00);
    writeByteData(0x80, 0x00);
}

static void readGIr_distance_sensor(void) {
    readData(GIr_distance_sensor_REG_RESULT_RANGE_STATUS, 12);
    _detailedData.ambientCount = ((_detailedData.originalData[6] & 0xFF) << 8) | 
                                 (_detailedData.originalData[7] & 0xFF);
    _detailedData.signalCount = ((_detailedData.originalData[8] & 0xFF) << 8) | 
                                (_detailedData.originalData[9] & 0xFF);
    _detailedData.distance = ((_detailedData.originalData[10] & 0xFF) << 8) | 
                             (_detailedData.originalData[11] & 0xFF);
    _detailedData.status = ((_detailedData.originalData[0] & 0x78) >> 3);
}

static void start(void) {
    uint8_t DeviceMode;
    uint8_t Byte;
    uint8_t StartStopByte = GIr_distance_sensor_REG_SYSRANGE_MODE_START_STOP;
    uint32_t LoopNb;
    
    DeviceMode = _detailedData.mode;
    
    writeByteData(0x80, 0x01);
    writeByteData(0xFF, 0x01);
    writeByteData(0x00, 0x00);
    writeByteData(0x91, 0x3c);
    writeByteData(0x00, 0x01);
    writeByteData(0xFF, 0x00);
    writeByteData(0x80, 0x00);
    
    switch (DeviceMode) {
        case eSingle:
            writeByteData(GIr_distance_sensor_REG_SYSRANGE_START, 0x01);
            Byte = StartStopByte;
            LoopNb = 0;
            do {
                if (LoopNb > 0) {
                    Byte = readByteData(GIr_distance_sensor_REG_SYSRANGE_START);
                }
                LoopNb++;
            } while (((Byte & StartStopByte) == StartStopByte) && 
                     (LoopNb < GIr_distance_sensor_DEFAULT_MAX_LOOP));
            break;
        case eContinuous:
            writeByteData(GIr_distance_sensor_REG_SYSRANGE_START, GIr_distance_sensor_REG_SYSRANGE_MODE_BACKTOBACK);
            break;
        default:
            printf("---Selected mode not supported---\r\n");
    }
}

static void stop(void) {
    writeByteData(GIr_distance_sensor_REG_SYSRANGE_START, GIr_distance_sensor_REG_SYSRANGE_MODE_SINGLESHOT);
    
    writeByteData(0xFF, 0x01);
    writeByteData(0x00, 0x00);
    writeByteData(0x91, 0x00);
    writeByteData(0x00, 0x01);
    writeByteData(0xFF, 0x00);
}

// 传感器初始化
void Gir_distance_sensor_init(void) {
    uint8_t val1;
    Delay_ms(2000);  // 传感器启动延迟
    _detailedData.i2cDevAddr = GIr_distance_sensor_DEF_I2C_ADDR;
    
    // 初始化传感器IO
    Distance_IO_Init();
    
    // 初始电平置高
    sda_high();
    scl_high();
    
    dataInit();
    setDeviceAddress(0x29);
    
    // 读取设备信息
    val1 = readByteData(GIr_distance_sensor_REG_IDENTIFICATION_REVISION_ID);
    printf("\r\nRevision ID: %X\r\n", val1);
    
    val1 = readByteData(GIr_distance_sensor_REG_IDENTIFICATION_MODEL_ID);
    printf("Device ID: %X\r\n\r\n", val1);
}


void Gir_setMode(ePrecisionState precision) {
    _detailedData.mode = 1; // 连续模式
    if (precision == eHigh) {
        highPrecisionEnable(eENABLE);
        _detailedData.precision = precision;
    } else {
        highPrecisionEnable(edisable);
        _detailedData.precision = precision;
    }
    start(); // 设置模式后启动测量
}


float getDistance(void) {
    if (_detailedData.mode == eSingle) {
        start(); // 单次模式每次读取前启动
    }
    
    readGIr_distance_sensor();
    
    // 距离数据处理
    if (_detailedData.distance == 20) {
        _detailedData.distance = _distance;
    } else {
        _distance = _detailedData.distance;
    }
    
    float distance_mm;
    if (_detailedData.precision == eHigh) {
        distance_mm = _detailedData.distance / 4.0; // 高精度模式除以4
    } else {
        distance_mm = _detailedData.distance;
    }
    
    // 转换为cm并保留一位小数，减3校准
    float distance_cm = distance_mm / 10.0;
		printf("distance:%.2fcm\n",distance_cm);
    return (float)((uint32_t)(distance_cm * 10 + 0.5)) / 10.0 - 3;
}

