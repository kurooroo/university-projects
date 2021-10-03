/*
Do nhiet do dung LM35 va ADC0808 hoac ADC0809
Code by Vu Van Thai
wWw.SangTaoClub.Net
*/
#include <REGX51.H>
#define VREF			5 //VREF=5V
//Khai bao chan giao tiep ADC0808
#define ADC0808_DATA 	P3	//PORT
#define ADC0808_A  		P2_0 //PIN
#define ADC0808_B  		P2_1
#define ADC0808_C  		P2_2
#define ADC0808_ALE   	P2_3
#define ADC0808_START  	P2_4
#define ADC0808_EOC   	P2_5
#define ADC0808_OE   	P2_6
#define ADC0808_CLK		P2_7
//Khai bao chan giao tiep LCD16x2 4bit
#define LCD_RS P0_0
#define LCD_RW P0_1
#define LCD_EN P0_2
#define LCD_D4 P0_4
#define LCD_D5 P0_5
#define LCD_D6 P0_6
#define LCD_D7 P0_7
/*****************ADC0808*********************/
//Ham doc ADC0808 theo kenh
unsigned char ADC0808_Read(unsigned char channel){
	unsigned char kq;
 	ADC0808_A = channel & 0x01;
	ADC0808_B = channel & 0x02;
	ADC0808_C = channel & 0x04;
	ADC0808_ALE = 1;
	ADC0808_START = 1;
	ADC0808_ALE = 0;
	ADC0808_START = 0;
	while(ADC0808_EOC);
	while(!ADC0808_EOC);
	ADC0808_OE = 1;
	kq = ADC0808_DATA;
	ADC0808_OE = 0;
	return kq;
}
/*****************Ham delay*********************/
 void delay_us(unsigned int t){
        unsigned int i;
        for(i=0;i<t;i++);
}
void delay_ms(unsigned int t){
        unsigned int i,j;
        for(i=0;i<t;i++)
        for(j=0;j<125;j++);
}
/**************Ctr giao tiep LCD 16x2 4bit**********************/
void LCD_Enable(void){
        LCD_EN =1;
        delay_us(3);
        LCD_EN=0;
        delay_us(50); 
}
//Ham Gui 4 Bit Du Lieu Ra LCD
void LCD_Send4Bit(unsigned char Data){
        LCD_D4=Data & 0x01;
        LCD_D5=(Data>>1)&1;
        LCD_D6=(Data>>2)&1;
        LCD_D7=(Data>>3)&1;
}
// Ham Gui 1 Lenh Cho LCD
void LCD_SendCommand(unsigned char command){
        LCD_Send4Bit(command >>4);/* Gui 4 bit cao */
        LCD_Enable();
        LCD_Send4Bit(command); /* Gui 4 bit thap*/
        LCD_Enable();
}
void LCD_Clear(){// Ham Xoa Man Hinh LCD
        LCD_SendCommand(0x01); 
        delay_us(10);
}
// Ham Khoi Tao LCD
void LCD_Init(){
        LCD_Send4Bit(0x00);
        delay_ms(20);
        LCD_RS=0;
        LCD_RW=0;
        LCD_Send4Bit(0x03);
        LCD_Enable();
        delay_ms(5);
        LCD_Enable();
        delay_us(100);
        LCD_Enable();
        LCD_Send4Bit(0x02);
        LCD_Enable();
        LCD_SendCommand( 0x28 ); // giao thuc 4 bit, hien thi 2 hang, ki tu 5x8
        LCD_SendCommand( 0x0c); // cho phep hien thi man hinh
        LCD_SendCommand( 0x06 ); // tang ID, khong dich khung hinh
        LCD_SendCommand(0x01); // xoa toan bo khung hinh
}
void LCD_Gotoxy(unsigned char x, unsigned char y){
        unsigned char address;
        if(!y)address=(0x80+x);
        else address=(0xc0+x);
        delay_us(1000);
        LCD_SendCommand(address);
        delay_us(50);
}
void LCD_PutChar(unsigned char Data){//Ham Gui 1 Ki Tu
        LCD_RS=1;
        LCD_SendCommand(Data);
        LCD_RS=0 ;
}
void LCD_Puts(char *s){//Ham gui 1 chuoi ky tu
        while (*s){
                LCD_PutChar(*s);
                s++;
        }
}
/**************Show Temp***********************
unsigned char Temp(unsigned char adc){//Tinh nhiet do tu adc8bit
	 return ((VREF*adc)/2.56f); //Tinh nhiet do 
}  */
void TempShow(unsigned char z){ //Chuyen doi hien thi
     LCD_Puts("Nhiet do: ");
     LCD_PutChar((z/100)+48);//Tram
     LCD_PutChar((z%100/10)+48);//Chuc
     LCD_PutChar((z%10)+48);//Don vi 
     LCD_Puts("'C");
}
/******************Ctr ngat timer 0**************************/
void INT_Timer0()interrupt 1 {
     //ctr phuc vu ngat tao xung clock cho ADC0808
     ADC0808_CLK=~ADC0808_CLK; //Dao bit
}
/******************Ctr chinh**************************/
unsigned char temp;
void main(){
	//Tao xung clock cho ADC0808 dung ngat timer
	TMOD=0x02; //Timer0 8bit tu nap lai
	TH0=TL0=236;//Tao ngat 20us
	TR0=1;//Khoi dong timer0
    ET0=1;//Ngat timer0
    EA=1;//Cho phep ngat cuc bo
	//init	
	LCD_Init();//Khoi tao LCD
    delay_ms(200);
	LCD_Puts("Do nhiet do...");//Gui chuoi len LCD
    delay_ms(500);
    LCD_Clear();//Xoa man hinh
	LCD_Gotoxy(0,1);
	LCD_Puts("SangTaoClub.Net");
	while(1){
		  LCD_Gotoxy(0,0);
		  temp=ADC0808_Read(0); //Doc ADC0
		  //temp=Temp(temp); //Tinh nhiet do
		  TempShow(temp); //Hien thi nhiet do
		  delay_ms(500);//0.5s doc mot lan
	}
}//THE END

