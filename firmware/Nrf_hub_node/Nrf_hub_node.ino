/*
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


#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

//#define RF_SETUP 0x17

#define node 1 //1to5
// Set up nRF24L01 radio on SPI pin for CE, CSN
RF24 radio(9,10);

// For best performance, use P1-P5 for writing and Pipe0 for reading as per the hub setting
// Below is the settings from the hub/receiver listening to P0 to P5
//const uint64_t pipes[6] = { 0x7365727631LL, 0xF0F0F0F0E1LL, 0xF0F0F0F0E2LL, 0xF0F0F0F0E3LL, 0xF0F0F0F0E4LL, 0xF0F0F0F0E5LL };
// Example below using pipe5 for writing
//const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0x7365727631LL };



//const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0x7365727631LL };

const uint64_t talking_pipes[5] = {
  0xF0F0F0F0D2LL, 0xF0F0F0F0C3LL, 0xF0F0F0F0B4LL, 0xF0F0F0F0A5LL, 0xF0F0F0F096LL };
const uint64_t listening_pipes[5] = {
  0x3A3A3A3AD2LL, 0x3A3A3A3AC3LL, 0x3A3A3A3AB4LL, 0x3A3A3A3AA5LL, 0x3A3A3A3A96LL };



// const uint64_t pipes[2] = { 0xF0F0F0F0E2LL, 0xF0F0F0F0E2LL };
// const uint64_t pipes[2] = { 0xF0F0F0F0E3LL, 0xF0F0F0F0E3LL };
// const uint64_t pipes[2] = { 0xF0F0F0F0E4LL, 0xF0F0F0F0E4LL };
// const uint64_t pipes[2] = { 0xF0F0F0F0E5LL, 0xF0F0F0F0E5LL };
// Pipe0 is F0F0F0F0D2 ( same as reading pipe )

char receivePayload[32];
uint8_t counter=0;

void setup(void)
{

  // Setup LCD


  Serial.begin(115200);

  printf_begin();
  printf("Sending nodeID & 4 sensor data\n\r");

  radio.begin();

  // Enable this seems to work better


  radio.setDataRate(RF24_2MBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(70);
  radio.enableDynamicPayloads();
  radio.setRetries(0,15);
  radio.setAutoAck(1);  
  radio.enableAckPayload();
  radio.setCRCLength(RF24_CRC_16);
  
  
  radio.openWritingPipe(talking_pipes[node-1]);
  radio.openReadingPipe(1,listening_pipes[node-1]);


  // Send only, ignore listening mode
  //radio.startListening();

  // Dump the configuration of the rf unit for debugging
  radio.printDetails();
  delay(1000);
}

void loop(void)
{
  uint8_t Data1,Data2,Data3,Data4 = 0;
  char temp[5];


  // Get the last two Bytes as node-id


  // Use the last 2 pipes address as nodeID
  // sprintf(nodeID,"%X",pipes[0]);

  char outBuffer[32]=""; // Clear the outBuffer before every loop
  unsigned long send_time, rtt = 0;

  // Get readings from sensors, change codes below to read sensors
  Data1 = counter++;
  //Data2 = analogRead(0);
  //Data3 = analogRead(1);
  //Data4 = random(0,1000);

  if ( counter > 999 ) counter = 0;

  // Append the hex nodeID to the beginning of the payload
  //sprintf(outBuffer,"%2X",node);

  //strcat(outBuffer,",");

  // Convert int to strings and append with zeros if number smaller than 3 digits
  // 000 to 999

  sprintf(temp,"%03d",Data1);
  strcat(outBuffer,temp);

  //strcat(outBuffer,",");

  //sprintf(temp,"%04d",Data2);
  //strcat(outBuffer,temp);

  //strcat(outBuffer,",");

  //sprintf(temp,"%04d",Data3);
  //strcat(outBuffer,temp);

  // strcat(outBuffer,",");

  //sprintf(temp,"%03d",Data4);
  //strcat(outBuffer,temp);

  // Test for max payload size
  //strcat(outBuffer,"012345678901");


  // End string with 0
  // strcat(outBuffer,0);

  printf("outBuffer: %s len: %d\n\r",outBuffer, strlen(outBuffer));

  //lcd.clear();
  //lcd.setCursor(0,0);

  //send_time = millis();

  // Stop listening and write to radio
  radio.stopListening();

  // Send to hub
  if ( radio.write( outBuffer, strlen(outBuffer)) ) {
    printf("Send successful\n\r");

  }
  else {
    printf("Send failed---------------------------------------------------\n\r");

  }

  radio.startListening();
  delay(1);

  //    while ( radio.available() ) {
  //
  //         uint8_t len = radio.getDynamicPayloadSize();
  //         radio.read( receivePayload, len);
  //
  //         receivePayload[len] = 0;
  //         printf("inBuffer:  %s\n\r",receivePayload);
  //
  //
  //     delay(1);
  //   } // End while

 //delay(250);
         uint8_t len = radio.getDynamicPayloadSize();
         radio.read( receivePayload, len);

         receivePayload[len] = 0;
         printf("inBuffer:  %s\n\r",receivePayload);

}







