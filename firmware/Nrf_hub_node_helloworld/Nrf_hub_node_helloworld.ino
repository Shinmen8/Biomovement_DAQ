/*
 This program reads commands fromthe master and sends sensor readings acoording to a timer set by the user.
 The user needs to choose the node number.

 the packet set has this




 Max payload size is 32 bytes

 This example is based on examples found on Rf24 library:
 Forked RF24 at github :-
 https://github.com/stanleyseow/RF24

timerone library
https://code.google.com/p/arduino-timerone/downloads/list


 Written by Papamichalis Pavlos
 pp_papamichalis@hotmail.com
 */

#include "config.h"
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
#include "TimerOne.h"






RF24 radio(9, 10); // Set up nRF24L01 radio on SPI pin for CE, CSN





const uint64_t talking_pipes[5] =   {
  0xF0D2LL, 0xF0C3LL, 0xF0B4LL, 0xF0A5LL, 0xF096LL
};
const uint64_t listening_pipes[6] = {
  0x3AD2LL, 0x3AC3LL, 0x3AB4LL, 0x3AA5LL, 0x3A96LL
};

unsigned long send_time = 0, previousMillis = 0, rtt = 0;


// const uint64_t pipes[2] = { 0xF0F0F0F0E2LL, 0xF0F0F0F0E2LL };
// const uint64_t pipes[2] = { 0xF0F0F0F0E3LL, 0xF0F0F0F0E3LL };
// const uint64_t pipes[2] = { 0xF0F0F0F0E4LL, 0xF0F0F0F0E4LL };
// const uint64_t pipes[2] = { 0xF0F0F0F0E5LL, 0xF0F0F0F0E5LL };
// Pipe0 is F0F0F0F0D2 ( same as reading pipe )

char receivePayload[32];
uint8_t counter = 0;
boolean timer_analog_flag = 0;

void callback()
{

  timer_analog_flag = 1;

}







void setup(void)
{
  Serial.begin(Baudrate);

  printf_begin();


  radio.begin();

  radio.setDataRate(RF24_2MBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(70);
  radio.enableDynamicPayloads();
  radio.setRetries(0, 15);
  radio.setAutoAck(1);
  radio.enableAckPayload();
  radio.setCRCLength(RF24_CRC_8 );
  radio.setAddressWidth(3);
  radio.enableAckPayload();

  radio.openWritingPipe(talking_pipes[node_address - 1]);
  radio.openReadingPipe(1, listening_pipes[node_address - 1]);

  radio.startListening();
  radio.printDetails(); // Dump the configuration of the rf unit for debugging
  delay(1000);
}

void loop(void)
{

  command_received();

  if (timer_analog_flag == 1) {
    timer_analog_flag = 0;




    //delay(10); /// !!!!!!!!!!!!!!! this delay must be after the command receive if you send more data in this delay time then the data wont be sent


    //uint8_t Data1, Data2, Data3, Data4 = 0;
    //char temp[5];


    // Get the last two Bytes as node-id


    // Use the last 2 pipes address as nodeID
    // sprintf(nodeID,"%X",pipes[0]);

    char outBuffer[32] = ""; // Clear the outBuffer before every loop


    // Get readings from sensors, change codes below to read sensors

    //Data2 = analogRead(0);
    //Data3 = analogRead(1);
    //Data4 = random(0,1000);
    counter++;
    if ( counter > 255 ) {
      counter = 1;
    }

    
    unsigned long currentMillis = millis();

    send_time = (unsigned long)(currentMillis - previousMillis);
    previousMillis = currentMillis;

    //Serial.println(send_time);
    //  sprintf(temp,"%04d",send_time);
    //  strcat(outBuffer,temp);

    //strcat(outBuffer,",");
    outBuffer[0] = (char)node_address;
    outBuffer[1] = (char)counter;
    outBuffer[2] = (char)send_time;
    //outBuffer[3]=(char)send_time;
    //outBuffer[4]=(char)send_time;


    //sprintf(temp, "%03d", send_time);
    //strcat(outBuffer, temp);


    //strcat(outBuffer,",");

    //sprintf(temp,"%04d",Data3);
    //strcat(outBuffer,temp);

    //strcat(outBuffer,"");

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

      //Serial.println(outBuffer);
    }
    else {
      printf("Send failed---------------------------------------------------\n\r");

    }




    radio.startListening();
  }

}


void command_received() {


  //delay(1);
  if ( radio.available() ) {

    //    for (char i=0; i<strlen(receivePayload); i++){
    //    receivePayload[i]=0;
    //    Serial.println(receivePayload[i]);
    //    }
    //char receivePayload[]="";
    uint8_t len = radio.getDynamicPayloadSize();
    radio.read( receivePayload, len);
    printf("inBuffer:%s\n\r", receivePayload);
    //get_values(receivePayload);
    switch (receivePayload[0]) {
      case 'C':
        Serial.println("Connect");
        break;
      case 'F':
        Serial.println("Firmware");
        break;
      case 'B':
        command_start();
        Serial.println("Start");
        break;
      case 'S':
        command_stop();
        Serial.println("Stop");
        break;
    }


    delay(250);
    

  }
}

void command_start() {
  int timer_config;
  if (receivePayload[1]>=5) {
  timer_config=receivePayload[1]*1000;
  Serial.print(timer_config);
  }
  else {
    timer_config=default_timer_config;
  }
  Timer1.initialize(timer_config);
  Timer1.attachInterrupt(callback);
  timer_analog_flag = 0;
}
void command_stop() {
  Timer1.detachInterrupt();
}

void command_respond_firmware() {
  // char Buffer[20]="F1/firmware 0.1";
  char Buffer[10] = "";
  //sprintf(Buffer, "F%01d/frmwred0.1", values[0]);
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

