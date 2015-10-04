/*
 This program is the receiver for nRF24_Send_to_RPi that was originally written 
 for the Raspberry Pi and ported back to UNO for completeness.

 Found out that I cannot have two startListening() statement, one in setup() and another
 inside the loop() and the radio.available() will not received anything at all.

 The receiver will accept 6 pipes and display received payload to the screen

 The receiver will return the receive payload to the sender usinbg the same pipe
 to calculate the rtt if the payload matched

 Max payload size is 32 bytes

Forked RF24 at github :-
https://github.com/stanleyseow/RF24

 Date : 08/05/2013
 Written by Stanley Seow
 stanleyseow@gmail.com
*/

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
int i=0;
//LiquidCrystal lcd(10, 7, 3, 4, 5, 6);

RF24 radio(9,10);

// Radio pipe addresses for the 2 nodes to communicate.
//const uint64_t pipes[6] = { 0xF0F0F0F0D2LL, 0xF0F0F0F0E1LL, 0xF0F0F0F0E2LL, 0xF0F0F0F0E3LL, 0xF0F0F0F0E4LL, 0xF0F0F0F0E5LL };
// bytes serv1 = 0x7365727631 in hex 
const uint64_t pipes[6] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0E1LL, 0xF0F0F0F0E2LL, 0xF0F0F0F0E3LL, 0xF0F0F0F0E4LL, 0xF0F0F0F0E5LL };


void setup(void)
{
  
  digitalWrite(2,HIGH);
  delay(500);
  digitalWrite(2,LOW);
  

  Serial.begin(115200);
  printf_begin();

  radio.begin();
  	
  radio.setDataRate(RF24_2MBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(76);
  //radio.disableCRC();
  radio.enableDynamicPayloads();
  radio.setRetries(15,15);
  
  radio.setCRCLength(RF24_CRC_16);

  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);
  radio.openReadingPipe(2,pipes[2]);
  radio.openReadingPipe(3,pipes[3]);
  radio.openReadingPipe(4,pipes[4]);
  radio.openReadingPipe(5,pipes[5]);

  
  radio.printDetails();
  radio.startListening();
  delay(2000);

}

void loop(void)
{ 
    char receivePayload[32]="";
    uint8_t len = 0;
    uint8_t pipe = 0;
        
    if( Serial.available()) { 
    radio.stopListening(); 
    radio.write( "123", 3);  
    Serial.read ();
    Serial.println("communication!");
     radio.startListening();
    }   
        
        
    // Loop thru the pipes 0 to 5 and check for payloads    
    if ( radio.available( &pipe ) ) {
      bool done = false;
      while (!done)
      {
        
         
        //radio.openWritingPipe(pipes[pipe]);
        //radio.write(receivePayload,len);
        len = radio.getDynamicPayloadSize();  
        done = radio.read(&receivePayload,len);
        
        // Sending back reply to sender using the same pipe
        radio.stopListening();
        // Format string for printing ending with 0
        receivePayload[len] = 0;
       // printf("Got: %s len:%i pipe:%i\n\r",receivePayload,len,pipe);
        if (receivePayload[2]<48 || receivePayload[2]>57 ){
         //printf("error");
          
        } 
        else{
        //printf("%s\n\r",receivePayload);
        Serial.println(receivePayload);
        }
        
    
        radio.startListening();
        
        // Increase pipe and reset to 0 if more than 5
        pipe++;
        if ( pipe > 5 ) pipe = 0;
      }

    }

//delay(1);

}
