#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include "HTTPSRedirect.h"
#include "credentials.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. ...
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Enter Google Script ID:
const char *GScriptId = G_SCRIPT_ID;

// Enter command (insert_row or append_row) and your Google Sheets sheet name (default is Sheet1):
// String payload_base =  "{\"command\": \"append_row\", \"sheet_name\": \"Sheet1\", \"values\": ";
String payload_base =  "{\"command\": \"append_row\", \"sheet_name\": ";
String payload = "";


const char* host = "script.google.com";
const int httpsPort = 443;
const char* fingerprint = "";
const int buttonPin = 12;
String url = String("/macros/s/") + GScriptId + "/exec";

HTTPSRedirect* client = nullptr;

// Declare variables that will be published to Google Sheets
int value0 = 0;
int forcePin = A0;
int forceReading;

// Button variables
int buttonPushCounter = 0;   // counter for the number of button presses
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button
String user = "Enda";

void connect(){

  WiFiManager wifiManager;
  //reset saved settings
 
  wifiManager.autoConnect("AutoConnectAP");

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());

  // Use HTTPSRedirect class to create a new TLS connection
  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");
  
  Serial.print("Connecting to ");
  Serial.println(host);

  // Try to connect for a maximum of 5 times
  bool flag = false;
  for (int i=0; i<5; i++){
    int retval = client->connect(host, httpsPort);
    if (retval == 1) {
       flag = true;
       Serial.println("WiFi Connected");
       break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }

  if (!flag){
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    return;
  }


  // delete HTTPSRedirect object
  delete client;
  client = nullptr;
  
}

void setup() {    
  // put your setup code here, to run once:
  Serial.begin(9600);

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.setRotation(2); 

  pinMode(buttonPin, INPUT);
  
  connect();
  
  display.clearDisplay();
}



void loop() {

  static bool flag = false;
  if (!flag){
    client = new HTTPSRedirect(httpsPort);
    client->setInsecure();
    flag = true;
    client->setPrintResponseBody(true);
    client->setContentTypeHeader("application/json");
  }
  if (client != nullptr){
    if (!client->connected()){
      client->connect(host, httpsPort);
    }
  }
  else{
    Serial.println("Error creating client object!");
  }

// User selection

// read the pushbutton input pin:
  buttonState = digitalRead(buttonPin);
  Serial.print("User: ");
  Serial.println(buttonPushCounter);
  // compare the buttonState to its previous state
  if (buttonState != lastButtonState) {
    // if the state has changed, increment the counter
    if (buttonState == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      buttonPushCounter++;
      Serial.println("on");
      Serial.print("User: ");
      Serial.println(buttonPushCounter);
    } else {
      // if the current state is LOW then the button went from on to off:
      Serial.println("off");
    }
    // Delay a little bit to avoid bouncing
    delay(50);
  }
  // save the current state as the last state, for next time through the loop
  lastButtonState = buttonState;
  
  if (buttonPushCounter > 1){
      buttonPushCounter = 0;
  }  

  if (buttonPushCounter == 0 ){
      user = "Enda";
    }
  else{
      user = "Other"; 
  }
    
  // Clear the buffer.
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.setTextColor(WHITE);
  display.println("WiFi-Connected");
  display.println();
  display.setTextSize(2);
  display.println("User: ");
  display.println(user);
  display.display();  

  forceReading = analogRead(forcePin);
  Serial.print("Analog reading = ");
  Serial.println(forceReading);
  //delay(1000);
  

  if(forceReading > 800){
    timer();
    }
  
}


void timer(){
  display.clearDisplay();
  display.display();
  display.setTextSize(5);
  display.setCursor(0, 0);
  int seconds = 0;
  Serial.print("Seconds = ");
  Serial.println(seconds);
  display.println(seconds);
  display.display();
  delay(1000);
    
  do {
    display.clearDisplay();
    display.setTextSize(5);
    display.setCursor(0, 0);
    display.setTextColor(WHITE);
    seconds++;
    Serial.print("Seconds = ");
    Serial.println(seconds);
    display.println(seconds);
//    display.flip();
    display.display();
    delay(1000); 
    forceReading = analogRead(forcePin);
  } while (forceReading > 800 );

  Serial.print("Seconds = ");
  Serial.println(seconds);
  display.println(seconds);
  delay(1000);
  display.clearDisplay();
  
  Serial.print("Total = ");
  Serial.println(seconds);
  display.setCursor(0, 0);
  display.setTextSize(3);
  display.println("Total: ");
  display.print(seconds);
//  display.flip();
  display.display();
  
  // Create json object string to send to Google Sheets
  payload = payload_base + "\"" + user  + "\", \"values\": " + "\"" + seconds  + "\"}";
  
  // Publish data to Google Sheets
  Serial.println("Publishing data...");
  if(client->POST(url, host, payload)){ 
    // do stuff here if publish was successful
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.println("Time");
    display.println("Logged");
    display.display();
  }
  else{
    // do stuff here if publish was not ful
    Serial.println("Error while connecting");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.println("Error");
    display.println("Logging");
    display.println("Time");
    display.display();
  }

// a delay of several seconds is required before publishing again    
delay(5000);
  
}
