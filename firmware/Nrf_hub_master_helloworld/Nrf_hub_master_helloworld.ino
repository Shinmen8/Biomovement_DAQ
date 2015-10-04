/*
 This program receives data from up to 5 nodes printing them on the serial peripheral.
 Additionaly it receives serial packets from the serial peripheral and sends data to the equivalent node.
 the ascii packet has the following shape:

 +-------+-------++-------+-------+---------------+             +-------+-------+---------------++---------------+
 |command||  node||   seperrator  |   number      |             |   seperrator  |   number      ||   end of      |
 | type  ||   ID ||               |               |  up to      |               |               ||   packet      |
 +---------------++---------------+---------------+  5 numbers  +---------------+---------------++---------------+
 |1ascii ||1ascii||    1ascii     | up to 6 digits|  . . .      |    1ascii     | up to 6 digits|| one ascc      |
 +-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+             +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+
 |a to Z ||  1-5 ||     "/"       |number ascii   |             |     "/"       |number ascii   ||  "/n"         |
 +-------+-------++-------+-------+---------------+             +-------+-------+---------------++---------------+

example
 "F1/100/101/102/103/104/n"
 this string will send to node 1 the command assigned to F with 4 different number fields.

 2 bytes addr data pipes to the beginning of the payloads. The sender will send and
 receive the payload on the same sender/receiver address.
 attention! the packet must not exceed 32bytes

 This example is based on examples found on Rf24 library:
 Forked RF24 at github :-
 https://github.com/stanleyseow/RF24


 Written by Papamichalis Pavlos
 pp_papamichalis@hotmail.com
 */






#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"



RF24 radio(9,10);


const char MaxChars = 40; // an int string contains up to 5 digits and is terminated by a 0 to indicate end of string
char strValue[MaxChars+1]; // must be big enough for digits and terminating null
int index = 0;


char i;


// Radio pipe addresses for the 2 nodes to communicate.
//const uint64_t pipes[6] = { 0xF0F0F0F0D2LL, 0xF0F0F0F0E1LL, 0xF0F0F0F0E2LL, 0xF0F0F0F0E3LL, 0xF0F0F0F0E4LL, 0xF0F0F0F0E5LL };
// bytes serv1 = 0x7365727631 in hex
const uint64_t talking_pipes[6] =   { 
  0, 0xF0F0F0F0D2LL, 0xF0F0F0F0C3LL, 0xF0F0F0F0B4LL, 0xF0F0F0F0A5LL, 0xF0F0F0F096LL };
const uint64_t listening_pipes[6] = { 
  0, 0x3A3A3A3AD2LL, 0x3A3A3A3AC3LL, 0x3A3A3A3AB4LL, 0x3A3A3A3AA5LL, 0x3A3A3A3A96LL };



byte unit;
//boolean analog_mode_flag,analog_mode_unit1_flag,analog_mode_unit2_flag,analog_mode_unit3_flag,analog_mode_unit4_flag,analog_mode_unit5_flag,analog_mode_unitall_flag;

//char command_packet_size = 0;
//unsigned char command_packet[command_packet_maxm_size];


boolean get_string(){

  // the index into the array storing the received digits
  if( Serial.available()) {
    char ch = Serial.read();
    Serial.println(ch);
    if(index < MaxChars && ch >= '!' && ch <= 'z'){
      strValue[index++] = ch; // add the ASCII character to the string;
      return(0);
    }
    else{
      // here when buffer full or on the first non digit
      strValue[index] = 0; // terminate the string with a 0
      index=0;
      return(1);
    }
  }
}



void setup(void)
{


  Serial.begin(38400);
  printf_begin();
  //while (!Serial) ;
  radio.begin();

  radio.setDataRate(RF24_2MBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(70);
  radio.enableDynamicPayloads();
  radio.setAutoAck(1);
  radio.setRetries(0,15);
  radio.enableAckPayload();
  radio.setCRCLength(RF24_CRC_16);

  //radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,talking_pipes[1]);
  radio.openReadingPipe(2,talking_pipes[2]);
  radio.openReadingPipe(3,talking_pipes[3]);
  radio.openReadingPipe(4,talking_pipes[4]);
  radio.openReadingPipe(5,talking_pipes[5]);

  radio.startListening();
  radio.printDetails();

  delay(1000);

}


void loop(void)
{


  if (get_string()==1){
    Serial_data();

//  char outBuffer[32]="";
//    strcat(outBuffer,"012");
//    // Sending back reply to sender using the same pipe
//    radio.stopListening();
//    radio.openWritingPipe(listening_pipes[1]);
//      // Open the correct pipe for writing
//    //radio.openWritingPipe(pipes[1]);
//    //radio.write(receivePayload,len);
//
//    if ( radio.write( outBuffer,  strlen(outBuffer)) ) {
//       printf("Send successful\n\r"); 
//
//    }
//    else {
//       printf("Send failed------------------------------------------\n\r");
//
//    }


   
  } 
  nrfhub_data();
}

 


void Serial_data(){

  char unit=strValue[1]-'0';
  Serial.println(unit,DEC);
  Serial.println(strValue);
  radio.stopListening();
  radio.openWritingPipe(listening_pipes[unit]);
  //unsigned int previousMillis=millis();
  char outBuffer[32]="";
  strcat(outBuffer, strValue);
  
  //int latch;
  if ( radio.write( outBuffer, strlen(outBuffer)) ) {

    sprintf(outBuffer,"T%01d/%s",unit,strValue);
    //sprintf(Buffer,"T");
    Serial.println(outBuffer);
  }
  else {
    //latch=previousMillis-millis();
    //sprintf(Buffer,"E%01d/%4d/%s",unit,latch,strValue);
    sprintf(outBuffer,"E%01d/%s",unit,strValue);
    Serial.println(outBuffer);
  }
  
  
}

void nrfhub_data(){
  char receivePayload[31];
  uint8_t len = 1;
  uint8_t pipe = 1;

    radio.startListening();
    if ( radio.available( &pipe)  ) {
  
      len = radio.getDynamicPayloadSize();
      radio.read( &receivePayload,len);
      receivePayload[len] = 0;
      //printf("%s",receivePayload);
      printf("Got payload: %s len:%i pipe:%i\n\r",receivePayload,len,pipe);
      //Serial.println();

      pipe++;
      if ( pipe > 6 ) pipe = 1;
      i++; 
  
    }
}
