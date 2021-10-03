# 1 "d:\\tunaOneDrive\\OneDrive - Hanoi University of Science and Technology\\tunaOneDriveSync\\tunaProjects\\Mercenary-projects\\WOOD-WORKSHOP\\code\\WOOD-WORKSHOP-ESP8266-REV1\\WOOD-WORKSHOP-ESP8266-REV1.ino"
# 2 "d:\\tunaOneDrive\\OneDrive - Hanoi University of Science and Technology\\tunaOneDriveSync\\tunaProjects\\Mercenary-projects\\WOOD-WORKSHOP\\code\\WOOD-WORKSHOP-ESP8266-REV1\\WOOD-WORKSHOP-ESP8266-REV1.ino" 2
# 3 "d:\\tunaOneDrive\\OneDrive - Hanoi University of Science and Technology\\tunaOneDriveSync\\tunaProjects\\Mercenary-projects\\WOOD-WORKSHOP\\code\\WOOD-WORKSHOP-ESP8266-REV1\\WOOD-WORKSHOP-ESP8266-REV1.ino" 2






char thingsboardServer[] = "demo.thingsboard.io";

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
    UART_GetData();
    // Gui data len thinkboard
    TB_SendData();
  }
  tb.loop();
}

void InitWiFi()
{
  WiFi.begin("DAUCHIEN" /* SSID wifi*/, "12344321" /* password*/);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
}

void reconnect()
{
  // Loop until we're reconnected
  while (!tb.connected())
  {
    status = WiFi.status();
    if (status != WL_CONNECTED)
    {
      WiFi.begin("DAUCHIEN" /* SSID wifi*/, "12344321" /* password*/);
      while (WiFi.status() != WL_CONNECTED)
      {
        delay(500);
      }
    }
    // Serial.print("Connecting to ThingsBoard node ...");
    if (tb.connect(thingsboardServer, "aJYIx3SzaBXDm303yTez"))
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
