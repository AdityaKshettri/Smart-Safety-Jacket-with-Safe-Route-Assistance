#include <Wire.h>
#include <I2C_Anything.h>
#include "DHT.h"
#include "TinyGPS++.h"
#include "SoftwareSerial.h"

//defining connection of all sensors 
SoftwareSerial GPSserial(10,9); //TX of GPS to 10, RX of GPS to 9
SoftwareSerial GSMserial(11, 12); //TX=11,RX=12
const int echoPin=2;
const int trigPin=4;
const int buzzer=5;
const int buttonPin=7; //pushbutton pin
#define DHTPIN 8
#define MQ2pin (0) //A0 pin in A0, leave D0 unconnected
#define x A1
#define y A2
#define z A3

//defining the constants
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
TinyGPSPlus gps;
int xsample=0;
int ysample=0;
int zsample=0;
#define samples 10
#define minVal -50
#define MaxVal 50

volatile float temp = 30;
volatile float hum = 70;
volatile float distance = 100;
volatile float mq2 = 100;
volatile float latitude = 12.844872;
volatile float longitude = 80.152809;
volatile float button = 0;
String message="Hey, This is Aditya. I m in serious trouble. Need Help. This is my Location. https://www.google.com/maps/place/";

void setup()
{
  Serial.begin (115200);
  GPSserial.begin(9600);
  GSMserial.begin(9600);
  dht.begin();
  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, LOW);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer,LOW);
  for(int i=0;i<samples;i++)
  {
    xsample+=analogRead(x);
    ysample+=analogRead(y);
    zsample+=analogRead(z);
  }
  xsample/=samples;
  ysample/=samples;
  zsample/=samples;
  Wire.begin(8);                /* join i2c bus with address 8 */
  Wire.onRequest(requestEvent); // function to run when asked for data
}

void loop()
{
  //dht11
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  Serial.print("temp = ");
  Serial.println(temp);
  Serial.print("hum = ");
  Serial.println(hum);

  //ultrasonic
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  distance= duration*0.034/2;
  Serial.print("distance = ");
  Serial.println(distance);

  //MQ-2 Sensor
  mq2 = analogRead(MQ2pin);
  Serial.print("mq2 = ");
  Serial.println(mq2);

  //buzzer condition
  if(temp>40||hum>90||mq2>300)
  {
    digitalWrite(buzzer,HIGH);
    delay(1000);
    digitalWrite(buzzer,LOW);
  }

  //getting location of the user
  latitude = 12.844872;
  longitude = 80.152809;
  while(GPSserial.available())//While there are characters to come from the GPS
  {
    gps.encode(GPSserial.read());//This feeds the serial NMEA data into the library one char at a time
  }
  if(gps.location.isUpdated())
  {
      latitude=gps.location.lat();
      longitude=gps.location.lng();
  }
  Serial.print("latitude = ");
  Serial.println(latitude);
  Serial.print("longitude = ");
  Serial.println(longitude);
  
  //reading the state of button
  button = digitalRead(buttonPin);

  //accelerometer caliberation
  int value1=analogRead(x);
  int value2=analogRead(y);
  int value3=analogRead(z);
  int xValue=xsample-value1;
  int yValue=ysample-value2;
  int zValue=zsample-value3;
  Serial.print("x = ");
  Serial.println(xValue);
  Serial.print("y = ");
  Serial.println(yValue);
  Serial.print("z = ");
  Serial.println(zValue);

  //checking the accident condition
  if(xValue<minVal||xValue>MaxVal||yValue<minVal||yValue>MaxVal||zValue<minVal||zValue>MaxVal)
  {
    button = 1.00;
    message = message + latitude + "," + longitude;
    GSMserial.println("AT+CMGF=1"); //To send SMS in Text Mode
    GSMserial.println("AT+CMGS=\"+919982779405\"\r"); //Change to destination phone number 
    GSMserial.println(message); //the content of the message
    GSMserial.println((char)26); //the stopping character Ctrl+Z
  }
  
  Serial.print("button = ");
  Serial.println(button);
  Serial.println("===========================");
}

// function: what to do when asked for data
void requestEvent(int howMany)
{
   I2C_writeAnything (temp);
   I2C_writeAnything (hum);
   I2C_writeAnything (distance);
   I2C_writeAnything (mq2);
   I2C_writeAnything (latitude);
   I2C_writeAnything (longitude);
   I2C_writeAnything (button);
   button = 0;

}
