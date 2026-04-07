# RS485相机测试指南

## 测试程序说明

已创建两个测试程序：

### 1. main_camera_simple_test.c（推荐）
**简单快速测试程序**，用于验证基本通信功能。

功能：
- 切换到颜色识别模式
- 持续读取并显示检测到的颜色
- LED指示灯闪烁提示

适合：快速验证相机是否正常工作

### 2. main_camera_test.c
**完整功能测试程序**，测试所有识别功能。

功能：
- 模式切换测试
- 颜色识别（指定/非指定）
- 数字识别
- 人脸识别
- 标签识别
- 设备控制
- 连续监测模式

适合：全面测试相机各项功能

## 使用步骤

### 1. 硬件连接
- 确保RS485相机正确连接到USART2
- 检查RS485方向控制引脚（PC4）
- 确认相机供电正常

### 2. 配置相机ID
在测试程序中修改相机ID（默认0x01）：
```c
#define CAMERA_ID  0x01  // 根据实际硬件修改
```

### 3. 配置波特率
确保波特率与相机设置一致（默认115200）：
```c
S485_Init(115200);  // 根据相机设置修改
```

### 4. 编译和下载
1. 在Keil中选择要测试的main文件
2. 编译项目
3. 下载到STM32
4. 打开串口调试助手（115200波特率）

### 5. 查看测试结果
通过串口输出查看测试结果：
- ✓ 表示成功
- ✗ 表示失败
- ○ 表示未检测到

## 常见问题

### 1. 通信超时
**现象**：所有操作都返回0或超时

**可能原因**：
- RS485方向控制不正确
- 波特率不匹配
- 相机ID设置错误
- 硬件连接问题

**解决方法**：
- 检查`S485_Dir_Control()`函数实现
- 确认相机波特率设置
- 使用示波器检查信号

### 2. 校验和错误
**现象**：接收到数据但校验失败

**可能原因**：
- 数据传输干扰
- 波特率偏差
- 线路质量问题

**解决方法**：
- 检查RS485线缆质量
- 添加终端电阻
- 降低波特率测试

### 3. 识别结果不准确
**现象**：通信正常但识别错误

**可能原因**：
- 光照条件不佳
- 目标距离不合适
- 相机参数未调整

**解决方法**：
- 调整光照环境
- 调整相机与目标距离
- 使用`Camera_DeviceControl()`调整参数

## API使用示例

### 基本流程
```c
// 1. 初始化
Camera_Init();

// 2. 主循环
while(1) {
    // 必须定期调用
    Camera_Process();
    
    // 3. 切换模式
    Camera_SetMode(CAMERA_ID, FUNC_COLOR);
    
    // 4. 读取识别结果
    uint8_t color;
    uint8_t result = Camera_ReadColorNonSpec(CAMERA_ID, &color);
    
    if (result != 0) {
        // 处理识别结果
    }
    
    Delay_ms(100);
}
```

### 指定识别示例
```c
// 检测是否有红色
uint8_t result = Camera_ReadColorSpec(CAMERA_ID, COLOR_RED);
if (result == RESULT_YES) {
    printf("检测到红色\n");
}
```

### 非指定识别示例
```c
// 检测任意颜色
uint8_t color;
uint8_t result = Camera_ReadColorNonSpec(CAMERA_ID, &color);
if (result != 0) {
    printf("检测到颜色: 0x%02X\n", color);
}
```

## 调试技巧

### 1. 添加调试输出
在`Camera_IRQHandler()`中添加：
```c
printf("RX: 0x%02X\n", data);  // 打印接收到的每个字节
```

### 2. 检查帧格式
在`Camera_WaitFrameById()`中添加：
```c
printf("Frame: ");
for(int i = 0; i < rxLenLocal; i++) {
    printf("%02X ", rxFrameLocal[i]);
}
printf("\n");
```

### 3. 监控超时
```c
printf("等待响应... (超时: %dms)\n", CAMERA_RX_TIMEOUT_MS);
```

## 性能优化建议

1. **调用频率**：`Camera_Process()`建议每10-50ms调用一次
2. **超时设置**：根据实际响应时间调整`CAMERA_RX_TIMEOUT_MS`
3. **缓冲区大小**：如需处理更多数据，增加`RX_BUF_SIZE`
4. **中断优先级**：确保USART2中断优先级合理设置

## 下一步

测试通过后，可以：
1. 集成到实际应用中
2. 添加错误重试机制
3. 实现多相机支持
4. 优化识别算法参数
