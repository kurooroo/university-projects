#include <Arduino.h>
#line 1 "d:\\tunaOneDrive\\OneDrive - Hanoi University of Science and Technology\\tunaOneDriveSync\\tunaProjects\\Mercenary-projects\\AUTO-HANDWASH-AND-THERMOMETER\\ARDUINO\\ARDUINO.ino"
/*
Nguyên lý hoạt động: khi có người đi đến trước thiết bị sẽ được yêu cầu rửa tay 
và đo nhiệt độ. Rửa tay bằng cách đưa tay che cảm biến hồng ngoại để nhận nước rửa tay
và đo nhiệt độ bằng cách đưa tay hoặc trán vào phần cảm biến nhiệt độ trên mạch. Sau khi
đo nhiệt độ và rửa tay máy sẽ phát ra thông báo cảm ơn. Nếu nhiệt độ đo lớn hơn 38 độ C 
máy sẽ phát ra cảnh báo "Nhiệt độ vượt quá 38 độ C"
Nếu trong 15s người đó không thực hiện hai hoạt động rửa tay và đo nhiệt độ máy sẽ 
quay trở lại trạng thái hoạt động bình thường để chờ người tiếp theo.
*/

#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

LiquidCrystal_I2C LCD(0x27, 16, 2);
Adafruit_MLX90614 MLX90614 = Adafruit_MLX90614();
SoftwareSerial mySoftwareSerial(10, 11);
DFRobotDFPlayerMini myDFPlayer;
const unsigned int HUMAN_DETECTION_TRIG_PIN = 2;
const unsigned int HUMAN_DETECTION_ECHO_PIN = 3;
const unsigned int LIQUID_LEVEL_TRIG_PIN = 4;
const unsigned int LIQUID_LEVEL_ECHO_PIN = 5;
const unsigned int HAND_DETECTION_PIN = A0;
const unsigned int PUMP_PIN = 6;

double nhietDo; //lưu lại nhiệt độ vừa đo
bool daDoNhietDo = 0; // nếu đã đo nhiệt độ thì = 1, chưa đo = 0
bool daRuaTay = 0; // nếu đã rửa tay thì = 1, chưa = 0
unsigned long humanTimeout = 0;
int khoangCachNguoi; // khoảng cách đến vật thể gần nhất, nên để 1m trước mặt trống để nhận biết người tốt hơn
int mucDungDich; // khoảng mức dung dịch rửa tay
int khoangCachTay; // khoảng cách đến tay để phun dung dịch

void ReadSRF05();
void ReadHandSensor();
void ReadMLX90614();

#line 40 "d:\\tunaOneDrive\\OneDrive - Hanoi University of Science and Technology\\tunaOneDriveSync\\tunaProjects\\Mercenary-projects\\AUTO-HANDWASH-AND-THERMOMETER\\ARDUINO\\ARDUINO.ino"
void setup();
#line 72 "d:\\tunaOneDrive\\OneDrive - Hanoi University of Science and Technology\\tunaOneDriveSync\\tunaProjects\\Mercenary-projects\\AUTO-HANDWASH-AND-THERMOMETER\\ARDUINO\\ARDUINO.ino"
void loop();
#line 148 "d:\\tunaOneDrive\\OneDrive - Hanoi University of Science and Technology\\tunaOneDriveSync\\tunaProjects\\Mercenary-projects\\AUTO-HANDWASH-AND-THERMOMETER\\ARDUINO\\ARDUINO.ino"
void Read_SRF05();
#line 40 "d:\\tunaOneDrive\\OneDrive - Hanoi University of Science and Technology\\tunaOneDriveSync\\tunaProjects\\Mercenary-projects\\AUTO-HANDWASH-AND-THERMOMETER\\ARDUINO\\ARDUINO.ino"
void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  // setup MLX90614
  MLX90614.begin();
  // setup LCD
  LCD.init();
  LCD.backlight();
  // setup DFPlayer Mini
  mySoftwareSerial.begin(9600);

  if (!myDFPlayer.begin(mySoftwareSerial))
  { //Use softwareSerial to communicate with mp3.
    while (true)
    {
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  myDFPlayer.volume(30); //Set volume value. From 0 to 30
  // setup SRF05
  pinMode(HUMAN_DETECTION_ECHO_PIN, INPUT);
  pinMode(HUMAN_DETECTION_TRIG_PIN, OUTPUT);
  pinMode(LIQUID_LEVEL_TRIG_PIN, OUTPUT);
  pinMode(LIQUID_LEVEL_ECHO_PIN, INPUT);
  // setup cam bien IR phat hien tay de phun dung dich rua tay
  pinMode(HAND_DETECTION_PIN, INPUT);
  // setup dong co bom
  pinMode(PUMP_PIN, OUTPUT);
  delay(1000);
}

void loop()
{
  // put your main code here, to run repeatedly:
  Read_SRF05();
  if (khoangCachNguoi < 100) // nếu phát hiện người trong khoảng 1m thì xin chào và yêu cầu đo nhiệt độ + rửa tay
  {
    humanTimeout = millis();
    myDFPlayer.play(1); // mp3 xin chào
    delay(500);
    while (millis() - humanTimeout < 15000)
    {
      ReadMLX90614(); // đọc cảm biến nhiệt độ
      ReadHandSensor(); // đọc cảm biến nhận biết tay để phun dung dịch
      if (daDoNhietDo)
      {
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("T: ");
        LCD.print(nhietDo);
        LCD.print("*C");
        LCD.setCursor(0, 1);
        LCD.print("Dung dich: ");
        LCD.print(mucDungDich);
        LCD.print("%");
        if (nhietDo > 38) // nếu nhiệt độ lớn hơn 38 thì gửi cảnh báo
        {
          myDFPlayer.stop();
          delay(200);
          myDFPlayer.play(3); //mp3 cảnh báo
          delay(5000);
        }
      }
      else
      {
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("T: San sang do");
        LCD.setCursor(0, 1);
        LCD.print("Dung dich: ");
        LCD.print(mucDungDich);
        LCD.print("%");
      }
      if ((khoangCachTay < 100) && (daRuaTay == 0)) // nếu đưa tay vào cảm biến thì phun dung dịch rửa tay
      {
        analogWrite(PUMP_PIN, 100);
        delay(2000); // phun nuoc rua tay trong 2s
        digitalWrite(PUMP_PIN, LOW);
        daRuaTay = 1;
      }
      else
      {
        digitalWrite(PUMP_PIN, LOW);
      }
      if ((daDoNhietDo == 1) && (daRuaTay == 1)) // nếu đã rửa tay và đo nhiệt độ thì thoát ra khỏi vòng lặp để chờ người tiếp theo
      {
        myDFPlayer.stop();
        delay(200);
        myDFPlayer.play(2); // mp3 cảm ơn
        delay(1000);
        break;
      }
      delay(500);
    }
    daRuaTay = 0;
    daDoNhietDo = 0;
  }
  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.print("T: San sang");
  LCD.setCursor(0, 1);
  LCD.print("Liquid: ");
  LCD.print(mucDungDich);
  LCD.print("%");
  delay(1000);
}

void Read_SRF05()
{
  // đọc cảm biến siêu âm để đo khoảng cách tới người
  digitalWrite(HUMAN_DETECTION_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(HUMAN_DETECTION_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(HUMAN_DETECTION_TRIG_PIN, LOW);
  unsigned long duration = pulseIn(HUMAN_DETECTION_ECHO_PIN, HIGH);
  khoangCachNguoi = duration / 29 / 2; // chuyển từ thời gian phản hồi sóng âm sang khoảng cách
  Serial.println(khoangCachNguoi);

  // đọc cảm biến siêu âm để xác định dung dịch còn lại
  digitalWrite(LIQUID_LEVEL_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(LIQUID_LEVEL_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(LIQUID_LEVEL_TRIG_PIN, LOW);
  duration = pulseIn(LIQUID_LEVEL_ECHO_PIN, HIGH);
  mucDungDich = duration / 29 / 2;  // chuyển từ thời gian phản hồi sóng âm sang khoảng cách
  mucDungDich = (10 - mucDungDich) * 100 / 10; // chuyển từ khoảng cách sang mức dung dịch còn lại
}

void ReadHandSensor()
{
  int analogHandDetection = analogRead(HAND_DETECTION_PIN);
  khoangCachTay = analogHandDetection;
}

void ReadMLX90614()
{
  Read_SRF05();
  if (khoangCachNguoi < 5) // nếu có vật thể (tay, trán) trong khoảng 5cm trước cảm biến thì đo nhiệt độ
  {
    nhietDo = MLX90614.readObjectTempC() + 2;
    daDoNhietDo = 1;
  }
}
