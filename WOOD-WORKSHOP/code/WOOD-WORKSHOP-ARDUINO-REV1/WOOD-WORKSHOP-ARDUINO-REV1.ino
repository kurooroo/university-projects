#include <MQ135.h>
#include <SoftwareSerial.h>
#include <U8glib.h> // Thu vien oled

#define DATA_UPDATE_RATE 10000   // 10 giay cap nhat du lieu len cloud va oled 1 lan
#define SENSOR_READ_RATE 1000    // 1 giay doc cac cam bien bui va mq135 1 lan
#define GP_SAFE_DUST_DENSITY 60 // nguong an toan cua mat do bui
unsigned long now = 0;
unsigned long lastDisplay = 0;
unsigned long lastReadSensor = 0; // lưu trữ các bộ đếm thời gian

// khai báo chân và biến cho cảm biến bụi
int GP_measurePin = 0;
int GP_ledPower = 12;
int GP_samplingTime = 280;
int GP_deltaTime = 40;
float GP_voMeasured = 0;
float GP_calcVoltage = 0;
float GP_dustDensity = 0;
float GP_dustDensityAvg = 0;
int GP_dustDensityIndex = 0;

//MQ135
#define MQ135_RLOAD 22.0
MQ135 MQ123_gasSensor = MQ135(A1);
int MQ135_sensorPin = A1;
int MQ135_sensorValue = 0;
float MQ135_ppm = 0;
float MQ135_ppmAvg = 0;
int MQ135_ppmIndex = 0;

/***************************MOTOR*********************/
#define MOTOR_FIREPUMP 9
#define MOTOR_DUSTPUMP 10
/***************************MOTOR*********************/

/***************************ESP8266*********************/
SoftwareSerial ESP(3, 2);

// hàm gửi dữ liệu cho esp8266
void UART_SendData(void)
{
  // Quy dinh bat dau gui du lieu
  ESP.write(0x99);
  // Bat dau gui du lieu
  ESP.println(MQ135_ppmAvg);
  ESP.println(GP_dustDensityAvg);
}
/***************************ESP8266*********************/

/**************************GP2Y1010AU0F*********************/
// đọc cảm biến bụi
void GP_ReadSensor(void)
{
  digitalWrite(GP_ledPower, LOW);
  delayMicroseconds(GP_samplingTime);
  GP_voMeasured = analogRead(GP_measurePin);
  delayMicroseconds(GP_deltaTime);
  digitalWrite(GP_ledPower, HIGH);
  GP_calcVoltage = GP_voMeasured * (5.0 / 1024.0);
  GP_dustDensity += 170 * GP_calcVoltage - 0.1;
  GP_dustDensityIndex++;
}
/***************************GP2Y1010AU0F*********************/

/***************************BUZZER*********************/
int BUZZER_PIN = 8; // pin còi chip
/***************************BUZZER*********************/

/***************************FLAME*********************/
int FLAME_sensorPin = 4;
bool FLAME_status = HIGH;
bool FLAME_fireFlag = false;
// hàm đọc cảm biến lửa và bật tắt còi/bơm chữa cháy
void FLAME_ReadSensor(void)
{
  FLAME_status = digitalRead(FLAME_sensorPin);
  if (FLAME_status == LOW) // co chay
  {
    FLAME_fireFlag = true;
    digitalWrite(BUZZER_PIN, HIGH);
    analogWrite(MOTOR_FIREPUMP, 200);
  }
  else // khong co chay
  {
    if(FLAME_fireFlag == true)
    {
      delay(2000); // chờ cho đám cháy được dập tắt hẳn
      if(digitalRead(FLAME_sensorPin) == HIGH)
      {
        FLAME_fireFlag = false;
        digitalWrite(BUZZER_PIN, LOW);
        digitalWrite(MOTOR_FIREPUMP, LOW);
      }
    }
    else
    {
      digitalWrite(BUZZER_PIN, LOW);
        digitalWrite(MOTOR_FIREPUMP, LOW);
    }
  }
}
/***************************FLAME*********************/

/***************************MQ135*********************/
// hàm đọc cảm biến mq135
void MQ135_ReadSensor(void)
{
  MQ135_sensorValue = analogRead(MQ135_sensorPin);
  MQ135_ppm += MQ123_gasSensor.getPPM();
  MQ135_ppmIndex++;
}
/***************************MQ135*********************/
/***************************OLED*********************/
U8GLIB_SH1106_128X64 OLED(U8G_I2C_OPT_NONE); // khai báo oled

// khởi đầu cho oled
void OLED_Init(void)
{
  if (OLED.getMode() == U8G_MODE_R3G3B2)
  {
    OLED.setColorIndex(255); // white
  }
  else if (OLED.getMode() == U8G_MODE_GRAY2BIT)
  {
    OLED.setColorIndex(3); // max intensity
  }
  else if (OLED.getMode() == U8G_MODE_BW)
  {
    OLED.setColorIndex(1); // pixel on
  }
  else if (OLED.getMode() == U8G_MODE_HICOLOR)
  {
    OLED.setHiColorByRGB(255, 255, 255);
  }
}

// Hien thi man hinh Start
void OLED_DisplayStart(void)
{
  OLED.firstPage();
  do
  {
    OLED.setFont(u8g_font_profont12);
    OLED.setPrintPos(0, 10);
    OLED.print("GIAM SAT XUONG GO");
    OLED.setPrintPos(0, 25);
    OLED.print("Thiet bi dang khoi");
    OLED.setPrintPos(0, 40);
    OLED.print("dong va thu thap");
    OLED.setPrintPos(0, 55);
    OLED.print("du lieu");
  } while (OLED.nextPage());
}

// Hien thi he thong do
void OLED_Display(void)
{
  OLED.firstPage();
  do
  {
    OLED.setFont(u8g_font_profont12);
    OLED.setPrintPos(0, 10);
    OLED.print("GIAM SAT XUONG GO");
    OLED.setPrintPos(0, 25);
    OLED.print("BUI: " + String(GP_dustDensityAvg) + " ug/m3");
    OLED.setPrintPos(0, 40);
    OLED.print("MQ135: " + String(MQ135_ppmAvg) + " ppm");
  } while (OLED.nextPage());
}
/***************************OLED*********************/

// the setup function runs once when you press reset or power the board
void setup()
{
  pinMode(GP_measurePin, INPUT);
  pinMode(GP_ledPower, OUTPUT);
  pinMode(MQ135_sensorPin, INPUT);
  pinMode(FLAME_sensorPin, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  pinMode(MOTOR_FIREPUMP, OUTPUT);
  digitalWrite(MOTOR_FIREPUMP, LOW);
  pinMode(MOTOR_DUSTPUMP, OUTPUT);
  digitalWrite(MOTOR_DUSTPUMP, LOW);
  OLED_Init();
  OLED_DisplayStart();
  Serial.begin(9600);
  ESP.begin(9600);
}

// the loop function runs over and over again forever
void loop()
{
  FLAME_ReadSensor(); // liên tục đọc cảm biến lửa
  now = millis(); 
  // 1 giây đọc cảm biến 1 lần
  if (now - lastReadSensor > SENSOR_READ_RATE)
  {
    lastReadSensor = now;
    MQ135_ReadSensor();
    GP_ReadSensor();
  }
  // 10 giây hiển thị lên oled và gửi dữ liệu 1 lần
  if (now - lastDisplay > DATA_UPDATE_RATE)
  {
    lastDisplay = now;
    GP_dustDensityAvg = GP_dustDensity / GP_dustDensityIndex;
    MQ135_ppmAvg = MQ135_ppm / MQ135_ppmIndex;
    MQ135_ppm = 0;
    MQ135_ppmIndex = 0;
    GP_dustDensityIndex = 0;
    GP_dustDensity = 0;
    // Neu do bui vuot nguong an toan thi bat bom phun suong
    if (GP_dustDensityAvg > GP_SAFE_DUST_DENSITY)
    {
      // bat bom phun suong
      analogWrite(MOTOR_DUSTPUMP, 200);
    }
    // Chờ cho mật độ bụi giảm xuông dưới mức an toàn 10 ug/m3 mới tắt bơm
    if (GP_dustDensityAvg < (GP_SAFE_DUST_DENSITY - 10))
    {
      digitalWrite(MOTOR_DUSTPUMP, LOW);
    }
    UART_SendData();
    OLED_Display();
  }
}
