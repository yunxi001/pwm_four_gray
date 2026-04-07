#include "DHT11.h"

#define DHT11_TIMEOUT_US 200

// 温湿度全局变量，供外部读取
volatile uint8_t dht11_temp = 0; // 温度整数部分
volatile uint8_t dht11_humi = 0; // 湿度整数部分

void DHT11_IO_OUTPUT_Init(void)
{
    // 1.����GPIOCʱ��
    // 2.����GPIO�ṹ��
    GPIO_InitTypeDef GPIO_InitStruct;
    // 3.�ṹ��������ã��������ģʽ
    GPIO_InitStruct.GPIO_Pin = DHT11_IO_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    // 4.GPIOC�˿����ų�ʼ��
    GPIO_Init(DHT11_IO_PORT, &GPIO_InitStruct);
}

void DHT11_IO_INPUT_Init(void)
{
    // 2.����GPIO�ṹ��
    GPIO_InitTypeDef GPIO_InitStruct;
    // 3.�ṹ��������ã���������ģʽ
    GPIO_InitStruct.GPIO_Pin = DHT11_IO_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    // 4.GPIOC�˿����ų�ʼ��
    GPIO_Init(DHT11_IO_PORT, &GPIO_InitStruct);
}

uint8_t DHT11_Start(void)
{
    uint32_t timeout;

    // 1. ����Ϊ���ģʽ����ʼ����
    DHT11_IO_OUTPUT_Init();
    GPIO_SetBits(DHT11_IO_PORT, DHT11_IO_PIN);
    Delay_ms(1);
    // 2. �������ߡ�18ms��Э��Ҫ��
    GPIO_ResetBits(DHT11_IO_PORT, DHT11_IO_PIN);
    Delay_ms(20);
    // 3. ��������20~40us���ȴ��ӻ���Ӧ
    GPIO_SetBits(DHT11_IO_PORT, DHT11_IO_PIN);
    Delay_us(30); // ����Э���20~40us
    // 4. ����Ϊ����ģʽ�����ӻ���Ӧ
    DHT11_IO_INPUT_Init();
    timeout = DHT11_TIMEOUT_US;
    // �ȴ��ӻ��������ߣ���Ӧ��ʼ�źţ�
    while (GPIO_ReadInputDataBit(DHT11_IO_PORT, DHT11_IO_PIN) && timeout--)
    {
        Delay_us(1);
    }
    if (timeout == 0)
        return 1; // ��ʱʧ��
    // 5. �ȴ��ӻ��������ߣ�80us��
    timeout = DHT11_TIMEOUT_US;
    while (!GPIO_ReadInputDataBit(DHT11_IO_PORT, DHT11_IO_PIN) && timeout--)
    {
        Delay_us(1);
    }
    if (timeout == 0)
        return 1; // ��ʱʧ��
    // 6. �ȴ��ӻ��������ߣ�׼�������ݣ�
    timeout = DHT11_TIMEOUT_US;
    while (GPIO_ReadInputDataBit(DHT11_IO_PORT, DHT11_IO_PIN) && timeout--)
    {
        Delay_us(1);
    }
    if (timeout == 0)
        return 1; // ��ʱʧ��
    return 0;     // ��ʼ�źųɹ�
}

uint8_t DHT11_Read_Byte(void)
{
    uint8_t byte = 0;
    uint32_t timeout, high_time;

    for (uint8_t i = 0; i < 8; i++)
    {
        // 1. �ȴ��͵�ƽ������50us��
        timeout = DHT11_TIMEOUT_US;
        while (!GPIO_ReadInputDataBit(DHT11_IO_PORT, DHT11_IO_PIN) && timeout--)
        {
            Delay_us(1);
        }
        if (timeout == 0)
            return 0xFF; // ��ʱ

        // 2. ��¼�ߵ�ƽ��ʼʱ�䣬���ߵ�ƽʱ��
        high_time = 0;
        timeout = DHT11_TIMEOUT_US;
        while (GPIO_ReadInputDataBit(DHT11_IO_PORT, DHT11_IO_PIN) && timeout--)
        {
            Delay_us(1);
            high_time++;
        }
        if (timeout == 0)
            return 0xFF; // ��ʱ

        // 3. �ж�bitֵ���ߵ�ƽ��40usΪ1������Ϊ0������DHT11ʱ��
        if (high_time >= 30)
        {
            byte |= (1 << (7 - i)); // ��λ�ȳ�
        }
    }
    return byte;
}

void DHT_Read(void)
{
    uint8_t data[5] = {0};
    DHT11_Start();
    for (uint8_t i = 0; i < 5; i++)
    {
        data[i] = DHT11_Read_Byte();
    }

    // 校验和验证，更新全局变量，不再直接操作数码管
    if ((data[0] + data[1] + data[2] + data[3]) == data[4])
    {
        dht11_temp = data[2]; // 温度整数部分
        dht11_humi = data[0]; // 湿度整数部分
//        printf("Temperature:%d.%d C\t", data[2], data[3]);
//        printf("Humidity:%d.%d\n", data[0], data[1]);
    }
}
