


#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"



RF24 radio(9,10);


const char MaxChars = 40; // an int string contains up to 5 digits and is terminated by a 0 to indicate end of string
char strValue[MaxChars+1]; // must be big enough for digits and terminating null
int index = 0;

const char NUMBER_OF_FIELDS = 5; // how many comma separated fields we expect
char fieldIndex = 0; // the current field being received
int values[NUMBER_OF_FIELDS];
char i;


// Radio pipe addresses for the 2 nodes to communicate.
//const uint64_t pipes[6] = { 0xF0F0F0F0D2LL, 0xF0F0F0F0E1LL, 0xF0F0F0F0E2LL, 0xF0F0F0F0E3LL, 0xF0F0F0F0E4LL, 0xF0F0F0F0E5LL };
// bytes serv1 = 0x7365727631 in hex
const uint64_t talking_pipes[6] =   { 
  0, 0xF0F0F0F0D2LL, 0xF0F0F0F0C3LL, 0xF0F0F0F0B4LL, 0xF0F0F0F0A5LL, 0xF0F0F0F096LL };
const uint64_t listening_pipes[6] = { 
  0, 0x3A3A3A3AD2LL, 0x3A3A3A3AC3LL, 0x3A3A3A3AB4LL, 0x3A3A3A3AA5LL, 0x3A3A3A3A96LL };



byte command_type,unit;
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

//boolean get_values(){
//  for ( i=0; i<strlen(strValue); i++){
//    Serial.print(strValue[i]);
//  }
//  Serial.println();
//  if (get_string()==1){///read a string of alphanumeric
//    for ( i=1; i<strlen(strValue)+1; i++){
//
//      char ch = strValue[i];
//      if(ch>= '0' && ch <= '9') // is this an ascii digit between 0 and 9?
//      {
//        // yes, accumulate the value
//        values[fieldIndex] = (values[fieldIndex] * 10) + (ch - '0');
//      }
//      else if (ch == '/') // comma is our separator, so move on to the next field
//      {
//        if(fieldIndex < NUMBER_OF_FIELDS-1)
//          fieldIndex++; // increment field index
//      }
//      else
//      {
//        //        Serial.print("1= ");
//        //        Serial.println(values[0]);
//        //        Serial.print("2= ");
//        //        Serial.println(values[1]);
//        //        Serial.print("3= ");
//        //        Serial.println(values[2]);
//        //        Serial.print("4= ");
//        //        Serial.println(values[3]);
//        //        Serial.print("5= ");
//        //        Serial.println(values[4]);
//
//      }
//    }
//    return(1);
//  }
//}




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
  } 
  nrfhub_data();
}

  //  char receivePayload[31];
  //
  //  //command_manipulation();
  //
  //
  //  uint8_t len = 0;
  //  uint8_t pipe = 1;
  //  // Loop thru the pipes 0 to 5 and check for payloads
  //  if ( radio.available( &pipe)  ) {
  //
  //    len = radio.getDynamicPayloadSize();
  //    radio.read( &receivePayload,len);
  //    receivePayload[len] = 0;
  //    printf("Got payload: %s len:%i pipe:%i    %i\n\r",receivePayload,len,pipe,i);
  //    // Sending back reply to sender using the same pipe
  //
  //
  //
  //    // Increase pipe and reset to 0 if more than 5
  //    pipe++;
  //    if ( pipe > 5 ) pipe = 0;
  //    i++; 
  //
  //  }

  //    radio.stopListening();
  //    // radio.openWritingPipe(listening_pipes[pipe-1]);
  //    // Open the correct pipe for writing
  //    radio.openWritingPipe(listening_pipes[0]);
  //    //radio.write(receivePayload,len);
  //
  //    if ( radio.write( receivePayload, len) ) {
  //      printf("Send successful\n\r"); 
  //
  //    }
  //    else {
  //      printf("Send failed------------------------------------------\n\r");
  //
  //    }
  //    // Format string for printing ending with 0
  //    receivePayload[len] = 0;
  //    printf("Got payload: %s len:%i pipe:%i\n\r",receivePayload,len,pipe);
  //
  //    //radio.openReadingPipe(1,pipes[1]);
  //    radio.startListening();
  //    i=0;

  //delay(1);


void Serial_data(){

  char unit=strValue[1]-'0';
//  Serial.println(unit,DEC);
//  Serial.println(strValue);
  radio.stopListening();
  radio.openWritingPipe(listening_pipes[unit]);
  //unsigned int previousMillis=millis();
  char outBuffer[20]="";
  strcat(outBuffer, strValue);
  //int latch;
  if ( radio.write( strValue, strlen(strValue)) ) {

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
  radio.startListening();
  
}

void nrfhub_data(){
  char receivePayload[31];
  uint8_t len = 1;
  uint8_t pipe = 1;

    
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


