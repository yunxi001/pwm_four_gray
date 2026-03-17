#ifndef GFIVE_GRAY_SENSOR_H
#define GFIVE_GRAY_SENSOR_H

#include <stdint.h>
#include <stdbool.h>

// 灰度传感器配置
#define GRAY_SENSOR_ADDRESS 0x4F  // 7位地址 (0x9E >> 1 = 0x4F)
#define SENSOR_COUNT 5
#define I2C_TIMEOUT 50
#define BLACK_THRESHOLD 700

// 灰度传感器数据结构
typedef struct {
    uint16_t values[SENSOR_COUNT];
    bool updated;
    uint8_t i2cDevAddr;
    uint8_t sdaPin;
    uint8_t sclPin;
} GraySensorArray;

// 函数声明
void graySensor_init(GraySensorArray* sensor);
bool graySensor_read(GraySensorArray* sensor);
bool graySensor_anyBlackLineDetected(GraySensorArray* sensor);
uint16_t graySensor_getValue(GraySensorArray* sensor, int index);

#endif