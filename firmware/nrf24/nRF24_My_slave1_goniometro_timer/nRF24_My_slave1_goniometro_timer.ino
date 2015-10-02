/*

goniometro
adc 5v
timer 200samples
packet




 This program sends readings from four or more sensor readings and appends
 2 bytes addr data pipes to the beginning of the payloads. The sender will send and
 receive the payload on the same sender/receiver address.

 The receiver is a RPi or UNO accepting 6 pipes and display received payload to the screen

 The receiver will return the receive payload for sender to calculate the rtt
 if the string compared matched to the lcd display

 Max payload size is 32 bytes

Forked RF24 at github :-
https://github.com/stanleyseow/RF24

 Date : 28/03/2013

 Written by Stanley Seow
 stanleyseow@gmail.com
*/
//Got payload: E1,061,0000,0024,434 len:20 pipe:1

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
#include <TimerOne.h>
#define RF_SETUP 0x17




// Set up nRF24L01 radio on SPI pin for CE, CSN
RF24 radio(9,10);

// For best performance, use P1-P5 for writing and Pipe0 for reading as per the hub setting
// Below is the settings from the hub/receiver listening to P0 to P5
//const uint64_t pipes[6] = { 0x7365727631LL, 0xF0F0F0F0E1LL, 0xF0F0F0F0E2LL, 0xF0F0F0F0E3LL, 0xF0F0F0F0E4LL, 0xF0F0F0F0E5LL };
// Example below using pipe5 for writing
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0x7365727631LL };

// const uint64_t pipes[2] = { 0xF0F0F0F0E2LL, 0xF0F0F0F0E2LL };
// const uint64_t pipes[2] = { 0xF0F0F0F0E3LL, 0xF0F0F0F0E3LL };
// const uint64_t pipes[2] = { 0xF0F0F0F0E4LL, 0xF0F0F0F0E4LL };
// const uint64_t pipes[2] = { 0xF0F0F0F0E5LL, 0xF0F0F0F0E5LL };
// Pipe0 is F0F0F0F0D2 ( same as reading pipe )

char receivePayload[32];
uint8_t counter=0;
uint16_t Data1,Data2 = 0;
bool timer_flag=0;




void setup(void)
{

  // Setup LCD
  
  Serial.begin(115200);
  
  printf_begin();
  printf("Sending nodeID & 4 sensor data\n\r");

  radio.begin();

  // Enable this seems to work better
  radio.enableDynamicPayloads();

  radio.setDataRate(RF24_2MBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(76);
  radio.setRetries(15,15);
  //radio.disableCRC();
  radio.openWritingPipe(pipes[0]); 
  radio.openReadingPipe(1,pipes[1]); 

      
  // Send only, ignore listening mode
  //radio.startListening();

  // Dump the configuration of the rf unit for debugging
  radio.printDetails(); 
  delay(2000);

  Timer1.initialize(5000); // set a timer of length 100000 microseconds (or 0.1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
  Timer1.attachInterrupt( timerIsr );
}

void loop(void)
{
      

  if (timer_flag==1){
  char temp[5];
  bool timeout=0;
  uint16_t nodeID = pipes[0] & 0xff; // Use the last 2 pipes address as nodeID
  char outBuffer[32]=""; // Clear the outBuffer before every loop
  unsigned long send_time, rtt = 0;
    
    // Get readings from sensors, change codes below to read sensors

    
//    Data3 = analogRead(1);
//    Data4 = random(0,1000);
    
    if ( counter > 999 ) counter = 0;

    // Append the hex nodeID to the beginning of the payload    
    //sprintf(outBuffer,"%2X",nodeID);   
    sprintf(outBuffer,"A");  
    //strcat(outBuffer,",");
    // Convert int to strings and append with zeros if number smaller than 3 digits
    // 000 to 999
    
    //sprintf(temp,"%03d",Data1);  
    //strcat(outBuffer,temp);
   
    //strcat(outBuffer,",");
    
    sprintf(temp,"%d",Data2);
    strcat(outBuffer,temp);    
//    strcat(outBuffer,",");
//
//    sprintf(temp,"%04d",Data3);
//    strcat(outBuffer,temp);
//   
//    strcat(outBuffer,",");
//   
//    sprintf(temp,"%03d",Data4);
//    strcat(outBuffer,temp); 

    // Test for max payload size
    //strcat(outBuffer,"012345678901");
    
    // End string with 0
    // strcat(outBuffer,0);
            
  // printf("outBuffer: %s len: %d\n\r",outBuffer, strlen(outBuffer));
    
    send_time = millis();
    
//    
//   radio.write( outBuffer, strlen(outBuffer));
 // delay(10);
    
    
    
    // Stop listening and write to radio 
    radio.stopListening();   
    // Send to hub
    if ( radio.write(outBuffer, strlen(outBuffer)) ) {
      // printf("Send successful\n\r"); 

    }
    else {
     //  printf("Send failed\n\r");

    }  
 //   delay(5); 
timer_flag=0;
    
    radio.startListening();
    //delay(5);  

//  while ( radio.available() && !timeout ) {
//
//         uint8_t len = radio.getDynamicPayloadSize();
//         radio.read( receivePayload, len); 
//         
//         receivePayload[len] = 0;
//        printf("inBuffer:  %s\n\r",receivePayload);
//         
//         //lcd.setCursor(2,1);
//         //lcd.print(receivePayload);
//        
//         // Compare receive payload with outBuffer        
//         if ( ! strcmp(outBuffer, receivePayload) ) {
//             rtt = millis() - send_time;
//
//            // printf("inBuffer --> rtt: %i \n\r",rtt);            
//
//             // Turn on buzzer to Pin 2
//             digitalWrite(2,HIGH);
//
//            // lcd.setCursor(0,1);
//           //  lcd.print("                ");
//
//           //  lcd.setCursor(0,1);
//           //  lcd.print(rtt);
//         }       
//    
//    // Check for timeout and exit the while loop
//    if ( millis() - send_time > radio.getMaxTimeout() ) {
//         //Serial.println("Timeout!!!");
//         timeout = 1;
//     }          
//      
//     delay(5);
//   } // End while  
//     
//   // delay(250);
//  //  digitalWrite(2,LOW); // Off the buzzer
  
  
  
  }
    
  }


void timerIsr()
{
    Data1 = counter++;
    Data2 = analogRead(3);
    timer_flag=1;
}


