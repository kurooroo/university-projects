#include <REGX52.H>

// Khai bao chan giao tiep voi ADC0808
#define ADC0808_DATA P0
#define ADC0808_A P2_0
#define ADC0808_B P2_1
#define ADC0808_C P2_2
#define ADC0808_ALE P2_3
#define ADC0808_START P2_4
#define ADC0808_EOC P2_5
#define ADC0808_OE P2_6
#define ADC0808_CLK P2_7
#define BUZZER_PIN P1_0
#define LED_PIN P1_1
#define MOTOR_PIN P1_2
#define BUTTON_PIN P3_2

// #define _5_PHUT 5 * 60 * 1000000 // thoi gian 5 phut thuc te
// #define _1_PHUT 1 * 60 * 1000000 // thoi gian 1 phut thuc te
#define _5_PHUT 10000000 // thoi gian 5 phut cho mo phong (uS)
#define _1_PHUT 2000000  // thoi gian 1 phut  cho mo phong (uS)
const char NHIET_DO_CAO = 38;
const char NHIET_DO_NGUY_HIEM_CAO = 40;
const char NHIET_DO_NGUY_HIEM_THAP = 36;

unsigned long timeUs = 0; // thoi gian ke tu khi bat dau chuong trinh
unsigned long hienTai = 0; // thoi gian hien tai
unsigned char nhietDoHienTai;
unsigned char tinhTrangSucKhoe = 0; // 0 = binh thuong, 1 = canh bao, 2 = nguy hiem
// unsigned char canhBaoSOS = 0;
unsigned char coNhietDoCanhBao = 0;
unsigned long rungBinhThuong = 0;
unsigned long thoiDiemNhietDoBatDauCao = 0;

/*****************ADC0808*********************/
// Ham doc ADC0808 theo kenh
// Input: kenh ADC (0->8)
// Return: ket qua ADC
unsigned char ADC0808_Read(unsigned char f_channel)
{
    unsigned char ret;
    ADC0808_A = f_channel & 0x01;   //Chon kenh adc
    ADC0808_B = f_channel & 0x02;   
    ADC0808_C = f_channel & 0x04;
    ADC0808_ALE = 1;                
    ADC0808_START = 1;
    ADC0808_ALE = 0;                // Chot dia chi kenh adc
    ADC0808_START = 0;              // Bat dau chuyen doi
    while (ADC0808_EOC)
        ;
    while (!ADC0808_EOC)
        ;                           // Cho chuyen doi
    ADC0808_OE = 1;
    ret = ADC0808_DATA;             // Doc du lieu
    ADC0808_OE = 0;
    return ret;
}

/*****************Timer*********************/
// Cau hinh timer0 ngat moi 10us de tao xung clock tan so 12.5kHz cho ADC0808 va
// theo doi thoi gian de thuc hien chuong trinh
// Ham khoi tao timer0
void TIMER_Init(void)
{
    EA = 0;             //cam ngat toan cuc
    TMOD = TMOD | 0x02; //timer0 8 bit tu nap lai
    TH0 = 216;
    TL0 = 216; // tao ngat moi 40us
    TR0 = 1;   // khoi dong timer 0
    ET0 = 1;   // cho phep ngat timer 0
    EA = 1;    // cho phep ngat toan cuc
}

// Chuong trinh phuc vu ngat timer 0
void TIMER0_ISR() interrupt 1
{
    ADC0808_CLK = ~ADC0808_CLK; // dao bit de tao xung clock co duty 50%
    timeUs += 40; // cong them thoi gian vao bien luu tru
}
/*****************External Interrupt*********************/
// Khoi tao nut nhan co ngat canh suon xuong
void BUTTON_Init(void)
{
    EA = 0;
    IT0 = 1;
    EX0 = 1;
    EA = 1;
}
// Chuong trinh phuc vu ngat nut nhan
void BUTTON_Sos(void) interrupt 0
{
    BUZZER_PIN = !BUZZER_PIN; // Dao trang thai pin coi chip
}

void main(void)
{
    // UART_Init();
    MOTOR_PIN = 0;
    LED_PIN = 0;
    BUZZER_PIN = 1;
    BUTTON_Init();
    TIMER_Init();
    // cho 1s = 1000000us de cam bien on dinh
    hienTai = timeUs;
    while (timeUs - hienTai < 1000000)
        ;
    while (1)
    {
        // Doc nhiet do cam bien
        nhietDoHienTai = ADC0808_Read(0);
        // Giam sat tinh trang suc khoe
        hienTai = timeUs;
        if ((nhietDoHienTai < NHIET_DO_NGUY_HIEM_THAP) || (nhietDoHienTai > NHIET_DO_NGUY_HIEM_CAO))
        {
            tinhTrangSucKhoe = 2;
        }
        if ((nhietDoHienTai >= NHIET_DO_NGUY_HIEM_THAP) && (nhietDoHienTai <= NHIET_DO_CAO))
        {
            tinhTrangSucKhoe = 0;
            coNhietDoCanhBao = 0;
        }
        if ((nhietDoHienTai > NHIET_DO_CAO) && (tinhTrangSucKhoe != 2))
        {
            if (coNhietDoCanhBao == 0)
            {
                thoiDiemNhietDoBatDauCao = hienTai;
                coNhietDoCanhBao = 1;
            }
            if (hienTai - thoiDiemNhietDoBatDauCao > _1_PHUT)
            {
                tinhTrangSucKhoe = 1;
            }
        }
        // Dua ra canh bao
        if (tinhTrangSucKhoe == 0)
        {
            if (hienTai - rungBinhThuong > _5_PHUT)
            {
                rungBinhThuong = hienTai;
                MOTOR_PIN = 1;
                hienTai = timeUs;
                while (timeUs - hienTai < 500000)
                    ;
                MOTOR_PIN = 0;
            }
            LED_PIN = 0;
            MOTOR_PIN = 0;
        }
        if (tinhTrangSucKhoe == 1)
        {
            MOTOR_PIN = 1;
        }
        if (tinhTrangSucKhoe == 2)
        {
            LED_PIN = 1;
            MOTOR_PIN = 1;
            hienTai = timeUs;
            while (timeUs - hienTai < 1000000)
                ;
            MOTOR_PIN = 0;
            hienTai = timeUs;
            while (timeUs - hienTai < 2000000)
                ;
        }
    }
}