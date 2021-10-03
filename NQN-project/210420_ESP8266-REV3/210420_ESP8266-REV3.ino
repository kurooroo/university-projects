#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "0KfXzaHTn8i3vEGr21V-PQctynL1CAN4";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Nguyen Quang Ninh";
char pass[] = "123455789";

// Dieu khien cua
BLYNK_WRITE(V0)
{
  int pinValue = param.asInt();
  if(pinValue == 1)
  {
    Serial.write(0x00); // mo cua
  }
  else
  {
    Serial.write(0x01); // dong cua
  }
}

// dieu khien den 1
BLYNK_WRITE(V1)
{
  int pinValue = param.asInt();
  if(pinValue == 1)
  {
    Serial.write(0x02); // bat den 1
  }
  else
  {
    Serial.write(0x03); // tat den 1
  }
}

// Dieu khien den 2
BLYNK_WRITE(V2)
{
  int pinValue = param.asInt();
  if(pinValue == 1)
  {
    Serial.write(0x04); // bat den 2
  }
  else
  {
    Serial.write(0x05); // tat den 2
  }
}

// Dieu khien quat
BLYNK_WRITE(V3)
{
  int pinValue = param.asInt();
  if(pinValue == 1)
  {
    Serial.write(0x06); // tat quat
  }
}

BLYNK_WRITE(V4)
{
  int pinValue = param.asInt();
  if(pinValue == 1)
  {
    Serial.write(0x07); // bat so 1
  }
}

BLYNK_WRITE(V5)
{
  int pinValue = param.asInt();
  if(pinValue == 1)
  {
    Serial.write(0x08); // bat so 2
  }
}

BLYNK_WRITE(V6)
{
  int pinValue = param.asInt();
  if(pinValue == 1)
  {
    Serial.write(0x09); // bat so 3
  }
}

void setup()
{
  // Debug 
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
}

void loop()
{
  Blynk.run();
}