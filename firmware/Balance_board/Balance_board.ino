/*
  Analog input, analog output, serial output
 
 Reads an analog input pin, maps the result to a range from 0 to 255
 and uses the result to set the pulsewidth modulation (PWM) of an output pin.
 Also prints the results to the serial monitor.
 
 The circuit:
 * potentiometer connected to analog pin 0.
   Center pin of the potentiometer goes to the analog pin.
   side pins of the potentiometer go to +5V and ground
 * LED connected from digital pin 9 to ground
 
 created 29 Dec. 2008
 modified 9 Apr 2012
 by Tom Igoe
 
 This example code is in the public domain.
 
 */

#define Fx A2
#define Fy A1
#define Fz A0
#define Mx A3
#define My A4
#define Mz A5


// These constants won't change.  They're used to give names
// to the pins used:
    // value output to the PWM (analog out)

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
}

void loop() {
  // read the analog in value:
  int sensorValue0 = analogRead(Fx);  
  int sensorValue1 = analogRead(Fy);
  int sensorValue2 = analogRead(Fz);  
  int sensorValue3 = analogRead(Mx);  
  int sensorValue4 = analogRead(My);  
  int sensorValue5 = analogRead(Mz);  
  // map it to the range of the analog out:
  //outputValue = map(sensorValue, 0, 1023, 0, 255);  
  // change the analog out value:
  //analogWrite(analogOutPin, outputValue);           

  // print the results to the serial monitor:
  Serial.print("V" );                       
  Serial.print(sensorValue0);      
  Serial.print(" ,");      
  Serial.print(sensorValue1);
  Serial.print(" ,");      
  Serial.print(sensorValue2);
  Serial.print(" ,");  
  Serial.print(sensorValue3);      
  Serial.print(" ,");      
  Serial.print(sensorValue4);
  Serial.print(" ,");      
  Serial.println(sensorValue5);

  // wait 2 milliseconds before the next loop
  // for the analog-to-digital converter to settle
  // after the last reading:
  delay(20);                     
}
