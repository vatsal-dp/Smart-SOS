#include <SoftwareSerial.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <iostream>
#include <string>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
SoftwareSerial mySUART(4, 5);  //D2, D1


const char * WIFI_SSID = "ESP";
const char * WIFI_PASS = "fwqx9263";

const int capacity = JSON_OBJECT_SIZE(10) + 100;
StaticJsonDocument<capacity> doc;

#define USE_SERIAL Serial
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{

  switch (type)
  {
  case WStype_DISCONNECTED:
    USE_SERIAL.printf("[WSc] Disconnected!\n");
    break;
  case WStype_CONNECTED:
    USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);
    break;
  case WStype_TEXT:
    USE_SERIAL.printf("[WSc] get text: %s\n", payload);
    break;
  case WStype_BIN:
    USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
    break;
  case WStype_ERROR:
  case WStype_FRAGMENT_TEXT_START:
  case WStype_FRAGMENT_BIN_START:
  case WStype_FRAGMENT:
  case WStype_FRAGMENT_FIN:
  case WStype_PING:
  case WStype_PONG:
    break;
  }
}

void setup()
{
  //Serial.begin(9600);
  mySUART.begin(9600);
  USE_SERIAL.begin(9600);
  USE_SERIAL.printf("Begin websocket client program....");

  USE_SERIAL.println();

  for (uint8_t t = 4; t > 0; t--)
  {
    USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }
  
  WiFiMulti.addAP(WIFI_SSID, WIFI_PASS);
  USE_SERIAL.printf("Connecting");
  
  while (WiFiMulti.run() != WL_CONNECTED)
  {
    USE_SERIAL.printf(".");
    delay(100);
  }
  USE_SERIAL.printf("Connected!");
  webSocket.beginSSL("octopus-app-dy9ew.ondigitalocean.app", 443, "/sendSensorData");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
}
String hr="",sp="",str="";
char s;
int flag=0,count=0;

void sensorVal(){
  if(mySUART.available()>0)
  {
    s = (char)mySUART.read();
    if(s=='\r'){
        count++;
        hr = str;
        int hrint=hr.toInt();
        str = "";
        if(hrint<220){
          doc.clear();
          
          doc["heartRate"] = hrint;
          
          flag = 1;
          //USE_SERIAL.println(hr);
        }
        else{
          str="";
          flag = 0;  
        }
      }
      else if(s=='\n'){
        count++;
        sp = str;
        int spi=sp.toInt();
        str = "";
        //flag = 0;
        if(spi<=100){
          if(flag==1){
            doc["SpO2"] = spi;
            char output[50];
  
            serializeJson(doc, output);
            USE_SERIAL.println(output);
          
            webSocket.sendTXT(output);
          }  
        }
        
        count = 0;
     } else{
      str+=s;
    }
  }
}
void loop()
{
  sensorVal();
  webSocket.loop();
}
