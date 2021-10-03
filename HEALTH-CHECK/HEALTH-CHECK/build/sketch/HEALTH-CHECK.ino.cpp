#include <Arduino.h>
#line 1 "d:\\tunaOneDrive\\OneDrive - Hanoi University of Science and Technology\\tunaOneDriveSync\\tunaProjects\\Mercenary-projects\\HEALTH-CHECK\\HEALTH-CHECK\\HEALTH-CHECK.ino"
// Thu vien doc cam bien nhiet do DS18B20
#include <OneWire.h>
#include <DallasTemperature.h>

#define BUZZER_PIN 6     // pin dieu khien coi
#define LED_PIN 5        // pin dieu khien led
#define SOS_BUTTON_PIN 2 // pin doc nut nhan sos
#define MOTOR_PIN 3      // pin dieu khien dong co rung
#define LM35_PIN A0      // pin doc cam bien nhiet do
// #define _5_PHUT 5 * 60 * 1000 // thoi gian 5 phut thuc te
// #define _1_PHUT 1 * 60 * 1000 // thoi gian 1 phut thuc te
#define _5_PHUT 10000 // thoi gian 5 phut cho mo phong
#define _1_PHUT 2000  // thoi gian 1 phut  cho mo phong

int tinhTrangSucKhoe = 0; // 0 = binh thuong, 1 = canh bao, 2 = nguy hiem
bool coNhietDoCanhBao = false;
bool SOS_Status = 0;
unsigned long rungBinhThuong = 0;
unsigned long thoiDiemNhietDoBatDauCao = 0;
const int NHIET_DO_CAO = 38;           // neu nhiet do > 40.0 trong 1 phut thi bao rung
const int NHIET_DO_NGUY_HIEM_CAO = 40; // neu nhiet do > 45.0 thi bao SOS
const int NHIET_DO_NGUY_HIEM_THAP = 36;

// Ham doc nhiet do tu cam bien
#line 25 "d:\\tunaOneDrive\\OneDrive - Hanoi University of Science and Technology\\tunaOneDriveSync\\tunaProjects\\Mercenary-projects\\HEALTH-CHECK\\HEALTH-CHECK\\HEALTH-CHECK.ino"
float LM35_Read();
#line 35 "d:\\tunaOneDrive\\OneDrive - Hanoi University of Science and Technology\\tunaOneDriveSync\\tunaProjects\\Mercenary-projects\\HEALTH-CHECK\\HEALTH-CHECK\\HEALTH-CHECK.ino"
void SOS();
#line 48 "d:\\tunaOneDrive\\OneDrive - Hanoi University of Science and Technology\\tunaOneDriveSync\\tunaProjects\\Mercenary-projects\\HEALTH-CHECK\\HEALTH-CHECK\\HEALTH-CHECK.ino"
void setup();
#line 63 "d:\\tunaOneDrive\\OneDrive - Hanoi University of Science and Technology\\tunaOneDriveSync\\tunaProjects\\Mercenary-projects\\HEALTH-CHECK\\HEALTH-CHECK\\HEALTH-CHECK.ino"
void loop();
#line 25 "d:\\tunaOneDrive\\OneDrive - Hanoi University of Science and Technology\\tunaOneDriveSync\\tunaProjects\\Mercenary-projects\\HEALTH-CHECK\\HEALTH-CHECK\\HEALTH-CHECK.ino"
float LM35_Read()
{
  float ret = 0;
  float adcValude = analogRead(LM35_PIN);
  ret = adcValude * 500 / 1024;
  Serial.println(ret);
  return ret;
}

//Ham ngat nut nhan sos
void SOS()
{
  SOS_Status = !SOS_Status;
  if (SOS_Status == 1)
  {
    digitalWrite(BUZZER_PIN, LOW);
  }
  else
  {
    digitalWrite(BUZZER_PIN, HIGH);
  }
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  pinMode(MOTOR_PIN, OUTPUT);
  digitalWrite(MOTOR_PIN, LOW);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, HIGH);
  pinMode(SOS_BUTTON_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(SOS_BUTTON_PIN), SOS, FALLING);
  delay(1000);
}

void loop()
{
  // put your main code here, to run repeatedly:
  // Giam sat tinh trang suc khoe
  unsigned long hienTai = millis();
  int nhietDoHienTai = (int)LM35_Read();
  if ((nhietDoHienTai < NHIET_DO_NGUY_HIEM_THAP) || (nhietDoHienTai > NHIET_DO_NGUY_HIEM_CAO))
  {
    tinhTrangSucKhoe = 2;
  }
  if ((nhietDoHienTai >= NHIET_DO_NGUY_HIEM_THAP) && (nhietDoHienTai <= NHIET_DO_CAO))
  {
    tinhTrangSucKhoe = 0;
    coNhietDoCanhBao = false;
  }
  if ((nhietDoHienTai > NHIET_DO_CAO) && (tinhTrangSucKhoe != 2))
  {
    if (!coNhietDoCanhBao)
    {
      thoiDiemNhietDoBatDauCao = hienTai;
      coNhietDoCanhBao = true;
    }
    if (hienTai - thoiDiemNhietDoBatDauCao > _1_PHUT)
    {
      tinhTrangSucKhoe = 1;
    }
  }

  // Dua ra canh bao
  if (tinhTrangSucKhoe == 0)
  {
    // xu ly khi tinh trang suc khoe binh thuong
    if (hienTai - rungBinhThuong > _5_PHUT)
    {
      rungBinhThuong = hienTai;
      analogWrite(MOTOR_PIN, 150);
      delay(1000);
      analogWrite(MOTOR_PIN, 0);
      delay(1000);
    }
    digitalWrite(LED_PIN, 0);
    analogWrite(MOTOR_PIN, 0);
  }
  if (tinhTrangSucKhoe == 1)
  {
    // xu ly khi tinh trang sung khoe can canh bao
    analogWrite(MOTOR_PIN, 150);
  }
  // xu ly khi tinh trang suc khoe nguy hiem, dong co rung manh, den led bao dong sang bao SOS
  if (tinhTrangSucKhoe == 2)
  {
    digitalWrite(LED_PIN, HIGH);
    analogWrite(MOTOR_PIN, 255);
    delay(2000);
    analogWrite(MOTOR_PIN, 0);
    delay(1000);
  }
}

