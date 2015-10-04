/*
 This program sends readings from four or more sensor readings and appends
 2 bytes addr data pipes to the beginning of the payloads. The sender will send and
 receive the payload on the same sender/receiver address.

 The receiver is a RPi or UNO accepting 6 pipes and display received payload to the screen

 The receiver will return the receive payload for sender to calculate the rtthe string compared matched to the lcd display

 Max payload size is 32 bytes

 Forked RF24 at github :-
 https://github.com/stanleyseow/RF24

 Date : 28/03/2013

 Written by Stanley Seow
 stanleyseow@gmail.com
 */
#include "config.h"
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

//#define RF_SETUP 0x17


// Set up nRF24L01 radio on SPI pin for CE, CSN
RF24 radio(9, 10);

// For best performance, use P1-P5 for writing and Pipe0 for reading as per the hub setting
// Below is the settings from the hub/receiver listening to P0 to P5
//const uint64_t pipes[6] = { 0x7365727631LL, 0xF0F0F0F0E1LL, 0xF0F0F0F0E2LL, 0xF0F0F0F0E3LL, 0xF0F0F0F0E4LL, 0xF0F0F0F0E5LL };
// Example below using pipe5 for writing
//const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0x7365727631LL };
const char NUMBER_OF_FIELDS = 5; // how many comma separated fields we expect
int  values[NUMBER_OF_FIELDS];


//const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0x7365727631LL };

const uint64_t talking_pipes[5] = {
  0xF0F0F0F0D2LL, 0xF0F0F0F0C3LL, 0xF0F0F0F0B4LL, 0xF0F0F0F0A5LL, 0xF0F0F0F096LL
};
const uint64_t listening_pipes[5] = {
  0x3A3A3A3AD2LL, 0x3A3A3A3AC3LL, 0x3A3A3A3AB4LL, 0x3A3A3A3AA5LL, 0x3A3A3A3A96LL
};
unsigned long send_time = 0, previousMillis = 0, rtt = 0;


// const uint64_t pipes[2] = { 0xF0F0F0F0E2LL, 0xF0F0F0F0E2LL };
// const uint64_t pipes[2] = { 0xF0F0F0F0E3LL, 0xF0F0F0F0E3LL };
// const uint64_t pipes[2] = { 0xF0F0F0F0E4LL, 0xF0F0F0F0E4LL };
// const uint64_t pipes[2] = { 0xF0F0F0F0E5LL, 0xF0F0F0F0E5LL };
// Pipe0 is F0F0F0F0D2 ( same as reading pipe )

char receivePayload[32];
uint8_t counter = 0;




boolean get_values(char strValue[30] ) {


  char fieldIndex = 0; // the current field being received



  for (int i = 0; i <= fieldIndex; i++) { //Serial.println(values[i]);
    values[i] = 0; // set the values to zero, ready for the next message
  }
  fieldIndex = 0; // ready to start over
  //  for ( char i=0; i<strlen(strValue); i++){
  //    Serial.print(strValue[i]);
  //  }
  //  Serial.println();
  // if (get_string()==1){///read a string of alphanumeric
  for (char i = 1; i < strlen(strValue) + 1; i++) {

    char ch = strValue[i];
    if (ch >= '0' && ch <= '9') // is this an ascii digit between 0 and 9?
    {
      // yes, accumulate the value
      values[fieldIndex] = (values[fieldIndex] * 10) + (ch - '0');
    }
    else if (ch == '/') // comma is our separator, so move on to the next field
    {
      if (fieldIndex < NUMBER_OF_FIELDS - 1)
        fieldIndex++; // increment field index
    }
    else
    {
      //      Serial.print("1= ");
      //      Serial.println(values[0]);
      //      Serial.print("2= ");
      //      Serial.println(values[1]);
      //      Serial.print("3= ");
      //      Serial.println(values[2]);
      //      Serial.print("4= ");
      //      Serial.println(values[3]);
      //      Serial.print("5= ");
      //      Serial.println(values[4]);

    }
  }

  return (1);

}






void setup(void)
{

  // Setup LCD


  Serial.begin(38400);

  printf_begin();
  printf("Sending nodeID & 4 sensor data\n\r");

  radio.begin();

  // Enable this seems to work better


  radio.setDataRate(RF24_2MBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(70);
  radio.enableDynamicPayloads();
  radio.setRetries(0, 15);
  radio.setAutoAck(1);
  radio.enableAckPayload();
  radio.setCRCLength(RF24_CRC_16);


  radio.openWritingPipe(talking_pipes[node - 1]);
  radio.openReadingPipe(1, listening_pipes[node - 1]);


  // Send only, ignore listening mode
  //radio.startListening();

  // Dump the configuration of the rf unit for debugging
  radio.printDetails();
  delay(1000);
}

void loop(void)
{

  command_received();
  delay(250); /// !!!!!!!!!!!!!!! this delay must be after the command receive if you send more data in this delay time then the data wont be sent


  uint8_t Data1, Data2, Data3, Data4 = 0;
  char temp[5];


  // Get the last two Bytes as node-id


  // Use the last 2 pipes address as nodeID
  // sprintf(nodeID,"%X",pipes[0]);

  char outBuffer[10] = ""; // Clear the outBuffer before every loop


  // Get readings from sensors, change codes below to read sensors
  //Data1 = counter++;
  //Data2 = analogRead(0);
  //Data3 = analogRead(1);
  //Data4 = random(0,1000);

  if ( counter > 999 ) counter = 0;

  // Append the hex nodeID to the beginning of the payload
  //sprintf(outBuffer,"%2X",node);

  //strcat(outBuffer,",");

  // Convert int to strings and append with zeros if number smaller than 3 digits
  // 000 to 999
  // send_time = millis();
  unsigned long currentMillis = millis();


  send_time = (unsigned long)(currentMillis - previousMillis);
  previousMillis = currentMillis;

  //Serial.println(send_time);
  //  sprintf(temp,"%04d",send_time);
  //  strcat(outBuffer,temp);

  //strcat(outBuffer,",");

  sprintf(temp, "%04d", send_time);
  strcat(outBuffer, temp);

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


  //printf("outBuffer: %s len: %d\n\r",outBuffer, strlen(outBuffer));

  //lcd.clear();
  //lcd.setCursor(0,0);



  // Stop listening and write to radio
  radio.stopListening();

  // Send to hub
  if ( radio.write( outBuffer, strlen(outBuffer)) ) {
    printf("Send successful\n\r");

    Serial.println(outBuffer);
  }
  else {
    printf("Send failed---------------------------------------------------\n\r");

  }



  //  //delay(1);
  //
  //  //    while ( radio.available() ) {
  //  //
  //  //         uint8_t len = radio.getDynamicPayloadSize();
  //  //         radio.read( receivePayload, len);
  //  //
  //  //         receivePayload[len] = 0;
  //  //         printf("inBuffer:  %s\n\r",receivePayload);
  //  //
  //  //
  //  //     delay(1);
  //  //   } // End while
  //
  
  //         uint8_t len = radio.getDynamicPayloadSize();
  //         radio.read( receivePayload, len);
  //
  //         receivePayload[len] = 0;
  //         printf("inBuffer:  %s\n\r",receivePayload);

}


void command_received() {

  radio.startListening();
  delay(1);
  if ( radio.available() ) {
    
    //    for (char i=0; i<strlen(receivePayload); i++){
    //    receivePayload[i]=0;
    //    Serial.println(receivePayload[i]);
    //    }
    //char receivePayload[]="";
    uint8_t len = radio.getDynamicPayloadSize();
    radio.read( receivePayload, len);
    printf("inBuffer:%s\n\r", receivePayload);
    get_values(receivePayload);
    if (command_type == cmd_request_firmware_edition) {
      //  command_respond_firmware();
      Serial.println("FIRMWARE");
    }
    //    else if(command_type==cmd_change_color){////////////read header of string and the numerical data
    //      command_change_color();
    //    }
    //    else if(command_type==cmd_analog_mode_start){////////////read header of string and the numerical data
    //      analog_mode_flag=1;
    //      analog_threshold=values[1];
    //    }
    //    else if(command_type==cmd_analog_read_stop){////////////read header of string and the numerical data
    //      analog_mode_flag=0;
    //      maxValue_mode_flag=0;
    //      analog_threshold=0;
    //    }
    //    else if(command_type==cmd_maxValue_mode_start){////////////read header of string and the numerical data
    //      maxValue_mode_flag=1;
    //      analog_threshold=values[1];
    //      maxValue_1=analog_threshold;
    //    }

  }
}


void command_respond_firmware() {
  // char Buffer[20]="F1/firmware 0.1";
  char Buffer[10] = "";
  sprintf(Buffer, "F%01d/frmwred0.1", values[0]);
  radio.stopListening();
  //printf(Buffer);
  // Send to hub
  if ( radio.write( Buffer, strlen(Buffer)) ) {

    printf("Send successful\n\r");

  }
  else {
    printf("fail --------------------------------------------------------\n\r");

  }
}

