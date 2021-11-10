#include "stm32f4xx.h"                  // Device header
#include "i2c_lcd.h"

void delay_ms(uint32_t u32DelayInMs);
void delay_us(uint32_t delay);

void delay_us(uint32_t delay)
{
	
	TIM_SetCounter(TIM6, 0);
	while (TIM_GetCounter(TIM6) < delay) {
	}
}

void delay_ms(uint32_t u32DelayInMs)
{
	
	while (u32DelayInMs) {
		/*
		TIM6->CNT = 0;
		while (TIM6->CNT < 1000) {
		}
		*/
		TIM_SetCounter(TIM6, 0);
		while (TIM_GetCounter(TIM6) < 1000) {
		}
		--u32DelayInMs;
	}
}

void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
uint8_t i2c_write(uint8_t u8Data);
uint8_t i2c_read(uint8_t u8Ack);

#define SDA_0 GPIO_ResetBits(GPIOA, GPIO_Pin_1)
#define SDA_1 GPIO_SetBits(GPIOA, GPIO_Pin_1)
#define SCL_0 GPIO_ResetBits(GPIOA, GPIO_Pin_2)
#define SCL_1 GPIO_SetBits(GPIOA, GPIO_Pin_2)
#define SDA_VAL (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1))

void i2c_init(void)
{
	GPIO_InitTypeDef gpioInit;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	gpioInit.GPIO_Mode = GPIO_Mode_OUT;
	gpioInit.GPIO_OType = GPIO_OType_OD;
	gpioInit.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
	gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpioInit.GPIO_Speed = GPIO_High_Speed;
	
	GPIO_Init(GPIOA, &gpioInit);
	
	SDA_1;
	SCL_1;
}

void i2c_start(void)
{
	
	SCL_1;
	delay_us(3);
	SDA_1;
	delay_us(3);
	SDA_0;
	delay_us(3);
	SCL_0;
	delay_us(3);
}

void i2c_stop(void)
{
	
	SDA_0;
	delay_us(3);
	SCL_1;
	delay_us(3);
	SDA_1;
	delay_us(3);
}

uint8_t i2c_write(uint8_t u8Data)
{
	uint8_t i;
	uint8_t u8Ret;
	
	for (i = 0; i < 8; ++i) {
		if (u8Data & 0x80) {
			SDA_1;
		} else {
			SDA_0;
		}
		delay_us(3);
		SCL_1;
		delay_us(5);
		SCL_0;
		delay_us(2);
		u8Data <<= 1;
	}
	
	SDA_1;
	delay_us(3);
	SCL_1;
	delay_us(3);
	if (SDA_VAL) {
		u8Ret = 0;
	} else {
		u8Ret = 1;
	}
	delay_us(2);
	SCL_0;
	delay_us(5);
	
	return u8Ret;
}

uint8_t i2c_read(uint8_t u8Ack)
{
	uint8_t i;
	uint8_t u8Ret;
	
	SDA_1;
	delay_us(3);
	
	for (i = 0; i < 8; ++i) {
		u8Ret <<= 1;
		SCL_1;
		delay_us(3);
		if (SDA_VAL) {
			u8Ret |= 0x01;
		}
		delay_us(2);
		SCL_0;
		delay_us(5);
	}
	
	if (u8Ack) {
		SDA_0;
	} else {
		SDA_1;
	}
	delay_us(3);
	
	SCL_1;
	delay_us(5);
	SCL_0;
	delay_us(5);
	
	return u8Ret;
}

int main(void)
{	
	GPIO_InitTypeDef gpioInit;
	TIM_TimeBaseInitTypeDef timInit;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
	gpioInit.GPIO_Mode = GPIO_Mode_OUT;
	gpioInit.GPIO_OType = GPIO_OType_PP;
	gpioInit.GPIO_Pin = GPIO_Pin_12;
	gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpioInit.GPIO_Speed = GPIO_High_Speed;
	
	GPIO_Init(GPIOD, &gpioInit);
	
	/*
	RCC->APB1ENR |= (1 << 4);
	TIM6->CR1 |= (1 << 1);
	TIM6->PSC = (84 - 1);
	TIM6->CR1 &= ~(1 << 1);
	TIM6->EGR = 1;
	TIM6->CR1 |= (1 << 0);
	*/
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	timInit.TIM_Prescaler = 84 - 1;
	timInit.TIM_Period = 0xFFFF;
	TIM_TimeBaseInit(TIM6, &timInit);
	TIM_Cmd(TIM6, ENABLE);
	
	I2C_LCD_Init();
	I2C_LCD_Clear();
	I2C_LCD_BackLight(1);
	I2C_LCD_Puts("STM32F407VGT6");
	I2C_LCD_NewLine();
	I2C_LCD_Puts("I2C: PA1 - PA2");
	
	while (1) {
		GPIO_SetBits(GPIOD, GPIO_Pin_12);
		delay_ms(100);
		GPIO_ResetBits(GPIOD, GPIO_Pin_12);
		delay_ms(200);
	}
}
