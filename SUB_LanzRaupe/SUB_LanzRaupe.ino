// make sure to upload with ESP32 Dev Module selected as the board under tools>Board>ESP32 Arduino
#include <Arduino.h>
//#include <ESP32Servo.h> // by Kevin Harrington
#include <ESPAsyncWebSrv.h> // by dvarrel
#include <iostream>
#include <sstream>

#if defined(ESP32)
#include <AsyncTCP.h> // by dvarrel
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESPAsyncTCP.h> // by dvarrel
#endif

// defines
#define throttleI1 32
#define throttleI2 33
#define throttleI3 25
#define throttleI4 26
#define bladeI1 12
#define bladeI2 13 
#define LED_BI 2

// global constants
extern const char* htmlHomePage PROGMEM;
const char* ssid = "Lanz_Raupe"; //this Value will change the name of you ESP32's network
int leftThrottleTrim = 0;
int LeftThrottleValue = 0;
int RightThrottleTrim = 0;
int RightThrottleValue = 0;
unsigned long bladeTimer = 0;
unsigned long lightTimer = 0;
bool lightIsOn = false;
int BladeSpeed = 512;
bool BladeRunning = false;

AsyncWebServer server(80);
AsyncWebSocket wsCarInput("/CarInput");

void LeftThrottleControl(int SentLeftThrottleValue)
{
  LeftThrottleValue = SentLeftThrottleValue + leftThrottleTrim;
  if (LeftThrottleValue < 0) {  // fwd speeds
    analogWrite(throttleI3, -LeftThrottleValue);
    analogWrite(throttleI4, 0);
  }
  else if (LeftThrottleValue == 0){
    analogWrite(throttleI3, 0);
    analogWrite(throttleI4, 0);
  } 
  else { // stop or rev
    analogWrite(throttleI3, 0);
    analogWrite(throttleI4, LeftThrottleValue);
  } 
}

void RightThrottleControl(int SentRightThrottleValue)
{
  RightThrottleValue = SentRightThrottleValue + RightThrottleTrim;
  if (RightThrottleValue < 0) {  // fwd speeds
    analogWrite(throttleI1, -RightThrottleValue);
    analogWrite(throttleI2, 0);
  }
  else if (RightThrottleValue == 0){
    analogWrite(throttleI1, 0);
    analogWrite(throttleI2, 0);
  } 
  else { // stop or rev
    analogWrite(throttleI1, 0);
    analogWrite(throttleI2, RightThrottleValue);
  } 
}

void BladeUp(int dir)
{ 
  bladeTimer = millis();
  if (dir == 1) {
    BladeRunning = true;
    analogWrite(bladeI1, BladeSpeed);
    analogWrite(bladeI2, 0);
  }
  else if (dir == 0){
    analogWrite(bladeI1, 0);
    analogWrite(bladeI2, 0);
    BladeRunning = false;
  }
}

void BladeDown(int dir)
{
  bladeTimer = millis();
  if (dir == 1) {
    BladeRunning = true;
    analogWrite(bladeI1, 0);
    analogWrite(bladeI2, BladeSpeed);
  }
  else if(dir == 0){
    analogWrite(bladeI1, 0);
    analogWrite(bladeI2, 0);
    BladeRunning = false;
  }
}

void handleRoot(AsyncWebServerRequest *request)
{
  request->send_P(200, "text/html", htmlHomePage);
}

void handleNotFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "File Not Found");
}

void onCarInputWebSocketEvent(AsyncWebSocket *server,
                              AsyncWebSocketClient *client,
                              AwsEventType type,
                              void *arg,
                              uint8_t *data,
                              size_t len)
{
  switch (type)
  {
    case WS_EVT_CONNECT:
      //Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      digitalWrite(LED_BI, LOW);
      break;
    case WS_EVT_DISCONNECT:
      //Serial.printf("WebSocket client #%u disconnected\n", client->id());
      //Failsafe in Case of disconnect as of now doesnt work
      RightThrottleControl(0);  
      LeftThrottleControl(0);   
      BladeUp(0);               
      BladeDown(0);
      digitalWrite(LED_BI, HIGH);
      break;
    case WS_EVT_DATA:
      AwsFrameInfo *info;
      info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
      {
        std::string myData = "";
        myData.assign((char *)data, len);
        std::istringstream ss(myData);
        std::string key, value;
        std::getline(ss, key, ',');
        std::getline(ss, value, ',');
        Serial.printf("Key [%s] Value[%s]\n", key.c_str(), value.c_str());
        int valueInt = atoi(value.c_str());
        if (key == "RightThrottle")
        {
          RightThrottleControl(valueInt);
        }
        else if (key == "LeftThrottle")
        {
          LeftThrottleControl(valueInt);
        }
        else if (key == "bladeUp")
        {
          BladeUp(valueInt);
        }
        else if (key == "bladeDown")
        {
          BladeDown(valueInt);
        }
      }
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
    default:
      break;
  }
}

void setUpPinModes()
{
  pinMode(throttleI1, OUTPUT);
  pinMode(throttleI2, OUTPUT);
  pinMode(throttleI3, OUTPUT);
  pinMode(throttleI4, OUTPUT);
  pinMode(bladeI1, OUTPUT);
  pinMode(bladeI2, OUTPUT);
  pinMode(LED_BI, OUTPUT);
}

void setup(void)
{
  setUpPinModes();
  Serial.begin(115200);

  WiFi.softAP(ssid );
  IPAddress IP = WiFi.softAPIP();
  Serial.print("IP is ");
  Serial.println(IP);

  server.on("/", HTTP_GET, handleRoot);
  server.onNotFound(handleNotFound);

  wsCarInput.onEvent(onCarInputWebSocketEvent);
  server.addHandler(&wsCarInput);
  
  server.begin();
  Serial.println("Server online");
  //Serial.end();
}

void loop()
{
  wsCarInput.cleanupClients();
  if(BladeRunning){
    if((millis() - bladeTimer) > 3000){
      analogWrite(bladeI1, 0);
      analogWrite(bladeI2, 0);
      BladeRunning = false;
    }
  }
}
