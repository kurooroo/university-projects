#include <stm8s.h>
#include <stdio.h>

#define _2CM_DISTANCE 200 // Hieu chuan dua tren thuc te

//LCD
#define clear_display                                        0x01
#define goto_home                                            0x02
#define cursor_direction_inc                         (0x04 | 0x02)
#define cursor_direction_dec                         (0x04 | 0x00)
#define display_shift                                (0x04 | 0x01)
#define display_no_shift                             (0x04 | 0x00)
#define display_on                                   (0x08 | 0x04)
#define display_off                                  (0x08 | 0x02)
#define cursor_on                                    (0x08 | 0x02)
#define cursor_off                                   (0x08 | 0x00)
#define blink_on                                     (0x08 | 0x01)
#define blink_off                                    (0x08 | 0x00)
#define _8_pin_interface                             (0x20 | 0x10)
#define _4_pin_interface                             (0x20 | 0x00)
#define _2_row_display                               (0x20 | 0x08)
#define _1_row_display                               (0x20 | 0x00)
#define _5x10_dots                                   (0x20 | 0x40)
#define _5x7_dots                                    (0x20 | 0x00)
#define BL_ON                                                    1
#define BL_OFF                                                  0
#define dly                                                     2
#define DAT                                                       1
#define CMD                                                       0
unsigned char bl_state;
unsigned char data_value;
void LCD_init(void);
void LCD_toggle_EN(void);
void LCD_send(unsigned char value, unsigned char mode);
void LCD_4bit_send(unsigned char lcd_data);           
void LCD_putstr(char *lcd_string);
void LCD_putchar(char char_data);
void LCD_clear_home(void);
void LCD_goto(unsigned char x_pos, unsigned char y_pos);
#define PCF8574_ADDRESS 0x4E


static char table[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
char _data[2];
uint16_t temp = 0;
float temperature;
char temperatureChar[6];
unsigned int distance = 0;
int i2ccheck = 0;

void Delay_us(uint32_t time_delay)
{
  while (time_delay--)
  {
  }
}

void Delay_ms(uint32_t time_delay)
{
  while (time_delay--)
  {
    Delay_us(200);
  }
}

/**********************Clock********************************/
void CLOCK_Setup(void)
{
  CLK_DeInit();
  CLK_HSECmd(DISABLE);
  CLK_LSICmd(DISABLE);
  CLK_HSICmd(ENABLE);
  while (CLK_GetFlagStatus(CLK_FLAG_HSIRDY) == FALSE)
    ;

  CLK_ClockSwitchCmd(ENABLE);
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV8);
  CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);

  CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI,
                        DISABLE, CLK_CURRENTCLOCKSTATE_ENABLE);

  CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, ENABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, ENABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_AWU, DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, ENABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, DISABLE);
}
/**********************Clock********************************/

/**********************UART********************************/
void UART1_Setup(void)
{
  UART1_DeInit();

  UART1_Init(9600,
             UART1_WORDLENGTH_8D,
             UART1_STOPBITS_1,
             UART1_PARITY_NO,
             UART1_SYNCMODE_CLOCK_DISABLE,
             UART1_MODE_TXRX_ENABLE);

  UART1_Cmd(ENABLE);
}

void UART1_Print(char string[])
{
  int i = 0;
  while (string[i] != 0x00)
  {
    UART1_SendData8(string[i]);
    while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET)
      ;
    i++;
  }
}

void UART1_Println(char string[])
{
  int i = 0;
  while (string[i] != 0x00)
  {
    UART1_SendData8(string[i]);
    while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET)
      ;
    i++;
  }
  UART1_SendData8('\n');
  //UART1_SendData8(0x0a);
}

void Float2String(char *str, double number, uint8_t intBit, uint8_t decinmalBit)
{
  uint8_t i;
  uint32_t temp = (uint8_t)number / 1;
  double t2 = 0.0;
  for (i = 1; i <= intBit; i++)
  {
    if (temp == 0)
    {
      str[intBit - i] = table[0];
    }
    else
    {
      str[intBit - i] = table[temp % 10];
      temp = temp / 10;
    }
  }
  *(str + intBit) = '.';
  temp = 0;
  t2 = number;
  for (i = 1; i <= decinmalBit; i++)
  {
    temp = t2 * 10;
    str[intBit + i] = table[temp % 10];
    t2 = t2 * 10;
  }
  *(str + intBit + decinmalBit + 1) = '\0';
}
/**********************UART********************************/

/**********************GPIO********************************/
void GPIO_Setup(void)
{
  GPIO_DeInit(GPIOB);
  GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST);
}
/**********************GPIO********************************/

/**********************I2C********************************/
void I2C_Setup()
{
  I2C_DeInit();
  I2C_Cmd(ENABLE);
  I2C_Init(25000,
           0xB4,
           I2C_DUTYCYCLE_2,
           I2C_ACK_CURR,
           I2C_ADDMODE_7BIT,
           (CLK_GetClockFreq() / 1000000));
}

char MLX90614_ReadTobj1(char *pBuffer)
{
  int timeOut = 0; // nếu giao tiếp lỗi thì bị timeOut và chương trình tiếp tục hoạt động, không bị treo
  int NumByteToRead = 2; // đọc 2 byte data từ MLX90614
  while (I2C_GetFlagStatus(I2C_FLAG_BUSBUSY)) // kiểm tra line i2c xem sẵn sàng không
  {
    timeOut++;
    if (timeOut > 1000)
    {
      return 0;
    }
  }
  I2C_GenerateSTART(ENABLE); // gửi tín hiệu băt đầu giao tiếp
  timeOut = 0;
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)) // kiểm tra xem đã chọn được mode master chưa
  {
    timeOut++;
    if (timeOut > 1000)
    {
      return 0;
    }
  }
  I2C_Send7bitAddress(0xB4, I2C_DIRECTION_TX);  // gửi địa chỉ slave, yêu cầu ghi
  timeOut = 0;
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) // đợi tín hiệu của slave
  {
    timeOut++;
    if (timeOut > 1000)
    {
      return 0;
    }
  }
  I2C_ClearFlag(I2C_FLAG_ADDRESSSENTMATCHED); // xoá cờ 
  I2C_SendData(0x07); // gửi bản tin địa chỉ ram cần đọc (nhiệt độ Tobj)
  timeOut = 0;
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED)) // đợi salve phản hồi
  {
    timeOut++;
    if (timeOut > 1000)
    {
      return 0;
    }
  }
  I2C_GenerateSTART(ENABLE); // gửi tín hiệu repeated start cho slave
  timeOut = 0;
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)) // kiểm tra xem đã chọn mode master chưa
  {
    timeOut++;
    if (timeOut > 1000)
    {
      return 0;
    }
  }
  I2C_Send7bitAddress(0xB4, I2C_DIRECTION_RX);  // gửi địa chỉ slave, yêu cầu đọc
  timeOut = 0;
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) // đợi slave trả lời
  {
    timeOut++;
    if (timeOut > 1000)
    {
      return 0;
    }
  }
  I2C_ClearFlag(I2C_FLAG_ADDRESSSENTMATCHED); // xoá cờ 
  timeOut = 0;
  while (NumByteToRead) // đợi đến khi đọc đủ 2 byte
  {
    if (NumByteToRead == 1) // nếu chỉ còn phải nhận 1 byte
    {
      I2C_AcknowledgeConfig(I2C_ACK_NONE); //bỏ tín hiệu ack sau khi nhận byte cuối cùng
      I2C_GenerateSTOP(ENABLE); // gửi tín hiệu stop ngừng giao tiếp
    }
    if (I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED))   // kiểm tra nếu có dữ liệu đến
    {
      *pBuffer = I2C_ReceiveData(); // đọc dữ liệu
      pBuffer++;
      NumByteToRead--;
    }
    {
      timeOut++;
      if (timeOut > 1000)
      {
        return 0;
      }
    }
  }
  I2C_AcknowledgeConfig(I2C_ACK_CURR); // đưa chế độ ack về gửi ack sau khi nhận dữ liệu để giao tiếp lần sau
  return 1;
}
/**********************I2C********************************/

/**********************ADC********************************/
void ADC_Setup(void)
{
  ADC1_DeInit();
  ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS,
            ADC1_CHANNEL_2,
            ADC1_PRESSEL_FCPU_D8,
            ADC1_EXTTRIG_TIM,
            DISABLE,
            ADC1_ALIGN_RIGHT,
            ADC1_SCHMITTTRIG_ALL,
            DISABLE);
  ADC1_Cmd(ENABLE);
}

unsigned int ADC_ReadAdc(void)
{
  unsigned int ret;
  ADC1_StartConversion();
  while (ADC1_GetFlagStatus(ADC1_FLAG_EOC) == FALSE)
    ;
  ret = ADC1_GetConversionValue();
  ADC1_ClearFlag(ADC1_FLAG_EOC);
  return ret;
}
/**********************ADC********************************/

/**********************LCD********************************/
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
  I2C_Send7bitAddress(PCF8574_ADDRESS, I2C_DIRECTION_RX);
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
  i2ccheck = 4;
  // while (I2C_GetFlagStatus(I2C_FLAG_BUSBUSY))
  // {
  //   timeOut++;
  //   if (timeOut > 1000)
  //   {
  //     UART1_Print("1");
  //     break;
  //   }
  // }
  I2C_GenerateSTART(ENABLE);
  timeOut = 0;
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT))
  {
    timeOut++;
    if (timeOut > 1000)
    {
      i2ccheck =1 ;
      break;
    }
  }
  I2C_Send7bitAddress(0x4E, I2C_DIRECTION_TX);
  timeOut = 0;
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    timeOut++;
    if (timeOut > 1000)
    {
      i2ccheck = 2;
      break;
    }
  }
  I2C_ClearFlag(I2C_FLAG_ADDRESSSENTMATCHED);
  I2C_SendData(data_byte); /* MSB */
  timeOut = 0;
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    timeOut++;
    if (timeOut > 1000)
    {
      i2ccheck = 3;
      break;
    }
  }
  while (!I2C_GetFlagStatus(I2C_FLAG_TRANSFERFINISHED))
  {
    timeOut++;
    if (timeOut > 1000)
    {
      i2ccheck = 3;
      break;
    }
  }
  I2C_GenerateSTOP(ENABLE);
  I2C->SR1;
  I2C->SR3;
}

void LCD_init(void)
{                      
    Delay_ms(100);
    bl_state = BL_ON;
    data_value = 0x04;
    PCF8574_write(data_value);
    Delay_ms(10);
    LCD_send(0x33, CMD);
    LCD_send(0x32, CMD);
    LCD_send((_4_pin_interface | _2_row_display | _5x7_dots), CMD);        
    LCD_send((display_on | cursor_off | blink_off), CMD);    
    LCD_send((clear_display), CMD);        
    LCD_send((cursor_direction_inc | display_no_shift), CMD);       
}  
void LCD_toggle_EN(void)
{
    data_value |= 0x04;
    PCF8574_write(data_value);
    Delay_ms(dly);
    data_value &= 0xF9;
    PCF8574_write(data_value);
    Delay_ms(dly);
}
void LCD_send(unsigned char value, unsigned char mode)
{
    switch(mode)
    {
       case CMD:
       {
            data_value &= 0xF4;
            break;
       }
       case DAT:
       {
           data_value |= 0x01;
           break;
       }
    }
    switch(bl_state)
    {
       case BL_ON:
       {
          data_value |= 0x08;
          break;
       }
       case BL_OFF:
       {
          data_value &= 0xF7;
          break;
       }
    }
    PCF8574_write(data_value);
    LCD_4bit_send(value);
    Delay_ms(dly);
}
void LCD_4bit_send(unsigned char lcd_data)      
{
    unsigned char temp = 0x00;
    temp = (lcd_data & 0xF0);
    data_value &= 0x0F;
    data_value |= temp;
    PCF8574_write(data_value);
    LCD_toggle_EN();
    temp = (lcd_data & 0x0F);
    temp <<= 0x04;
    data_value &= 0x0F;
    data_value |= temp;
    PCF8574_write(data_value);
    LCD_toggle_EN();
} 
void LCD_putstr(char *lcd_string)
{
    do
    {
        LCD_putchar(*lcd_string++);
    }while(*lcd_string != '\0') ;
}
void LCD_putchar(char char_data)
{
    if((char_data >= 0x20) && (char_data <= 0x7F))
    {
       LCD_send(char_data, DAT);
    }
}
void LCD_clear_home(void)
{
    LCD_send(clear_display, CMD);
    LCD_send(goto_home, CMD);
}
void LCD_goto(unsigned char x_pos,unsigned char y_pos)
{                                                   
    if(y_pos == 0)   
    {                             
        LCD_send((0x80 | x_pos), CMD);
    }
    else
    {                                             
        LCD_send((0x80 | 0x40 | x_pos), CMD);
    }
}

// Hien thi trang thai len LCD. _status = 1 thi hien thi ready de do nhiet do.
// _status = 0 thi hien thi nhiet do dang do duoc
void LCD_Display(char _status)
{
  LCD_clear_home();
  if (_status == 0)
  {
    LCD_goto(0, 0);
    LCD_putstr("Nhiet do: ");
    LCD_goto(0,1);
    LCD_putstr(temperatureChar);
    LCD_putstr(" do C");
  }
  if (_status == 1)
  {
    LCD_goto(0, 0);
    LCD_putstr("Thiet bi san");
    LCD_goto(0, 1);
    LCD_putstr("sang do nhiet do");
  }
}
/**********************LCD********************************/

/**********************main********************************/
int main(void)
{
  CLOCK_Setup();
  GPIO_Setup();
  UART1_Setup();
  I2C_Setup();
  ADC_Setup();
  LCD_init();
  while (1)
  {
    MLX90614_ReadTobj1(_data);
    temp = _data[1];
    temp = temp << 8;
    temp += _data[0];
    temperature = temp * 0.02;
    temperature -= 273.15;
    Float2String(temperatureChar, temperature, 2, 2);
    distance = ADC_ReadAdc();
    if (distance < _2CM_DISTANCE)
    {
      UART1_Print("Nhiet do: ");
      UART1_Print(temperatureChar);
      UART1_Println(" do C");
      LCD_Display(0);
      Delay_ms(1000);
    }
    //else
    //{
      //UART1_Println("Thiet bi san sang do nhiet do");
      //LCD_Display(1);
    //}
  }
}
