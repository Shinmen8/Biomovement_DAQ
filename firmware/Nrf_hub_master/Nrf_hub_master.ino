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



RF24 radio(9,10);

// Radio pipe addresses for the 2 nodes to communicate.
//const uint64_t pipes[6] = { 0xF0F0F0F0D2LL, 0xF0F0F0F0E1LL, 0xF0F0F0F0E2LL, 0xF0F0F0F0E3LL, 0xF0F0F0F0E4LL, 0xF0F0F0F0E5LL };
// bytes serv1 = 0x7365727631 in hex
const uint64_t talking_pipes[5] =   { 0xF0F0F0F0D2LL, 0xF0F0F0F0C3LL, 0xF0F0F0F0B4LL, 0xF0F0F0F0A5LL, 0xF0F0F0F096LL };
const uint64_t listening_pipes[5] = { 0x3A3A3A3AD2LL, 0x3A3A3A3AC3LL, 0x3A3A3A3AB4LL, 0x3A3A3A3AA5LL, 0x3A3A3A3A96LL };


void setup(void)
{
  digitalWrite(2,HIGH);
  delay(500);
  digitalWrite(2,LOW);



  Serial.begin(115200);
  printf_begin();
  //while (!Serial) ;
  radio.begin();

  radio.setDataRate(RF24_2MBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(70);
  radio.enableDynamicPayloads();
  radio.setRetries(1,10);
  radio.enableAckPayload();
  radio.setCRCLength(RF24_CRC_16);

  //radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1,talking_pipes[0]);
    radio.openReadingPipe(2,talking_pipes[1]);
    radio.openReadingPipe(3,talking_pipes[2]);
    radio.openReadingPipe(4,talking_pipes[3]);
    radio.openReadingPipe(5,talking_pipes[4]);

  radio.startListening();
  radio.printDetails();

  delay(1000);

}

void loop(void)
{
  char receivePayload[31];
  uint8_t len = 0;
  uint8_t pipe = 1;

  // Loop thru the pipes 0 to 5 and check for payloads
  if ( radio.available( &pipe)  ) {

    len = radio.getDynamicPayloadSize();
    radio.read( &receivePayload,len);

    // Sending back reply to sender using the same pipe
    radio.stopListening();
    radio.openWritingPipe(listening_pipes[pipe-1]);
      // Open the correct pipe for writing
    //radio.openWritingPipe(pipes[1]);
    //radio.write(receivePayload,len);

    if ( radio.write( receivePayload, len) ) {
       printf("Send successful\n\r"); 

    }
    else {
       printf("Send failed------------------------------------------\n\r");

    }

    // Format string for printing ending with 0
    receivePayload[len] = 0;
    printf("Got payload: %s len:%i pipe:%i\n\r",receivePayload,len,pipe);

     //radio.openReadingPipe(1,pipes[1]);
    radio.startListening();

    // Increase pipe and reset to 0 if more than 5
    pipe++;
    if ( pipe > 5 ) pipe = 0;

  }

  //delay(1);

}


