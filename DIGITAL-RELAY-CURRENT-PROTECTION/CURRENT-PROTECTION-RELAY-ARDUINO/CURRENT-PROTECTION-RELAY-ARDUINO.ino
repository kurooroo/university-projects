#include <math.h>
#include <LiquidCrystal.h>

#define ACS712_PIN A0
#define RELAY_PIN 8
#define RESET_BUTTON 9
#define _200_uSecond 200
#define Ip 1.0           // Dong dien dinh muc
#define I_NGAN_MACH 10.0 //Dong dien ngan mach
#define DAC_TINH_A 0.052
#define DAC_TINH_B 0.114
#define DAC_TINH_p 0.02
#define DAC_TINH_Tr 4.85
#define DAC_TINH_Tc 20000       // = 0.02s
#define CHU_KI_HIEN_THI 1000000 //1s cap nhan man hinh LCD 1 lan

unsigned long loopTime = 0;
unsigned long hienThi = 0;
unsigned long thoiGianTacDong = 0;
double tI;
double cI = 0;
double IRMS;
double IRMS_Print;

double ACS712_TrueRMS(void)
{
  int sampleNumber = 0;
  unsigned long adcSampleTime = micros();
  double currentSquare = 0;
  double IRMS = 0;
  while (sampleNumber < 50)
  {
    if (micros() - adcSampleTime > _200_uSecond)
    {
      adcSampleTime = micros();
      sampleNumber++;
      int ADC_read = analogRead(ACS712_PIN);
      currentSquare += (ADC_read - 512.0) * (ADC_read - 512.0);
    }
  }
  currentSquare *= (5000.0 / (1024.0 * 185.0)) * (5000.0 / (1024.0 * 185.0));
  // 0.93 la he so hieu chinh tu thuc te dong ho do
  IRMS = sqrt(currentSquare / sampleNumber) * 0.93;
  return IRMS;
}

// double ACS712_TrueRMS2(void)
// {
//   int sampleNumber = 0;
//   unsigned long adcSampleTime = micros();
//   double currentSquare = 0;
//   double IRMS = 0;
//   while (sampleNumber < 50)
//   {
//     if (micros() - adcSampleTime > _200_uSecond)
//     {
//       adcSampleTime = micros();
//       sampleNumber++;
//       int ADC_read = analogRead(ACS712_PIN);
//       currentSquare += abs(ADC_read - 512.0);
//     }
//   }
//   currentSquare *= (5000.0 / (1024.0 * 185.0));
//   IRMS = 0.02221441 * currentSquare;
//   return IRMS;
// }

LiquidCrystal LCD(7, 6, 5, 4, 3, 2);

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(RESET_BUTTON, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  LCD.begin(16, 2);
}

void loop()
{
  unsigned long _now = micros();
  if (_now - loopTime > DAC_TINH_Tc)
  {
    loopTime = _now;
    IRMS = ACS712_TrueRMS();
    if (IRMS > I_NGAN_MACH)
    {
      // tac dong
      digitalWrite(RELAY_PIN, HIGH);
      LCD.setCursor(0, 1);
      LCD.print("TRIP    ");
    }
    if (IRMS > Ip)
    {
      tI = (DAC_TINH_A / (pow(IRMS / Ip, DAC_TINH_p) - 1)) + DAC_TINH_B;
    }
    else
    {
      tI = (DAC_TINH_Tr) / (pow(IRMS / Ip, 2) - 1);
    }
    Serial.print("CI: ");
    Serial.println(cI);
    cI += (DAC_TINH_Tc / (tI * 1000000));
    if (cI >= 1)
    {
      // tac dong
      digitalWrite(RELAY_PIN, HIGH);
      LCD.setCursor(0, 1);
      LCD.print("TRIP    ");
    }
    if (cI < 0)
    {
      cI = 0;
    }
  }
  if (_now - hienThi >= CHU_KI_HIEN_THI)
  {
    hienThi = _now;
    LCD.setCursor(0, 0);
    LCD.print("IRMS: ");
    LCD.print(IRMS);
  }
  if (digitalRead(RESET_BUTTON) == 0)
  {
    digitalWrite(RELAY_PIN, LOW);
    LCD.setCursor(0, 1);
    LCD.print("RESET          ");
  }
}
