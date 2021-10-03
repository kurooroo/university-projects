#include <ESP8266WiFi.h>

const char* ssid = "Ducnvm18";
const char* password = "ducnvm18";

bool doorStatus = 0; //0 = dong cua, 1 = mo cua
bool lightStatus = 0; //0 = tat den, 1 = bat den
bool fanStatus = 0; //0 = tat quat, 1 = bat quat

WiFiServer server(80);

IPAddress localIP(172, 20, 10, 10);
IPAddress gateway(172, 20, 10, 1);
IPAddress subnet(255, 255, 255, 240);

void setup(){
  Serial.begin(9600);
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);
  if(!WiFi.config(localIP, gateway, subnet))
  {
    Serial.println("STA failed to configure");
  }
  Serial.print("Connecting to the Newtork");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected"); 
  server.begin();
  Serial.println("Server started");
  Serial.print("IP Address of network: "); // will IP address on Serial Monitor
  Serial.println(WiFi.localIP());
  Serial.print("Copy and paste the following URL: http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void loop(){
  WiFiClient client = server.available();
  if (!client){
    return;}
    
  Serial.println("Waiting for new client");
  while(!client.available())
  {
    delay(1);
  }
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
  
  if(request.indexOf("/DOOR=OPEN") != -1){
    
    Serial.write(0);
    doorStatus = 1;
  }

  if(request.indexOf("/DOOR=CLOSE") != -1){

    Serial.write(1);
    doorStatus = 0;
  } 

  if(request.indexOf("/LIGHT=ON") != -1){

    Serial.write(2);
    lightStatus = 1;
  } 

  if(request.indexOf("/LIGHT=OFF") != -1){

    Serial.write(3);
    lightStatus = 0;
  } 

  if(request.indexOf("/FAN=ON") != -1){

    Serial.write(4);
    fanStatus = 1;
  } 

  if(request.indexOf("/FAN=OFF") != -1){

    Serial.write(5);
    fanStatus = 0;
  } 
  
//*------------------HTML Page Code---------------------*//

  client.println("HTTP/1.1 200 OK"); //
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.print(" CONTROL DOOR: ");
  if(doorStatus == 1){
    client.print("OPEN");
  }
  else
  {
    client.print("CLOSE");
  }
  client.println("<br><br>");
  client.println("<a href=\"/DOOR=OPEN\"\"><button>OPEN</button></a>");
  client.println("<a href=\"/DOOR=CLOSE\"\"><button>CLOSE</button></a><br />");
  client.println("</html>");

  client.print(" CONTROL LIGHT: ");
  if(lightStatus == 0){
    client.print("OFF");
  }
  else
  {
    client.print("ON");
  }
  client.println("<br><br>");
  client.println("<a href=\"/LIGHT=ON\"\"><button>ON</button></a>");
  client.println("<a href=\"/LIGHT=OFF\"\"><button>OFF</button></a><br />");
  client.println("</html>");
  
  client.print(" CONTROL FAN: ");
  if(fanStatus == 0){
    client.print("OFF");
  }
  else
  {
    client.print("ON");
  }
  client.println("<br><br>");
  client.println("<a href=\"/FAN=ON\"\"><button>ON</button></a>");
  client.println("<a href=\"/FAN=OFF\"\"><button>OFF</button></a><br />");
  client.println("</html>");

  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
}