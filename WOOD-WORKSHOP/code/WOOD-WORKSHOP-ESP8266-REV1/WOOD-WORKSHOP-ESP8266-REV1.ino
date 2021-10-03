#include <ESP8266WiFi.h>
#include <ThingsBoard.h>

#define WIFI_AP "DAUCHIEN" // SSID wifi
#define WIFI_PASSWORD "12344321"  // password

#define TOKEN "aJYIx3SzaBXDm303yTez" // token của thiết bị 

char thingsboardServer[] = "demo.thingsboard.io"; // máy chủ think board

WiFiClient wifiClient;

ThingsBoard tb(wifiClient);

int status = WL_IDLE_STATUS;
unsigned long lastSend;
float MQ135_data;
float DUST_data;

void setup()
{
  Serial.begin(9600);
  delay(10);
  InitWiFi();
  lastSend = 0;
}

void loop()
{
  if (!tb.connected())
  {
    reconnect();
  }

  if (Serial.available())
  {
    // nhận dữ liệu từ arduinonano
    UART_GetData();
    // gửi dữ liệu lên thinkboard
    TB_SendData();
  }
  tb.loop();
}

// Kết nối với Wifi
void InitWiFi()
{
  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
}

// Kết nối lại wifi và thinkboard khi mất kết nối
void reconnect()
{
  // Loop until we're reconnected
  while (!tb.connected())
  {
    status = WiFi.status();
    if (status != WL_CONNECTED)
    {
      WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      while (WiFi.status() != WL_CONNECTED)
      {
        delay(500);
      }
    }
    // Serial.print("Connecting to ThingsBoard node ...");
    if (tb.connect(thingsboardServer, TOKEN))
    {
      // Serial.println( "[DONE]" );
    }
    else
    {
      // Serial.print( "[FAILED]" );
      // Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void UART_GetData(void)
{
  String temp;
  int check;
  check = Serial.read();
  if (check = 0x99)
  {
    temp = Serial.readStringUntil('\n');
    MQ135_data = temp.toFloat();
    temp = Serial.readStringUntil('\n');
    DUST_data = temp.toFloat();
  }
}

void TB_SendData(void)
{
  tb.sendTelemetryFloat("dust", DUST_data);
  tb.sendTelemetryFloat("mq135", MQ135_data);
}