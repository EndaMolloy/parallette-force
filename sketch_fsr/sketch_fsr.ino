#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic


int forcePin = A0;
int forceReading;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  WiFiManager wifiManager;
  //reset saved settings
 
  wifiManager.autoConnect("AutoConnectAP");

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
}

void loop() {
  // put your main code here, to run repeatedly:
  
  forceReading = analogRead(forcePin);
  Serial.print("Analog reading = ");
  Serial.println(forceReading);
 
  delay(500);

  if(forceReading > 800){
    timer();
    }
  
}

void timer(){

  int seconds = 0;
  Serial.print("Seconds = ");
  Serial.println(seconds);
  delay(1000);
    
  do {
    seconds++;
    Serial.print("Seconds = ");
    Serial.println(seconds);
    delay(1000); 
    forceReading = analogRead(forcePin);
  } while (forceReading > 800 );

  seconds++;
  Serial.print("Seconds = ");
  Serial.println(seconds);
  
  Serial.print("Total = ");
  Serial.println(seconds);
}
