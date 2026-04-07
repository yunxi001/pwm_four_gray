#include "TM1637.h"

// ��������ܶ��루0-9, A-F, Ϩ��, С���㣩����λ��ǰ����TM1637
const uint8_t TM1637_SEG_TABLE[] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F, // 9
    0x77, // A
    0x7C, // B
    0x39, // C
    0x5E, // D
    0x79, // E
    0x71, // F
    0x00, // Ϩ��
    0x80  // С���㣨�����ֶ�����ӣ���0x3F|0x80=0xBF��ʾ0.��
};

void SMG_IO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 1. 开启 GPIOA 时钟（CLK=PA4, DIO=PA5）
    TM1637_IO_CLK_ENABLE();

    // 2. 配置 CLK 和 DIO 引脚为推挽输出
    GPIO_InitStructure.GPIO_Pin = TM1637_CLK_PIN | TM1637_DIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 50MHz输出速度
    GPIO_Init(TM1637_CLK_PORT, &GPIO_InitStructure);  // 初始化引脚
}

// ��ʼ�źţ�CLK�ߵ�ƽʱ��DIO�Ӹ�����
static void TM1637_Start(void)
{
    TM1637_CLK_HIGH();
    TM1637_DIO_HIGH();
    Delay_us(1);
    TM1637_DIO_LOW();
    Delay_us(1);
    TM1637_CLK_LOW();
    Delay_us(1);
}

// ֹͣ�źţ�CLK�ߵ�ƽʱ��DIO�ӵ�����
static void TM1637_Stop(void)
{
    TM1637_CLK_LOW();
    TM1637_DIO_LOW();
    Delay_us(1);
    TM1637_CLK_HIGH();
    Delay_us(1);
    TM1637_DIO_HIGH();
    Delay_us(1);
}

// д1���ֽڣ���λ��ǰ��������Ӧ��״̬
static uint8_t TM1637_WriteByte(uint8_t byte)
{
    uint8_t i, ack = 0;

    for (i = 0; i < 8; i++) // ��λ���ͣ���λ���ͣ�
    {
        TM1637_CLK_LOW();
        Delay_us(1);

        // ���͵�ǰλ
        if (byte & 0x01)
            TM1637_DIO_HIGH();
        else
            TM1637_DIO_LOW();

        byte >>= 1;
        Delay_us(1);

        TM1637_CLK_HIGH(); // CLK��������������
        Delay_us(1);
    }

    // ��ȡӦ���ź�
    TM1637_CLK_LOW();
    TM1637_DIO_HIGH(); // �ͷ�DIO����
    Delay_us(1);
    TM1637_CLK_HIGH();
    Delay_us(1);

    if (TM1637_DIO_READ() == 0) // DIO���ͱ�ʾӦ��ɹ�
        ack = 1;

    TM1637_CLK_LOW();
    Delay_us(1);

    return ack;
}

void TM1637_Init(void)
{
    SMG_IO_Init();

    // 3. ��ʼ��ƽ�ø�
    TM1637_CLK_HIGH();
    TM1637_DIO_HIGH();

    // 4. ��ʼ����ʾ״̬
    TM1637_ClearDisplay();
}

// �������ȣ�level=0~7��0���7������
void TM1637_SetBrightness(uint8_t level)
{
    if (level > 7)
        level = 7;
    TM1637_Start();
    TM1637_WriteByte(0x88 | level); // �������0x88=�������� + ���ȵȼ�
    TM1637_Stop();
}

// �����ʾ�����������Ϩ��
void TM1637_ClearDisplay(void)
{
    uint8_t i;
    TM1637_Start();
    TM1637_WriteByte(0x40); // �Զ���ַ����ģʽ
    TM1637_Stop();

    TM1637_Start();
    TM1637_WriteByte(0xC0);
    for (i = 0; i < 4; i++) // 4λ�����ȫ��дϨ����
        TM1637_WriteByte(0x00);
    TM1637_Stop();

    TM1637_SetBrightness(3); // Ĭ������3
}

// ��ʾ4λ���֣�num=0~9999��������Χ��ʾ"----"
void TM1637_DisplayNumber(uint16_t num)
{
    uint8_t seg_data[4];

    // ���ֲ�֣�����0~9999��
    if (num > 9999) // ������Χ��ʾ"----"
    {
        seg_data[0] = 0x40; // - ���Ŷ���
        seg_data[1] = 0x40;
        seg_data[2] = 0x40;
        seg_data[3] = 0x40;
    }
    else
    {
        seg_data[0] = TM1637_SEG_TABLE[num / 1000];       // ǧλ
        seg_data[1] = TM1637_SEG_TABLE[(num / 100) % 10]; // ��λ
        seg_data[2] = TM1637_SEG_TABLE[(num / 10) % 10];  // ʮλ
        seg_data[3] = TM1637_SEG_TABLE[num % 10];         // ��λ
    }

    // ������ʾ����
    TM1637_Start();
    TM1637_WriteByte(0x40); // �Զ���ַ����ģʽ
    TM1637_Stop();

    TM1637_Start();
    TM1637_WriteByte(0xC0); // ��ʼ��ַ0x00
    TM1637_WriteByte(seg_data[0]);
    TM1637_WriteByte(seg_data[1]);
    TM1637_WriteByte(seg_data[2]);
    TM1637_WriteByte(seg_data[3]);
    TM1637_Stop();

    TM1637_SetBrightness(3); // ��ʾ����3
}

// ��ʾָ��λ�õ��ַ���pos=0~3����Ӧǧλ~��λ��seg_code=�����������
void TM1637_DisplaySeg(uint8_t pos, uint8_t seg_code)
{
    if (pos > 3)
        return;

    TM1637_Start();
    TM1637_WriteByte(0x44); // �̶���ַģʽ
    TM1637_Stop();

    TM1637_Start();
    TM1637_WriteByte(0xC0 + pos); // ����ָ��λ�õ�ַ
    TM1637_WriteByte(TM1637_SEG_TABLE[seg_code]);
    TM1637_Stop();
}
