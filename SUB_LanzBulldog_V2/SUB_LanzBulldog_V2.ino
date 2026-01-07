// make sure to upload with ESP32 Dev Module selected as the board under tools>Board>ESP32 Arduino

#include <Arduino.h>
#include <ESP32Servo.h> // by Kevin Harrington
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

#define steeringServoPin 21
#define couplingServoPin 18
#define throttleI1 32
#define throttleI2 33
#define throttleI3 25  
#define throttleI4 26 
#define LED_BI 2

// global constants

extern const char* htmlHomePage PROGMEM;
const char* ssid = "Lanz_Bulldog"; //this Value will change the name of you ESP32's network

Servo steeringServo;
Servo couplingServo;

int steeringTrim = 9; //change this value in case your bulldog is not going straight larger number corrects to the right, smaller number to the left
int throttleTrim = 0;
int throttleValue = 0;
int steeringServoValue = 86;
int couplingServoValue = 180;
unsigned long couplingTimer = 0;
bool trailerCoupeled = false;
volatile int targetSteering = 86;
volatile int targetThrottle = 0;

AsyncWebServer server(80);
AsyncWebSocket wsCarInput("/CarInput");

void steeringControl(int steeringValue)
{
  steeringServoValue = steeringValue - steeringTrim;
  steeringServo.write(steeringServoValue);
}

void throttleControl(int SentThrottleValue)
{
  throttleValue = -(SentThrottleValue + throttleTrim);
  if (throttleValue < 0) {  // fwd speeds
    analogWrite(throttleI1, -throttleValue);
    analogWrite(throttleI2, 0);

    analogWrite(throttleI3, -throttleValue);
    analogWrite(throttleI4, 0);
  }
  else if (throttleValue == 0){
    analogWrite(throttleI1, 0);
    analogWrite(throttleI2, 0);

    analogWrite(throttleI3, 0);
    analogWrite(throttleI4, 0);
  } 
  else { // stop or rev
    analogWrite(throttleI1, 0);
    analogWrite(throttleI2, throttleValue);

    analogWrite(throttleI3, 0);
    analogWrite(throttleI4, throttleValue);
  }
}

void couplingControl()
{
  if ((millis() - couplingTimer) > 200){  
    if (trailerCoupeled == false){
      couplingServo.write(180);
      trailerCoupeled = true;
    }
    else if (trailerCoupeled == true){
      couplingServo.write(163);
      trailerCoupeled = false;
    }
    couplingTimer = millis();
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
      throttleControl(0);    
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
        if (key == "steering")
        {
          targetSteering = valueInt;
          
        }
        else if (key == "throttle")
        {
          targetThrottle = valueInt;
          
        }
        else if (key == "coupling")
        {
          couplingControl();
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
  steeringServo.attach(steeringServoPin);
  couplingServo.attach(couplingServoPin);
  steeringControl(steeringServoValue);
  pinMode(throttleI1, OUTPUT);
  pinMode(throttleI2, OUTPUT);
  pinMode(throttleI3, OUTPUT);
  pinMode(throttleI4, OUTPUT);
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

unsigned long lastControlUpdate = 0;
void loop()
{
  wsCarInput.cleanupClients();

  if (millis() - lastControlUpdate >= 20) { // 50 Hz
    steeringControl(targetSteering);
    throttleControl(targetThrottle);
    lastControlUpdate = millis();
  }
}
