#include <REGX52.H>
#include <stdio.h>
#include <intrins.h>

// Khai bao SPI
#define SPI_SCLK P2_0
#define SPI_MOSI P2_1
#define SPI_MISO P2_2
#define SPI_SS0 P2_3
#define SPI_SS1 P2_4
#define DS18B20_DATA P2_5

// Bien toan cuc luu du lieu cam bien nhiet do
unsigned char temperatureBuffer[2];

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
    temperatureBuffer[0] = DS18B20_ReadByte();
    temperatureBuffer[1] = DS18B20_ReadByte();
}

// Cau hinh UART baudrate 2400
void UART_Init()
{
    SCON = 0x50; // Chon mode uart 1
    TMOD = 0x20;
    TH1 = 0xf3; // lua chon baudrate 2400
    TR1 = 1;    // Khoi dong timer 1
    TI = 1;     // Cho phep gui du lieu
    REN = 1;    // Cho phep nhan du lieu
    ES = 1;     // Cho phep ngat serial
    EA = 1;
}

// Ham doc 1 byte UART
unsigned char UART_ReadByte()
{
    unsigned char _data;
    while (!RI)
        ;
    _data = SBUF;
    RI = 0;
    return _data;
}

// Ham gui 1 byte UART
void UART_WriteByte(unsigned char _data)
{
    SBUF = _data;
    while (!TI)
        ;
    TI = 0;
}

// Ham gui 1 string UART
void UART_Print(unsigned char *outString)
{
    char i = 0;
    while (outString[i] != '\0')
    {
        UART_WriteByte(outString[i]);
        i++;
    }
}

// Ham gui 1 string UART co xuong dong
void UART_Println(unsigned char *outString)
{
    char i = 0;
    while (outString[i] != '\0')
    {
        UART_WriteByte(outString[i]);
        i++;
    }
    UART_WriteByte('\n');
    UART_WriteByte('\r');
}

// Ham khoi tao SPI Master
void SoftSPI_Init(void)
{
    SPI_SS0 = 1;
    SPI_SS1 = 1;
    SPI_SCLK = 0;
    SPI_MISO = 1;
}

// Ham gui 1 byte SPI
void SoftSPI_WriteByte(unsigned char _data)
{
    unsigned char i;
    for (i = 0; i < 8; i++)
    {
        if ((_data & 0x80) != 0)
        {
            SPI_MOSI = 1;
        }
        else
        {
            SPI_MOSI = 0;
        }
        _nop_();
        _nop_();
        _nop_();
        // DelayMs(100);
        SPI_SCLK = 1;
        _nop_();
        _nop_();
        _nop_();
        // DelayMs(100);
        SPI_SCLK = 0;
        _data = _data << 1;
    }
}

// Ham nhan 1 byte SPI
unsigned char SoftSPI_ReadByte()
{
    unsigned char i, _data = 0x00;
    for (i = 0; i < 8; i++)
    {
        _data <<= 1;
        SPI_SCLK = 1;
        _nop_();
        _nop_();
        _nop_();
        if (SPI_MISO)
        {
            _data |= 0x01;
        }
        SPI_SCLK = 0;
        _nop_();
        _nop_();
        _nop_();
    }
    return _data;
}


// Ngat Serial
void serial_interrupt(void) interrupt 4
{
    unsigned char uartData;
    ES = 0;
    if (TI == 1)
    {
        TI = 0;
    }
    else
    {
        uartData = UART_ReadByte();
        switch (uartData)
        {
        case 49:
            SPI_SS0 = 0;
            DelayMs(1);
            SoftSPI_WriteByte(0x01);
            DelayMs(1);
            SPI_SS0 = 1;
            break;
        case 50:
            SPI_SS0 = 0;
            DelayMs(1);
            SoftSPI_WriteByte(0x02);
            DelayMs(1);
            SPI_SS0 = 1;
            break;
        case 51:
            SPI_SS1 = 0;
            DelayMs(1);
            SoftSPI_WriteByte(0x01);
            DelayMs(1);
            SPI_SS1 = 1;
            break;
        case 52:
            SPI_SS1 = 0;
            DelayMs(1);
            SoftSPI_WriteByte(0x02);
            DelayMs(1);
            SPI_SS1 = 1;
            break;
        }
    }
    ES = 1;
}

// Ham doc nhiet do tu cac slave
void ReadTemperature(unsigned char slave)
{
    if (slave == 0)
    {
        SPI_SS0 = 0;
        DelayMs(1);
        SoftSPI_WriteByte(0xAA);
        DelayMs(100);
        temperatureBuffer[0] = SoftSPI_ReadByte();
        DelayMs(1);
        temperatureBuffer[1] = SoftSPI_ReadByte();
        DelayMs(1);
        SPI_SS0 = 1;
    }
    if (slave == 1)
    {
        SPI_SS1 = 0;
        DelayMs(1);
        SoftSPI_WriteByte(0xAA);
        DelayMs(100);
        temperatureBuffer[0] = SoftSPI_ReadByte();
        DelayMs(1);
        temperatureBuffer[1] = SoftSPI_ReadByte();
        DelayMs(1);
        SPI_SS1 = 1;
    }
}

void main()
{
    unsigned char header[] = "Nhiet do ";
    unsigned char headerMaster[] = "Master: ";
    unsigned char headerStr0[] = "Slave 0: ";
    unsigned char headerStr1[] = "Slave 1: ";
    unsigned char tempStr[6];
    signed int temp;
    float currentTemp;
    UART_Init();
    UART_WriteByte(0);
    SoftSPI_Init();
    SoftSPI_WriteByte(0);
    DelayMs(500);
    while (1)
    {
        DS18B20_ReadTemperature();
        // Xu ly du lieu nhiet do
        temp = temperatureBuffer[0];
        temp &= 0x00ff;
        temp |= (temperatureBuffer[1] << 8);
        currentTemp = temp * 0.0625;
        sprintf(tempStr, "%.2f", currentTemp);
        UART_Print(header);
        UART_Print(headerMaster);
        UART_Println(tempStr);

        ReadTemperature(0);
        temp = temperatureBuffer[0];
        temp &= 0x00ff;
        temp |= (temperatureBuffer[1] << 8);
        currentTemp = temp * 0.0625;
        sprintf(tempStr, "%.2f", currentTemp);
        UART_Print(header);
        UART_Print(headerStr0);
        UART_Println(tempStr);

        ReadTemperature(1);
        temp = temperatureBuffer[0];
        temp &= 0x00ff;
        temp |= (temperatureBuffer[1] << 8);
        currentTemp = temp * 0.0625;
        sprintf(tempStr, "%.2f", currentTemp);
        UART_Print(header);
        UART_Print(headerStr1);
        UART_Println(tempStr);
        DelayMs(500);
    }
}