#include "PCF8574.h"
unsigned char PCF8574_read(void)
{
    unsigned char port_byte = 0x00;
    unsigned char num_of_bytes = 0x01;
    int timeOut = 0;
    while (I2C_GetFlagStatus(I2C_FLAG_BUSBUSY))
    {
        timeOut++;
        if (timeOut > 1000)
        {
            return 0;
        }
    }
    I2C_GenerateSTART(ENABLE);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT))
    {
        timeOut++;
        if (timeOut > 1000)
        {
            return 0;
        }
    }
    I2C_Send7bitAddress(PCF8574_address, I2C_DIRECTION_RX);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        timeOut++;
        if (timeOut > 1000)
        {
            return 0;
        }
    }
    while (num_of_bytes)
    {
        if (I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED))
        {
            if (num_of_bytes == 0)
            {
                I2C_AcknowledgeConfig(I2C_ACK_NONE);
                I2C_GenerateSTOP(ENABLE);
            }
            port_byte = I2C_ReceiveData();
            num_of_bytes--;
        }
        timeOut++;
        if (timeOut > 1000)
        {
            return 0;
        }
    };
    I2C_AcknowledgeConfig(I2C_ACK_CURR);
    return port_byte;
}
void PCF8574_write(unsigned char data_byte)
{
    int timeOut = 0;
    I2C_GenerateSTART(ENABLE);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT))
    {
        timeOut++;
        if (timeOut > 1000)
        {
            break;
        }
    }
    I2C_Send7bitAddress(PCF8574_address, I2C_DIRECTION_TX);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        timeOut++;
        if (timeOut > 1000)
        {
            break;
        }
    }
    I2C_SendData(data_byte);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        timeOut++;
        if (timeOut > 1000)
        {
            break;
        }
    }
    I2C_GenerateSTOP(ENABLE);
}