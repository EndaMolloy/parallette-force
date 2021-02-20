
int forcePin = A0;
int forceReading;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
}

void loop() {
  // put your main code here, to run repeatedly:
  
  forceReading = analogRead(forcePin);
  //Serial.print("Analog reading = ");
  //Serial.println(forceReading);
 
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
