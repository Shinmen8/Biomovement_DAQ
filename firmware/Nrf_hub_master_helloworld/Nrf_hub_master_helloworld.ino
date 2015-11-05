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


 attention! the packet must not exceed 32bytes

 This example is based on examples found on Rf24 library:
 Forked RF24 at github :-
 https://github.com/stanleyseow/RF24


 Written by Papamichalis Pavlos
 pp_papamichalis@hotmail.com
 */

#include "config.h"
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"



RF24 radio(9, 10); // Set up nRF24L01 radio on SPI pin for CE, CSN

const uint64_t talking_pipes[6] =   {
  0, 0xF0D2LL, 0xF0C3LL, 0xF0B4LL, 0xF0A5LL, 0xF096LL
};
const uint64_t listening_pipes[6] = {
  0, 0x3AD2LL, 0x3AC3LL, 0x3AB4LL, 0x3AA5LL, 0x3A96LL
};



uint32_t connected_talking_pipes[6];
uint32_t connected_listening_pipes[6];
uint8_t  connected_nodes = 0;


const char NUMBER_OF_FIELDS = 2; // how many comma separated fields we expect
int  values[NUMBER_OF_FIELDS];

const char MaxChars = 40; // an int string contains up to 5 digits and is terminated by a 0 to indicate end of string
char strValue[MaxChars + 1]; // must be big enough for digits and terminating null
int index = 0;

char i;
byte unit;


boolean get_string() {  // receive data from the serial communication

  // the index into the array storing the received digits
  if ( Serial.available()) {
    char ch = Serial.read();
    //Serial.println(ch);
    if (index < MaxChars && ch >= '!' && ch <= 'z') {
      strValue[index++] = ch; // add the ASCII character to the string;
      return (0);
    }
    else {
      // end here when buffer full or on the first non digit
      strValue[index] = 0; // terminate the string with a 0
      index = 0;
      return (1);
    }
  }
}



void setup(void)
{


  Serial.begin(Baudrate);
  printf_begin();
  //while (!Serial) ;
  radio.begin();

  radio.setDataRate(RF24_2MBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(70);
  radio.enableDynamicPayloads();
  radio.setAutoAck(1);
  radio.setRetries(0, 15);
  radio.enableAckPayload();
  radio.setCRCLength(RF24_CRC_8 );
  radio.setAddressWidth(3);
  radio.enableAckPayload();


  //radio.openWritingPipe(pipes[0]);
  //  radio.openReadingPipe(1, talking_pipes[1]);
  //  radio.openReadingPipe(2, talking_pipes[2]);
  //  radio.openReadingPipe(3, talking_pipes[3]);
  //  radio.openReadingPipe(4, talking_pipes[4]);
  //  radio.openReadingPipe(5, talking_pipes[5]);
  //
  //  radio.startListening();
  #ifdef debug_mode
  radio.printDetails();  // Dump the configuration of the rf unit for debugging
  #endif
  delay(1000);

}



void loop(void)
{
  if (get_string() == 1) { //if serial data received send to nodes
    Serial_data();
  }
  nrfhub_data();         //if nrf data print to serial
}




void Serial_data() {

  if (strValue[0] == 'C') { //check how many devices are on
    check_connected_nodes();
  }
  else if (strValue[0] == 'B') { //start data 
    command_start();
  }
  else {  //send the command straigth to the device 
    char unit = (strValue[1] - '0')+1;
    Serial.println(unit, DEC);
    Serial.println(strValue);
    radio.stopListening();
    radio.openWritingPipe(listening_pipes[unit]);
    //unsigned int previousMillis=millis();
    char outBuffer[32] = "";
    strcat(outBuffer, strValue);

    //int latch;
    if ( radio.write( outBuffer, strlen(outBuffer)) ) {

      sprintf(outBuffer, "T%01d/%s", unit, strValue);
      //sprintf(Buffer,"T");
      Serial.println(outBuffer);
    }
    else {
      //latch=previousMillis-millis();
      //sprintf(Buffer,"E%01d/%4d/%s",unit,latch,strValue);
      sprintf(outBuffer, "E%01d/%s", unit, strValue);
      Serial.println(outBuffer);
    }
  }

}


void nrfhub_data() {
  uint8_t receivePayload[31] = "";
  uint8_t len = 1;
  uint8_t pipe = 1;

  radio.startListening();
  if ( radio.available( &pipe)  ) {

    len = radio.getDynamicPayloadSize();
    radio.read( &receivePayload, len);
    receivePayload[len] = 0;




    //    Serial.print(receivePayload[2],DEC);
    //    Serial.print("/");
    //    Serial.print(receivePayload[3],DEC);
    //    Serial.print("/");
    //    Serial.print(receivePayload[4],DEC);
    //    Serial.print("/");
    //    Serial.print(receivePayload[5],DEC);
    //    Serial.print("/");
    //    Serial.print(receivePayload[6],DEC);
    //    Serial.print("/");
    //    Serial.print(receivePayload[7],DEC);
    //    Serial.print("/");
    //    Serial.print(receivePayload[8],DEC);
    //    Serial.print("/");
    //    Serial.print(receivePayload[9],DEC);
    //    Serial.print("/");
    //    Serial.print(receivePayload[10],DEC);
    //    Serial.print("/");
    //    Serial.print(receivePayload[11],DEC);
    //    Serial.print("/");
    //    Serial.print(receivePayload[12],DEC);
    //    Serial.print("/");
    //    Serial.print(receivePayload[13],DEC);
    //    Serial.print("/");
    //    Serial.print(receivePayload[14],DEC);
    //    Serial.print("/");
    //    Serial.print(receivePayload[15],DEC);
    //    Serial.print("/");
    //    Serial.print(receivePayload[16],DEC);
    //    Serial.print("/");
    //    Serial.print(receivePayload[17],DEC);
    //    Serial.print("/");
    //    Serial.print(receivePayload[18],DEC);
    //    Serial.print("/");
    //    Serial.println(receivePayload[19],DEC);
    //Serial.println(strlen(receivePayload));


    Serial.print("B");
    Serial.print(pipe, DEC);
    Serial.print("/");
    Serial.print((byte)receivePayload[0]);
    Serial.print("/");
    Serial.print((byte)receivePayload[1]);
    Serial.print("/");

    int tempnum = ((byte)receivePayload[2] << 8) + (byte)receivePayload[3];
    Serial.print(tempnum);
    Serial.print("/");
    tempnum = (receivePayload[4] << 8) + receivePayload[5];
    Serial.print(tempnum);
    Serial.print("/");
    tempnum = (receivePayload[6] << 8) + receivePayload[7];
    Serial.print(tempnum);
    Serial.print("/");
    tempnum = (receivePayload[8] << 8) + receivePayload[9];
    Serial.print(tempnum);
    Serial.print("/");
    tempnum = (receivePayload[10] << 8) + receivePayload[11];
    Serial.print(tempnum);
    Serial.print("/");
    tempnum = (receivePayload[12] << 8) + receivePayload[13];
    Serial.print(tempnum);
    Serial.print("/");
    tempnum = (receivePayload[14] << 8) + receivePayload[15];
    Serial.print(tempnum);
    Serial.print("/");
    tempnum = (receivePayload[16] << 8) + receivePayload[17];
    Serial.print(tempnum);
    Serial.print("/");
    tempnum = (receivePayload[18] << 8) + receivePayload[19];
    Serial.println(tempnum);



    //    tempnum=(receivePayload[4]<<8)| receivePayload[5];
    //    Serial.print(tempnum);
    //    Serial.print("/");
    //    tempnum=(receivePayload[6]<<8)| receivePayload[7];
    //    Serial.print(tempnum);
    //    Serial.print("/");
    //    tempnum=(receivePayload[8]<<8)| receivePayload[9];
    //    Serial.print(tempnum);
    //    Serial.print("/");
    //    tempnum=(receivePayload[10]<<8)| receivePayload[11];
    //    Serial.print(tempnum);
    //    Serial.print("/");
    //    tempnum=(receivePayload[12]<<8)| receivePayload[13];
    //    Serial.print(tempnum);
    //    Serial.print("/");
    //    tempnum=(receivePayload[14]<<8)| receivePayload[15];
    //    Serial.print(tempnum);
    //    Serial.print("/");
    //    tempnum=(receivePayload[16]<<8)| receivePayload[17];
    //    Serial.print(tempnum);
    //    Serial.print("/");
    //    tempnum=(receivePayload[18]<<8)| receivePayload[19];
    //    Serial.println(tempnum);

    //    Serial.print("   ");
    //    Serial.println(receivePayload[2], DEC);
    //    // Serial.print("   ");
    // printf("len:%i pipe:%i\n\r",len,pipe);
    //printf("Got payload: %s len:%i pipe:%i\n\r",receivePayload,len,pipe);
    //Serial.println();

    pipe++;
    //Serial.println(pipe);
    if ( pipe > connected_nodes ) pipe = 1;


  }
}





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
      //  Serial.print("1= ");
      //   Serial.println(values[0]);
      //    Serial.print("2= ");
      //     Serial.println(values[1]);
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



void check_connected_nodes() {
  Serial.print("L");
  Serial.println("searching for devices");
  uint8_t i = 0;

  for (int address = 1; address <= 6; address++) {

    //radio.openReadingPipe(address, talking_pipes[address]);

    radio.stopListening();
    radio.openWritingPipe(listening_pipes[address]);
    char outBuffer[32] = "";
    strcat(outBuffer, strValue);

    if ( radio.write( outBuffer, strlen(outBuffer)) ) {
      i++;
      connected_listening_pipes[i] = listening_pipes[address];
      connected_talking_pipes[i] = talking_pipes[address];
      sprintf(outBuffer, "T%01d/%s", address, strValue);
      //sprintf(Buffer,"T");
      //Serial.print("L");
      //Serial.println(outBuffer);
    }
    else {
      //latch=previousMillis-millis();
      //sprintf(Buffer,"E%01d/%4d/%s",unit,latch,strValue);
      //Serial.print("L");
      sprintf(outBuffer, "E%01d/%s", address, strValue);

      //Serial.println(outBuffer);
    }
    delay(100);
  }

  connected_nodes = i;
  //Serial.print("L");
  //Serial.print("nodes found:");
  //Serial.println(connected_nodes);
  for (int k = 1; k <= connected_nodes; k++) {
    radio.openReadingPipe(k, connected_talking_pipes[k]);
    Serial.print("D");
    Serial.println(k);
    Serial.print("L");
    Serial.print(connected_talking_pipes[k], HEX);
    Serial.print("   ");
    Serial.println(connected_listening_pipes[k], HEX);
  }
}

void command_start() {
  get_values(strValue);

  char unit = strValue[1] - '0';
  //Serial.println(unit, DEC);
  //Serial.println(strValue);
  radio.stopListening();
  radio.openWritingPipe(connected_listening_pipes[unit]);
  //unsigned int previousMillis=millis();
  char outBuffer[32] = "";
  outBuffer[0] = 'B';
  //outBuffer[1] = (char)values[1];
  outBuffer[1] = (byte)values[1];
  //Serial.print("Got payload:");

  // Serial.println(outBuffer[1], DEC);

  //int latch;
  if ( radio.write( outBuffer, strlen(outBuffer)) ) {

    sprintf(outBuffer, "T%01d/%s", unit, strValue);
    //sprintf(Buffer,"T");
    Serial.println(outBuffer);
  }
  else {
    //latch=previousMillis-millis();
    //sprintf(Buffer,"E%01d/%4d/%s",unit,latch,strValue);
    sprintf(outBuffer, "E%01d/%s", unit, strValue);
    Serial.println(outBuffer);
  }
}




