# 相机协议移植说明

## 概述
已将新版本的RS485相机协议从HAL库（F4）移植到标准库（F1）版本。

## 主要变更

### 1. 协议层改进
- **多设备支持**：所有函数增加了 `id` 参数，支持多个相机设备
- **简化接口**：函数直接返回结果值，不再需要传递多个指针参数
- **优化状态机**：采用更简洁的帧解析状态机

### 2. 函数接口变更

#### 旧版本接口示例：
```c
uint8_t result;
uint8_t ret = Camera_ReadColorSpec(COLOR_RED, &result);
if (ret == CAMERA_OK) {
    // 使用 result
}
```

#### 新版本接口示例：
```c
uint8_t result = Camera_ReadColorSpec(0x01, COLOR_RED);
if (result == RESULT_SUCCESS) {
    // 识别成功
}
```

### 3. 核心函数列表

#### 初始化和处理
- `Camera_Init()` - 初始化协议
- `Camera_Process()` - 在主循环中调用，解析接收数据

#### 设备控制
- `Camera_Reset(id)` - 重启设备
- `Camera_SetMode(id, func)` - 切换识别模式
- `Camera_DeviceControl(id, ctrl_item, param)` - 设备参数控制

#### 识别功能
- `Camera_ReadColorSpec(id, color)` - 读指定颜色
- `Camera_ReadColorNonSpec(id, *detected_color)` - 读任意颜色
- `Camera_ReadFaceSpec(id, face_id)` - 读指定人脸
- `Camera_ReadFaceNonSpec(id, *detected_id)` - 读任意人脸
- `Camera_ReadNumberSpec(id, num)` - 读指定数字
- `Camera_ReadNumberNonSpec(id, *number)` - 读任意数字
- `Camera_ReadLabelSpec(id, label)` - 读指定标签
- `Camera_ReadLabelNonSpec(id, *label)` - 读任意标签
- `Camera_ReadQR_Code(id, *buf, *len)` - 读二维码

### 4. 使用示例

```c
// 初始化
Camera_Init();

// 主循环
while(1) {
    Camera_Process();  // 必须定期调用
    
    // 切换到颜色识别模式
    if (Camera_SetMode(0x01, FUNC_COLOR) == RESULT_SUCCESS) {
        // 读取红色识别结果
        uint8_t result = Camera_ReadColorSpec(0x01, COLOR_RED);
        if (result == RESULT_YES) {
            printf("检测到红色\n");
        }
    }
    
    Delay_ms(100);
}
```

### 5. 注意事项

1. **设备ID范围**：0x01 ~ 0xFE
2. **超时设置**：默认100ms，可通过 `CAMERA_RX_TIMEOUT_MS` 修改
3. **必须调用**：`Camera_Process()` 必须在主循环中定期调用
4. **串口配置**：确保USART2已正确初始化并使能接收中断
5. **RS485方向控制**：需要实现 `S485_Dir_Control()` 函数

### 6. 依赖项

- `stm32f10x.h` - STM32F1标准库
- `Serial.h` - 串口相关定义
- `Delay.h` - 延时和系统时钟（sys_tick_counter）
- `S485_Dir_Control()` - RS485方向控制函数（外部实现）

## 移植完成度

✅ 核心协议层
✅ 帧解析状态机
✅ 多设备ID支持
✅ 所有识别功能
✅ 设备控制功能
✅ 标准库适配

## 测试建议

1. 测试单设备通信
2. 测试多设备通信（不同ID）
3. 测试各种识别功能
4. 测试超时处理
5. 测试校验和错误处理
