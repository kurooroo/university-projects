/*******************************************************
This program was created by the
CodeWizardAVR V3.12 Advanced
Automatic Program Generator
� Copyright 1998-2014 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 5/6/2021
Author  : 
Company : 
Comments: 


Chip type               : ATmega16
Program type            : Application
AVR Core Clock frequency: 8.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 256
*******************************************************/

#include <mega16.h>
// Standard Input/Output functions
#include <stdio.h>
#include <delay.h>

// Declare your global variables here
unsigned char G_temperature0 = 0;
unsigned char G_temperature1 = 0;

#define DATA_REGISTER_EMPTY (1<<UDRE)
#define RX_COMPLETE (1<<RXC)
#define FRAMING_ERROR (1<<FE)
#define PARITY_ERROR (1<<UPE)
#define DATA_OVERRUN (1<<DOR)

// USART Receiver buffer
#define RX_BUFFER_SIZE 8
char rx_buffer[RX_BUFFER_SIZE];

#if RX_BUFFER_SIZE <= 256
unsigned char rx_wr_index=0,rx_rd_index=0;
#else
unsigned int rx_wr_index=0,rx_rd_index=0;
#endif

#if RX_BUFFER_SIZE < 256
unsigned char rx_counter=0;
#else
unsigned int rx_counter=0;
#endif

// This flag is set on USART Receiver buffer overflow
bit rx_buffer_overflow;

// Hàm gửi data SPI
int SPI_Write(char slaveAddress, char data)
{
   switch(slaveAddress)
   {
      case 0:
         PORTB &= 0b11110111;
         SPDR = data;
         delay_ms(20);
         PORTB |= 0b00001000;
         return 1;
      case 1:
         PORTB &= 0b11101111;
         SPDR = data;
         delay_ms(20);
         PORTB |= 0b00010000;
         return 1;
   }
}

// Ham doc nhiet do
int SPI_ReadTemperature(char slaveAddress)
{
   switch(slaveAddress)
   {
      case 0:
         PORTB &= 0b11110111;
         SPDR = 0;
         delay_ms(2);
         SPDR = 0xff;
         delay_ms(2);
         G_temperature0 = SPDR;
         PORTB |= 0b00001000;
         return 1;
      case 1:
         PORTB &= 0b11101111;
         SPDR = 0;
         delay_ms(2);
         SPDR = 0xff;
         delay_ms(2);
         G_temperature1 = SPDR;
         PORTB |= 0b00010000;
         return 1;
   }
}

// Hàm gửi data UART
void UART_Write(char data)
{
   while((UCSRA & (1<< UDRE)) == 0);
   UDR = data;
}

// Hàm gửi string UART có xuống dòng
void UART_Println(char *string)
{
   char i = 0;
   while(string[i] != '\0')
   {
      UART_Write(string[i]);
      i++;
   }
   UART_Write('\n');
   UART_Write('\r');
}

// Hàm gửi string UART
void UART_Print(char *string)
{
   char i = 0;
   while(string[i] != '\0')
   {
      UART_Write(string[i]);
      i++;
   }
}

// Hàm ngắt nhận UART
interrupt [USART_RXC] void usart_rx_isr(void)
{
   char status,data; 
   status=UCSRA;
   data=UDR;
   if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
      {
      rx_buffer[rx_wr_index++]=data;
   #if RX_BUFFER_SIZE == 256
      // special case for receiver buffer size=256
      if (++rx_counter == 0) rx_buffer_overflow=1;
   #else
      if (rx_wr_index == RX_BUFFER_SIZE) rx_wr_index=0;
      if (++rx_counter == RX_BUFFER_SIZE)
         {
         rx_counter=0;
         rx_buffer_overflow=1;
         }
   #endif
      }
   switch (data)
   {
   case 49:
      SPI_Write(0, 1);
      break;
   case 50:
      SPI_Write(0, 2);
      break;
   case 51:
      SPI_Write(1, 1);
      break;
   case 52:
      SPI_Write(1, 2);
      break;
   default:
      break;
   }
}

#ifndef _DEBUG_TERMINAL_IO_
// Get a character from the USART Receiver buffer
#define _ALTERNATE_GETCHAR_
#pragma used+
char getchar(void)
{
char data;
while (rx_counter==0);
data=rx_buffer[rx_rd_index++];
#if RX_BUFFER_SIZE != 256
if (rx_rd_index == RX_BUFFER_SIZE) rx_rd_index=0;
#endif
#asm("cli")
--rx_counter;
#asm("sei")
return data;
}
#pragma used-
#endif

// SPI interrupt service routine
interrupt [SPI_STC] void spi_isr(void)
{
unsigned char data;
data=SPDR;
}

void main(void)
{
// Declare your local variables here

// Input/Output Ports initialization
// Port A initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRA=(0<<DDA7) | (0<<DDA6) | (0<<DDA5) | (0<<DDA4) | (0<<DDA3) | (0<<DDA2) | (0<<DDA1) | (0<<DDA0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTA=(0<<PORTA7) | (0<<PORTA6) | (0<<PORTA5) | (0<<PORTA4) | (0<<PORTA3) | (0<<PORTA2) | (0<<PORTA1) | (0<<PORTA0);

// Port B initialization
// Function: Bit7=Out Bit6=In Bit5=Out Bit4=Out Bit3=Out Bit2=In Bit1=In Bit0=In 
DDRB=(1<<DDB7) | (0<<DDB6) | (1<<DDB5) | (1<<DDB4) | (1<<DDB3) | (0<<DDB2) | (0<<DDB1) | (0<<DDB0);
// State: Bit7=0 Bit6=T Bit5=0 Bit4=0 Bit3=T Bit2=T Bit1=T Bit0=T 
PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);

// Port C initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRC=(0<<DDC7) | (0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTC=(0<<PORTC7) | (0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

// Port D initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRD=(0<<DDD7) | (0<<DDD6) | (0<<DDD5) | (0<<DDD4) | (0<<DDD3) | (0<<DDD2) | (0<<DDD1) | (0<<DDD0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTD=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
// Mode: Normal top=0xFF
// OC0 output: Disconnected
TCCR0=(0<<WGM00) | (0<<COM01) | (0<<COM00) | (0<<WGM01) | (0<<CS02) | (0<<CS01) | (0<<CS00);
TCNT0=0x00;
OCR0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: Timer1 Stopped
// Mode: Normal top=0xFFFF
// OC1A output: Disconnected
// OC1B output: Disconnected
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10);
TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (0<<CS11) | (0<<CS10);
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;

// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: Timer2 Stopped
// Mode: Normal top=0xFF
// OC2 output: Disconnected
ASSR=0<<AS2;
TCCR2=(0<<PWM2) | (0<<COM21) | (0<<COM20) | (0<<CTC2) | (0<<CS22) | (0<<CS21) | (0<<CS20);
TCNT2=0x00;
OCR2=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=(0<<OCIE2) | (0<<TOIE2) | (0<<TICIE1) | (0<<OCIE1A) | (0<<OCIE1B) | (0<<TOIE1) | (0<<OCIE0) | (0<<TOIE0);

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
// INT2: Off
MCUCR=(0<<ISC11) | (0<<ISC10) | (0<<ISC01) | (0<<ISC00);
MCUCSR=(0<<ISC2);

// USART initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART Receiver: On
// USART Transmitter: On
// USART Mode: Asynchronous
// USART Baud Rate: 9600
UCSRA=(0<<RXC) | (0<<TXC) | (0<<UDRE) | (0<<FE) | (0<<DOR) | (0<<UPE) | (0<<U2X) | (0<<MPCM);
UCSRB=(1<<RXCIE) | (0<<TXCIE) | (0<<UDRIE) | (1<<RXEN) | (1<<TXEN) | (0<<UCSZ2) | (0<<RXB8) | (0<<TXB8);
UCSRC=(1<<URSEL) | (0<<UMSEL) | (0<<UPM1) | (0<<UPM0) | (0<<USBS) | (1<<UCSZ1) | (1<<UCSZ0) | (0<<UCPOL);
UBRRH=0x00;
UBRRL=0x33;

// Analog Comparator initialization
// Analog Comparator: Off
// The Analog Comparator's positive input is
// connected to the AIN0 pin
// The Analog Comparator's negative input is
// connected to the AIN1 pin
ACSR=(1<<ACD) | (0<<ACBG) | (0<<ACO) | (0<<ACI) | (0<<ACIE) | (0<<ACIC) | (0<<ACIS1) | (0<<ACIS0);
SFIOR=(0<<ACME);

// ADC initialization
// ADC disabled
ADCSRA=(0<<ADEN) | (0<<ADSC) | (0<<ADATE) | (0<<ADIF) | (0<<ADIE) | (0<<ADPS2) | (0<<ADPS1) | (0<<ADPS0);

// SPI initialization
// SPI Type: Master
// SPI Clock Rate: 500.000 kHz
// SPI Clock Phase: Cycle Start
// SPI Clock Polarity: Low
// SPI Data Order: MSB First
SPCR=(1<<SPIE) | (1<<SPE) | (0<<DORD) | (1<<MSTR) | (0<<CPOL) | (0<<CPHA) | (0<<SPR1) | (1<<SPR0);
SPSR=(0<<SPI2X);

// Clear the SPI interrupt flag
#asm
    in   r30,spsr
    in   r30,spdr
#endasm

// TWI initialization
// TWI disabled
TWCR=(0<<TWEA) | (0<<TWSTA) | (0<<TWSTO) | (0<<TWEN) | (0<<TWIE);

// Global enable interrupts
#asm("sei")

SPI_Write(0, 0);
SPI_Write(1, 0);
delay_ms(1000);
while (1)
      {
      // Place your code here
      char str0[] = "Nhiet do slave 0: ";
      char str1[] = "Nhiet do slave 1: ";
      char strTemperature[5];
      SPI_ReadTemperature(0);
      sprintf(strTemperature, "%d", G_temperature0);
      UART_Print(str0);
      UART_Println(strTemperature);
      SPI_ReadTemperature(1);
      sprintf(strTemperature, "%d", G_temperature1);
      UART_Print(str1);
      UART_Println(strTemperature);
      delay_ms(500);
      }
}
