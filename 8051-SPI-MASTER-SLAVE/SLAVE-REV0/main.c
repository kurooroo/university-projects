#include <REGX52.H>

// Khai bao SPI
#define SPI_SCLK P2_0
#define SPI_MOSI P2_1
#define SPI_MISO P2_2
#define SPI_SS P3_2
#define DS18B20_DATA P2_4
#define LIGHT1 P2_6
#define LIGHT2 P2_7

unsigned char DS18B20_buffer[2];
// delay ms
void DelayMs(unsigned int time)
{
    unsigned int i, j;
    for (i = 0; i < time; i++)
    {
        for (j = 0; j < 123; j++)
        {
        }
    }
}

// Delay cho DS18B20
void DS18B20_Delay(unsigned int time)
{
    while (time--)
        ;
}

// Khoi tao cam bien
void DS18B20_Init()
{
    DS18B20_DATA = 1;
    DS18B20_Delay(8);
    DS18B20_DATA = 0;
    DS18B20_Delay(80);
    DS18B20_DATA = 1;
    DS18B20_Delay(34);
}

// Doc 1 byte tu DS18B20
unsigned char DS18B20_ReadByte()
{
    unsigned char i;
    unsigned char _data = 0x00;
    for (i = 8; i > 0; i--)
    {
        DS18B20_DATA = 0;
        _data >>= 1;
        DS18B20_DATA = 1;
        if (DS18B20_DATA)
        {
            _data |= 0x80;
        }
        DS18B20_Delay(4);
    }
    return _data;
}

// Ghi 1 byte vao DS18B20
void DS18B20_WriteByte(unsigned char _data)
{
    unsigned int i;
    for (i = 8; i > 0; i--)
    {
        DS18B20_DATA = 0;
        DS18B20_DATA = _data & 0x01;
        DS18B20_Delay(5);
        DS18B20_DATA = 1;
        _data >>= 1;
    }
    DS18B20_Delay(4);
}

// Doc nhiet do
void DS18B20_ReadTemperature(void)
{
    DS18B20_Init();
    DS18B20_WriteByte(0xCC);
    DS18B20_WriteByte(0x44);
    DS18B20_Delay(10);
    DS18B20_Init();
    DS18B20_WriteByte(0xCC);
    DS18B20_WriteByte(0xBE);
    DS18B20_buffer[0] = DS18B20_ReadByte();
    DS18B20_buffer[1] = DS18B20_ReadByte();
}

// Cau hinh soft spi che do slave
void SoftSPI_Init()
{
    SPI_SCLK = 1;
    SPI_MOSI = 1;
    // Cau hinh ngat cho chan SS
    IT0 = 1;
    EX0 = 1;
    EA = 1;
}

// Ham doc 1 byte SPI slave
unsigned char SoftSPI_ReadByte()
{
    unsigned char i, _data = 0x00;
    for (i = 0; i < 8; i++)
    {
        _data <<= 1;
        while (SPI_SCLK == 0)
            ;
        if (SPI_MOSI == 1)
        {
            _data |= 0x01;
        }
        while (SPI_SCLK == 1)
            ;
    }
    return _data;
}

// Ham gui 1 byte SPI slave
void SoftSPI_WriteByte(unsigned char _data)
{
    unsigned char i;
    for (i = 0; i < 8; i++)
    {
        if ((_data & 0x80) != 0)
        {
            SPI_MISO = 1;
        }
        else
        {
            SPI_MISO = 0;
        }
        _data = _data << 1;
        while (SPI_SCLK == 0)
            ;
        while (SPI_SCLK == 1)
            ;
    }
    SPI_MISO = 1;
}

void ex0_isr(void) interrupt 0
{
    unsigned char spiData;
    spiData = SoftSPI_ReadByte();
    if (spiData == 0xAA)
    {
        DS18B20_ReadTemperature();
        SoftSPI_WriteByte(DS18B20_buffer[0]);
        SoftSPI_WriteByte(DS18B20_buffer[1]);
    }
    if (spiData == 0x01)
    {
        LIGHT1 = ~LIGHT1;
    }
    if (spiData == 0x02)
    {
        LIGHT2 = ~LIGHT2;
    }
}

void main()
{
    SoftSPI_Init();
    // DS18B20_Init();
    LIGHT1 = 0;
    LIGHT2 = 0;
    while (1)
    {
        
    }
}