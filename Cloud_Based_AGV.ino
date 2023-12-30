#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include "Arduino.h"
#include <analogWrite.h>

// Motor A

int enA = 14;
int in1 = 25;
int in2 = 26;

//Motor B

int enB = 15;
int in3 = 19;
int in4 = 18;

// defines pins numbers
const int trigPin = 2;
const int echoPin = 4;

signed int motor1Speed;
int encoder_pin = 21;  // The pin the encoder is connected           
unsigned int rpm;     // rpm reading
volatile byte pulses;  // number of pulses
unsigned long timeold; 
// The number of pulses per revolution
// depends on your index disc!!
unsigned int pulsesperturn = 20;

// defines variables
long duration;
int distance;

String payload;
int minspeedr = 185;
int minspeedl = 185;
int maxspeedr = 225;
int maxspeedl = 225;
int nospeed = 0;

const char* ssid = "CAISER";
const char* password =  "caiser123";

// URL link to Post Ultrasonic
String serverName = "http://thingssentral.io/postlong?data=AccessKey|AMZIE2021@0019001010103|";

unsigned long lastTime = 0;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

void moveControl()
{
  Ultrasonic();
  encoder();
  if (distance <= 30)
  {
    //Move Backward
    if (payload.substring(14, 15) == "2")
    {
      digitalWrite(in2, HIGH);
      digitalWrite(in1, LOW);
      digitalWrite(in4, HIGH);
      digitalWrite(in3, LOW);
      analogWrite(enA, maxspeedr);
      analogWrite(enB, maxspeedl);
    }

    //Move Backward Right
    else if (payload.substring(14, 15) == "4")
    {
      digitalWrite(in2, HIGH);
      digitalWrite(in1, LOW);
      digitalWrite(in4, HIGH);
      digitalWrite(in3, LOW);
      analogWrite(enA, minspeedr);
      analogWrite(enB, maxspeedl);
    }

    //Move Backward Left
    else if (payload.substring(14, 15) == "6")
    {
      digitalWrite(in2, HIGH);
      digitalWrite(in1, LOW);
      digitalWrite(in4, HIGH);
      digitalWrite(in3, LOW);
      analogWrite(enA, maxspeedr);
      analogWrite(enB, minspeedl);
    }
    else
    {
      analogWrite(enA, nospeed);
      analogWrite(enB, nospeed);
    }
  }
  else if (distance > 30)
  {

    //Move Forward
    if (payload.substring(14, 15) == "1")
    {
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
      analogWrite(enA, maxspeedr);
      analogWrite(enB, maxspeedl);
    }

    //Move Forward Right
    else if (payload.substring(14, 15) == "3")
    {
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
      analogWrite(enA, minspeedr);
      analogWrite(enB, maxspeedl);
    }

    //Move Forward Left
    else if (payload.substring(14, 15) == "5")
    {
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
      analogWrite(enA, maxspeedr);
      analogWrite(enB, minspeedl);
    }

    //No Move
    else if (payload.substring(14, 15) == "0")
    {
      analogWrite(enA, nospeed);
      analogWrite(enB, nospeed);
    }

    //Move Backward
    else if (payload.substring(14, 15) == "2")
    {
      digitalWrite(in2, HIGH);
      digitalWrite(in1, LOW);
      digitalWrite(in4, HIGH);
      digitalWrite(in3, LOW);
      analogWrite(enA, maxspeedr);
      analogWrite(enB, maxspeedl);
    }

    //Move Backward Right
    else if (payload.substring(14, 15) == "4")
    {
      digitalWrite(in2, HIGH);
      digitalWrite(in1, LOW);
      digitalWrite(in4, HIGH);
      digitalWrite(in3, LOW);
      analogWrite(enA, minspeedr);
      analogWrite(enB, maxspeedl);
    }

    //Move Backward Left
    else if (payload.substring(14, 15) == "6")
    {
      digitalWrite(in2, HIGH);
      digitalWrite(in1, LOW);
      digitalWrite(in4, HIGH);
      digitalWrite(in3, LOW);
      analogWrite(enA, maxspeedr);
      analogWrite(enB, minspeedl);
    }
  }
}


void Ultrasonic ()
{
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculating the distance
  distance = duration * 0.034 / 2;

  // Prints the distance on the Serial Monitor
  //Serial.print("Distance: ");
  //Serial.println(distance);
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;

      String serverPath = serverName + distance;

      // Your Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());

      // Send HTTP GET request
      int httpResponseCode = http.GET();

      if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

void encoder()
{
   if (millis() - timeold >= 1000){  /*Update every one second, this will be equal to reading frecuency (Hz).*/
 
  //Don't process interrupts during calculations
   detachInterrupt(0);
   //Note that this would be 60*1000/(millis() - timeold)*pulses if the interrupt
   //happened once per revolution
   rpm = (60 * 1000 / pulsesperturn )/ (millis() - timeold)* pulses;
   timeold = millis();
   pulses = 0;
   
   //Write it out to serial port
   Serial.print("RPM = ");
   Serial.println(rpm,DEC);
   //Restart the interrupt processing
   attachInterrupt(encoder_pin, counter, FALLING);
   //int potvalue = analogRead(1);  // Potentiometer connected to Pin A1
    //int motorspeed = map(potvalue, 0, 1023, 255, 0);
  //analogWrite(motor1Speed, motorspeed); 
  digitalWrite(motor1Speed, 1);
   }
}

 void counter()
 {
    //Update count
      pulses++;    
 }


void setup()
{

  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  pinMode(encoder_pin, INPUT);
  pinMode(motor1Speed , OUTPUT); 
  attachInterrupt(encoder_pin, counter, FALLING);
  pulses = 0;
  rpm = 0;
  timeold = 0;

  Serial.begin(115200);
  delay(4000);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");

}

void loop()
{

  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status

    HTTPClient http;

    http.begin("http://thingssentral.io/ReadNode?Params=tokenid|xxx@NodeId|0019001010102"); //Specify the URL
    int httpCode = http.GET();                                        //Make the request

    if (httpCode > 0)
    { //Check for the returning code
      payload = http.getString();
      Serial.println(httpCode);
      Serial.println(payload.substring(14, 15));
      //Ultrasonic();
      moveControl();
    }

    else {
      Serial.println("Error on HTTP request");
    }

    http.end(); //Free the resources
  }

  

}
