# 1 "d:\\tunaOneDrive\\OneDrive - Hanoi University of Science and Technology\\tunaOneDriveSync\\tunaProjects\\Mercenary-projects\\DIGITAL-RELAY-CURRENT-PROTECTION\\CURRENT-PROTECTION-RELAY-ARDUINO\\CURRENT-PROTECTION-RELAY-ARDUINO.ino"
# 2 "d:\\tunaOneDrive\\OneDrive - Hanoi University of Science and Technology\\tunaOneDriveSync\\tunaProjects\\Mercenary-projects\\DIGITAL-RELAY-CURRENT-PROTECTION\\CURRENT-PROTECTION-RELAY-ARDUINO\\CURRENT-PROTECTION-RELAY-ARDUINO.ino" 2
# 3 "d:\\tunaOneDrive\\OneDrive - Hanoi University of Science and Technology\\tunaOneDriveSync\\tunaProjects\\Mercenary-projects\\DIGITAL-RELAY-CURRENT-PROTECTION\\CURRENT-PROTECTION-RELAY-ARDUINO\\CURRENT-PROTECTION-RELAY-ARDUINO.ino" 2
# 17 "d:\\tunaOneDrive\\OneDrive - Hanoi University of Science and Technology\\tunaOneDriveSync\\tunaProjects\\Mercenary-projects\\DIGITAL-RELAY-CURRENT-PROTECTION\\CURRENT-PROTECTION-RELAY-ARDUINO\\CURRENT-PROTECTION-RELAY-ARDUINO.ino"
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
    if (micros() - adcSampleTime > 200)
    {
      adcSampleTime = micros();
      sampleNumber++;
      int ADC_read = analogRead(A0);
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
  pinMode(9, 0x2);
  pinMode(8, 0x1);
  digitalWrite(8, 0x0);
  LCD.begin(16, 2);
}

void loop()
{
  unsigned long _now = micros();
  if (_now - loopTime > 20000 /* = 0.02s*/)
  {
    loopTime = _now;
    IRMS = ACS712_TrueRMS();
    if (IRMS > 10.0 /*Dong dien ngan mach*/)
    {
      // tac dong
      digitalWrite(8, 0x1);
      LCD.setCursor(0, 1);
      LCD.print("TRIP    ");
    }
    if (IRMS > 1.0 /* Dong dien dinh muc*/)
    {
      tI = (0.052 / (pow(IRMS / 1.0 /* Dong dien dinh muc*/, 0.02) - 1)) + 0.114;
    }
    else
    {
      tI = (4.85) / (pow(IRMS / 1.0 /* Dong dien dinh muc*/, 2) - 1);
    }
    Serial.print("CI: ");
    Serial.println(cI);
    cI += (20000 /* = 0.02s*/ / (tI * 1000000));
    if (cI >= 1)
    {
      // tac dong
      digitalWrite(8, 0x1);
      LCD.setCursor(0, 1);
      LCD.print("TRIP    ");
    }
    if (cI < 0)
    {
      cI = 0;
    }
  }
  if (_now - hienThi >= 1000000 /*1s cap nhan man hinh LCD 1 lan*/)
  {
    hienThi = _now;
    LCD.setCursor(0, 0);
    LCD.print("IRMS: ");
    LCD.print(IRMS);
  }
  if (digitalRead(9) == 0)
  {
    digitalWrite(8, 0x0);
    LCD.setCursor(0, 1);
    LCD.print("RESET          ");
  }
}
