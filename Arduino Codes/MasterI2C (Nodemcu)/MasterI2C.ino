#include <ESP8266WiFi.h>
#include <WiFiClient.h>;
#include <Wire.h>
#include <I2C_Anything.h>

const char *ssid =  "RaspiWifi"; // My WiFi SSID
const char *pass =  "jioaditya"; // My WiFi Password

const char* thingspeak = "api.thingspeak.com"; // Thingspeak URL
String apiKey = "E1FGU5V3DSD81A0H"; // My Thingspeak API key

const char* pushingbox = "api.pushingbox.com"; // PushingBox URL
String deviceId = "v5D4D4E76B8B1BD1"; // My PushingBox DeviceID

const int port = 80;
const String message="Hey, This is Aditya. I m in serious trouble. Need Help. This is my Location. https://www.google.com/maps/place/";

WiFiClient client;

volatile float temp;
volatile float hum;
volatile float distance;
volatile float mq2;
volatile float latitude;
volatile float longitude;
volatile float button;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  Wire.begin(D1, D2); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */
}

void loop()
{
  Wire.beginTransmission (8);
  Wire.requestFrom(8, 28); // request 8 bytes from salve Uno #8
  I2C_readAnything (temp);
  I2C_readAnything (hum);
  I2C_readAnything (distance);
  I2C_readAnything (mq2);
  I2C_readAnything (latitude);
  I2C_readAnything (longitude);
  I2C_readAnything (button);
  Wire.endTransmission ();

  Serial.print("Temperature value is = ");
  Serial.print(temp);
  Serial.println(" Celsius, ");

  Serial.print("Humididty value is = ");
  Serial.print(hum);
  Serial.println(",");

  Serial.print("Distance value is = ");
  Serial.print(distance);
  Serial.println(" cm,");

  Serial.print("Smoke Sensor value is = ");
  Serial.print(mq2);
  Serial.println(",");
  
  Serial.print("Latitude is = ");
  Serial.print(latitude);
  Serial.println(",");
  
  Serial.print("Longitude is = ");
  Serial.print(longitude);
  Serial.println(",");

  Serial.print("Button sensor value is = ");
  Serial.println(button);
  Serial.println("==============================");

  if(button==1.00)
  {
    sendAlertMessage(latitude, longitude);
  }
  updateThingspeak(temp, hum, distance, mq2);
}

void updateThingspeak(float t, float h, float d, float m)
{
  if (client.connect(thingspeak,port)) //"184.106.153.149" or api.thingspeak.com
  {  
    String postStr=apiKey;
    postStr +="&field1=" + String(t);
    postStr +="&field2=" + String(h);
    postStr +="&field3=" + String(d);
    postStr +="&field4=" + String(m);
    postStr += "\r\n\r\n";
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
  }
  client.stop(); 
}

void sendAlertMessage(float latitude, float longitude)
{
  if (client.connect(pushingbox, port)) 
  { 
    String postStr = "devid=";
    postStr += String(deviceId);
    postStr += "&message_parameter=";
    postStr += message+String(latitude)+","+String(longitude);
    postStr += "\r\n\r\n";
    client.print("POST /pushingbox HTTP/1.1\n");
    client.print("Host: api.pushingbox.com\n");
    client.print("Connection: close\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
  }
  client.stop();
}
