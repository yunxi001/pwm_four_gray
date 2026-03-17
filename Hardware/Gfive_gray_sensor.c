//#include "Gfive_gray_sensor.h"
//#include "delay.h"

//// 软件I2C基础时序函数
//static void i2c_delay(void) {
//    Delay_us(5);
//}

//static void sda_out(GraySensorArray* sensor) {
//    pinMode(sensor->sdaPin, OUTPUT);
//}

//static void sda_in(GraySensorArray* sensor) {
//    pinMode(sensor->sdaPin, INPUT);
//}

//static void scl_high(GraySensorArray* sensor) {
//    digitalWrite(sensor->sclPin, HIGH);
//    i2c_delay();
//}

//static void scl_low(GraySensorArray* sensor) {
//    digitalWrite(sensor->sclPin, LOW);
//    i2c_delay();
//}

//static void sda_high(GraySensorArray* sensor) {
//    digitalWrite(sensor->sdaPin, HIGH);
//    i2c_delay();
//}

//static void sda_low(GraySensorArray* sensor) {
//    digitalWrite(sensor->sdaPin, LOW);
//    i2c_delay();
//}

//static void i2c_start(GraySensorArray* sensor) {
//    sda_out(sensor);
//    sda_high(sensor);
//    scl_high(sensor);
//    sda_low(sensor);
//    scl_low(sensor);
//}

//static void i2c_stop(GraySensorArray* sensor) {
//    sda_out(sensor);
//    scl_low(sensor);
//    sda_low(sensor);
//    scl_high(sensor);
//    sda_high(sensor);
//    i2c_delay();
//}

//static uint8_t i2c_write_byte(GraySensorArray* sensor, uint8_t data) {
//    sda_out(sensor);
//    for (uint8_t i = 0; i < 8; i++) {
//        if (data & 0x80) {
//            sda_high(sensor);
//        } else {
//            sda_low(sensor);
//        }
//        data <<= 1;
//        scl_high(sensor);
//        scl_low(sensor);
//    }
//    sda_in(sensor);
//    scl_high(sensor);
//    uint8_t ack = digitalRead(sensor->sdaPin);
//    scl_low(sensor);
//    sda_out(sensor);
//    return ack;
//}

//static uint8_t i2c_read_byte(GraySensorArray* sensor, uint8_t ack) {
//    sda_in(sensor);
//    uint8_t data = 0;
//    for (uint8_t i = 0; i < 8; i++) {
//        data <<= 1;
//        scl_high(sensor);
//        if (digitalRead(sensor->sdaPin)) {
//            data |= 0x01;
//        }
//        scl_low(sensor);
//    }
//    sda_out(sensor);
//    if (ack) {
//        sda_low(sensor);
//    } else {
//        sda_high(sensor);
//    }
//    scl_high(sensor);
//    scl_low(sensor);
//    sda_high(sensor);
//    return data;
//}

//// 初始化函数
//void graySensor_init(GraySensorArray* sensor)
// {

//    sensor->sdaPin = 20;
//    sensor->sclPin = 21;

//    sensor->i2cDevAddr = GRAY_SENSOR_ADDRESS;
//    sensor->updated = false;
//    
//    for (int i = 0; i < SENSOR_COUNT; i++) 
//    {
//        sensor->values[i] = 0;
//    }
//    pinMode(sensor->sdaPin, OUTPUT);
//    pinMode(sensor->sclPin, OUTPUT);
//    sda_high(sensor);
//    scl_high(sensor);
//}

//// 快速读取所有传感器值
//bool graySensor_read(GraySensorArray* sensor) {
//    uint8_t rawBytes[SENSOR_COUNT * 2];
//    
//    // 开始传输（写模式）
//    i2c_start(sensor);
//    if (i2c_write_byte(sensor, sensor->i2cDevAddr << 1)) {
//        i2c_stop(sensor);
//        return false;
//    }
//    
//    // 写入寄存器地址
//    if (i2c_write_byte(sensor, 0x00)) {
//        i2c_stop(sensor);
//        return false;
//    }
//    
//    // 重复起始条件
//    i2c_start(sensor);
//    
//    // 请求读取数据
//    if (i2c_write_byte(sensor, (sensor->i2cDevAddr << 1) | 0x01)) {
//        i2c_stop(sensor);
//        return false;
//    }
//    
//    // 读取数据（带超时检查）
//    unsigned long start = millis();
//    delayMicroseconds(100);
//    
//    // 读取所有字节
//    for (int i = 0; i < SENSOR_COUNT * 2; i++) {
//        // 检查超时
//        if (millis() - start > I2C_TIMEOUT) {
//            i2c_stop(sensor);
//            return false;
//        }
//        
//        // 除最后一个字节外发送ACK
//        rawBytes[i] = i2c_read_byte(sensor, i < (SENSOR_COUNT * 2 - 1) ? 1 : 0);
//    }
//    
//    i2c_stop(sensor);
//    
//    // 解析数据到数组
//    for (int i = 0; i < SENSOR_COUNT; i++) {
//        sensor->values[i] = rawBytes[i*2] + (rawBytes[i*2+1] * 256);
//    }
//    
//    sensor->updated = true;
//    return true;
//}

//// 检查任意传感器是否检测到黑线
//bool graySensor_anyBlackLineDetected(GraySensorArray* sensor) {
//    for (int i = 0; i < SENSOR_COUNT; i++) {
//        if (sensor->values[i] < BLACK_THRESHOLD) {
//            return true;
//        }
//    }
//    return false;
//}

//// 获取指定传感器值
//uint16_t graySensor_getValue(GraySensorArray* sensor, int index) {
//    if (index >= 0 && index < SENSOR_COUNT) {
//        return sensor->values[index];
//    }
//    return 0;
//}