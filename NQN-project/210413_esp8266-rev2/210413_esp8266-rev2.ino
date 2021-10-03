#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

/* Put your SSID & Password */
const char* ssid = "ESP8266-WiFi";  // Enter SSID here
const char* password = "12345678";  //Enter Password here

/* Put IP Address details */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);

bool DOOR_STATUS = LOW;
bool LIGHT1_STATUS = LOW;
bool LIGHT2_STATUS = LOW;
int FAN1_STATUS = 0;
int FAN2_STATUS = 0;
String HTML_HEADER0;
String HTML_HEADER1;

void setup() {
  Serial.begin(9600);
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);
  HTML_HEADER0 += "<!DOCTYPE html> <html>\n";
  HTML_HEADER0 +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  HTML_HEADER0 +="<title>WIFI ROOM CONTROL</title>\n";
  HTML_HEADER0 +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  HTML_HEADER0 +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  HTML_HEADER0 +=".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  HTML_HEADER0 +=".button-on {background-color: #1abc9c;}\n";
  HTML_HEADER0 +=".button-on:active {background-color: #16a085;}\n";
  HTML_HEADER0 +=".button-off {background-color: #34495e;}\n";
  HTML_HEADER0 +=".button-off:active {background-color: #2c3e50;}\n";
  HTML_HEADER1 +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  HTML_HEADER1 += "table.center{\n";
  HTML_HEADER1 += "margin-left: auto;\n";
  HTML_HEADER1 += "margin-right: auto;\n";
  HTML_HEADER1 += "}\n";
  HTML_HEADER1 +="</style>\n";
  HTML_HEADER1 +="<meta http-equiv='Content-Type' content='text/html; charset=utf-8'>\n";
  HTML_HEADER1 +="</head>\n";
  HTML_HEADER1 +="<body>\n";
  // HTML_HEADER1 +="<h1>TÊN ĐỒ ÁN: </h1>\n";
  // HTML_HEADER1 +="<h3>SINH VIÊN THỰC HIỆN: </h3>\n";
  // HTML_HEADER1 +="<h3>MSSV: </h3>\n";
  WiFi.softAP(ssid, password, 11);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  
  server.on("/", handle_OnConnect);
  server.on("/door1open", handle_doorOpen);
  server.on("/door1close", handle_doorClose);
  server.on("/light1on", handle_light1on);
  server.on("/light1off", handle_light1off);
  server.on("/light2on", handle_light2on);
  server.on("/light2off", handle_light2off);
  server.on("/fan1on50", handle_fan1on50);
  server.on("/fan1on75", handle_fan1on75);
  server.on("/fan1on100", handle_fan1on100);
  server.on("/fan1off", handle_fan1off);
  server.on("/fan2on50", handle_fan2on50);
  server.on("/fan2on75", handle_fan2on75);
  server.on("/fan2on100", handle_fan2on100);
  server.on("/fan2off", handle_fan2off);
  server.onNotFound(handle_NotFound);
  
  server.begin();
  // Serial.println("HTTP server started");
}
void loop() {
  server.handleClient();
}

void handle_OnConnect() {
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", HTML_HEADER0);
  server.sendContent(HTML_HEADER1);
  SendHTML(DOOR_STATUS, LIGHT1_STATUS, LIGHT2_STATUS, FAN1_STATUS, FAN2_STATUS);
  server.sendContent("");
  server.stop();
  server.begin();
}

void handle_doorOpen()
{
  DOOR_STATUS = HIGH;
  Serial.write(0x00);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", HTML_HEADER0);
  server.sendContent(HTML_HEADER1);
  SendHTML(DOOR_STATUS, LIGHT1_STATUS, LIGHT2_STATUS, FAN1_STATUS, FAN2_STATUS);
  server.sendContent("");
  server.stop();
  server.begin();
}

void handle_doorClose()
{
  DOOR_STATUS = LOW;
  Serial.write(0x01);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", HTML_HEADER0);
  server.sendContent(HTML_HEADER1);
  SendHTML(DOOR_STATUS, LIGHT1_STATUS, LIGHT2_STATUS, FAN1_STATUS, FAN2_STATUS);
  server.sendContent("");
  server.stop();
  server.begin();
}

void handle_light1on() {
  LIGHT1_STATUS = HIGH;
  Serial.write(0x04);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", HTML_HEADER0);
  server.sendContent(HTML_HEADER1);
  SendHTML(DOOR_STATUS, LIGHT1_STATUS, LIGHT2_STATUS, FAN1_STATUS, FAN2_STATUS);
  server.sendContent("");
  server.stop();
  server.begin();
}

void handle_light1off() {
  LIGHT1_STATUS = LOW;
  Serial.write(0x05);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", HTML_HEADER0);
  server.sendContent(HTML_HEADER1);
  SendHTML(DOOR_STATUS, LIGHT1_STATUS, LIGHT2_STATUS, FAN1_STATUS, FAN2_STATUS);
  server.sendContent("");
  server.stop();
  server.begin();
}

void handle_light2on() {
  LIGHT2_STATUS = HIGH;
  Serial.write(0x06);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", HTML_HEADER0);
  server.sendContent(HTML_HEADER1);
  SendHTML(DOOR_STATUS, LIGHT1_STATUS, LIGHT2_STATUS, FAN1_STATUS, FAN2_STATUS);
  server.sendContent("");
  server.stop();
  server.begin();
}

void handle_light2off() {
  LIGHT2_STATUS = LOW;
  Serial.write(0x07);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", HTML_HEADER0);
  server.sendContent(HTML_HEADER1);
  SendHTML(DOOR_STATUS, LIGHT1_STATUS, LIGHT2_STATUS, FAN1_STATUS, FAN2_STATUS);
  server.sendContent("");
  server.stop();
  server.begin();
}

void handle_fan1on50()
{
  FAN1_STATUS = 1;
  Serial.write(0x09);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", HTML_HEADER0);
  server.sendContent(HTML_HEADER1);
  SendHTML(DOOR_STATUS, LIGHT1_STATUS, LIGHT2_STATUS, FAN1_STATUS, FAN2_STATUS);
  server.sendContent("");
  server.stop();
  server.begin();
}

void handle_fan1on75()
{
  FAN1_STATUS = 2;
  Serial.write(0x0A);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", HTML_HEADER0);
  server.sendContent(HTML_HEADER1);
  SendHTML(DOOR_STATUS, LIGHT1_STATUS, LIGHT2_STATUS, FAN1_STATUS, FAN2_STATUS);
  server.sendContent("");
  server.stop();
  server.begin();
}

void handle_fan1on100()
{
  FAN1_STATUS = 3;
  Serial.write(0x0B);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", HTML_HEADER0);
  server.sendContent(HTML_HEADER1);
  SendHTML(DOOR_STATUS, LIGHT1_STATUS, LIGHT2_STATUS, FAN1_STATUS, FAN2_STATUS);
  server.sendContent("");
  server.stop();
  server.begin();
}

void handle_fan1off()
{
  FAN1_STATUS = 0;
  Serial.write(0X08);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", HTML_HEADER0);
  server.sendContent(HTML_HEADER1);
  SendHTML(DOOR_STATUS, LIGHT1_STATUS, LIGHT2_STATUS, FAN1_STATUS, FAN2_STATUS);
  server.sendContent("");
  server.stop();
  server.begin();
}

void handle_fan2on50()
{
  FAN2_STATUS = 1;
  Serial.write(0x0D);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", HTML_HEADER0);
  server.sendContent(HTML_HEADER1);
  SendHTML(DOOR_STATUS, LIGHT1_STATUS, LIGHT2_STATUS, FAN1_STATUS, FAN2_STATUS);
  server.sendContent("");
  server.stop();
  server.begin();
}

void handle_fan2on75()
{
  FAN2_STATUS = 2;
  Serial.write(0x0E);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", HTML_HEADER0);
  server.sendContent(HTML_HEADER1);
  SendHTML(DOOR_STATUS, LIGHT1_STATUS, LIGHT2_STATUS, FAN1_STATUS, FAN2_STATUS);
  server.sendContent("");
  server.stop();
  server.begin();
}

void handle_fan2on100()
{
  FAN2_STATUS = 3;
  Serial.write(0x0F);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", HTML_HEADER0);
  server.sendContent(HTML_HEADER1);
  SendHTML(DOOR_STATUS, LIGHT1_STATUS, LIGHT2_STATUS, FAN1_STATUS, FAN2_STATUS);
  server.sendContent("");
  server.stop();
  server.begin();
}

void handle_fan2off()
{
  FAN2_STATUS = 0;
  Serial.write(0x0C);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", HTML_HEADER0);
  server.sendContent(HTML_HEADER1);
  SendHTML(DOOR_STATUS, LIGHT1_STATUS, LIGHT2_STATUS, FAN1_STATUS, FAN2_STATUS);
  server.sendContent("");
  server.stop();
  server.begin();
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

int SendHTML(uint8_t doorStatus, uint8_t light1Status, uint8_t light2Status, uint8_t fan1Status, uint8_t fan2Status){
  String ptr;  
  if(doorStatus)
  {
    ptr += "<p>CỬA CHÍNH : MỞ</p><a class=\"button button-off\" href=\"/door1close\">ĐÓNG</a>\n";
  }
  else
  {
    ptr += "<p>CỬA CHÍNH: ĐÓNG</p><a class=\"button button-on\" href=\"/door1open\">MỞ</a>\n";
  }

  if(light1Status)
  {
    ptr += "<p>ĐÈN 1: BẬT</p><a class=\"button button-off\" href=\"/light1off\">TẮT</a>\n";
  }
  else
  {
    ptr += "<p>ĐÈN 1: TẮT</p><a class=\"button button-on\" href=\"/light1on\">BẬT</a>\n";
  }

  if(light2Status)
  {
    ptr += "<p>ĐÈN 2: BẬT</p><a class=\"button button-off\" href=\"/light2off\">TẮT</a>\n";
  }
  else
  {
    ptr += "<p>ĐÈN 2: TẮT</p><a class=\"button button-on\" href=\"/light2on\">BẬT</a>\n";
  }
  server.sendContent(ptr);
  ptr = "";
  switch (fan1Status)
  {
  case 0:
    ptr += "<p>QUẠT 1: TẮT</p>\n";
    break;
  case 1:
    ptr += "<p>QUẠT 1: BẬT SỐ 1</p>\n";
    break;
  case 2:
    ptr += "<p>QUẠT 1: BẬT SỐ 2</p>\n";
    break;
  case 3:
    ptr += "<p>QUẠT 1: BẬT SỐ 3</p>\n";
    break;
  }
  ptr += "\n<table class=\"center\">\n<tr>";
  ptr += "<td><a class=\"button button-on\" href=\"/fan1off\">TẮT</a></td>";
  ptr += "<td><a class=\"button button-on\" href=\"/fan1on50\">SỐ 1</a></td>";
  ptr += "<td><a class=\"button button-on\" href=\"/fan1on75\">SỐ 2</a></td>";
  ptr += "<td><a class=\"button button-on\" href=\"/fan1on100\">SÔ 3</a></td>";
  ptr += "</tr>\n</table>\n";

  switch (fan2Status)
  {
  case 0:
    ptr += "<p>QUẠT 2: TẮT</p>\n";
    break;
  case 1:
    ptr += "<p>QUẠT 2: BẬT SỐ 1</p>\n";
    break;
  case 2:
    ptr += "<p>QUẠT 2: BẬT SỐ 2</p>\n";
    break;
  case 3:
    ptr += "<p>QUẠT 2: BẬT SỐ 3</p>\n";
    break;
  }
  ptr += "<table class=\"center\">\n<tr>";
  ptr += "<td><a class=\"button button-on\" href=\"/fan2off\">TẮT</a></td>";
  ptr += "<td><a class=\"button button-on\" href=\"/fan2on50\">SỐ 1</a></td>";
  ptr += "<td><a class=\"button button-on\" href=\"/fan2on75\">SỐ 2</a></td>";
  ptr += "<td><a class=\"button button-on\" href=\"/fan2on100\">SÔ 3</a></td>";
  ptr += "</tr>\n</table>\n";

  ptr +="</body>\n";
  ptr +="</html>\n";
  server.sendContent(ptr);
  return 1;
}