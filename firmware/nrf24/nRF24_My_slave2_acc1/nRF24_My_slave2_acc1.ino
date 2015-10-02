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
#include "I2Cdev.h"
#include "MPU6050.h"
#include "HMC5883L.h"
//#include <TimerOne.h>
// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;
HMC5883L mag;
//MPU6050 accelgyro(0x69); // <-- use for AD0 high

int16_t ax, ay, az;
int16_t gx, gy, gz;
int16_t mx, my, mz;
int potvalue=A6;


// uncomment "OUTPUT_READABLE_ACCELGYRO" if you want to see a tab-separated
// list of the accel X/Y/Z and then gyro X/Y/Z values in decimal. Easy to read,
// not so easy to parse, and slow(er) over UART.
#define OUTPUT_READABLE_ACCELGYRO

// uncomment "OUTPUT_BINARY_ACCELGYRO" to send all 6 axes of data as 16-bit
// binary, one right after the other. This is very fast (as fast as possible
// without compression or data loss), and easy to parse, but impossible to read
// for a human.
//#define OUTPUT_BINARY_ACCELGYRO


#define LED_PIN 13
bool blinkState = false;
bool timer_flag=0;



// Set up nRF24L01 radio on SPI pin for CE, CSN
RF24 radio(9,10);

// For best performance, use P1-P5 for writing and Pipe0 for reading as per the hub setting
// Below is the settings from the hub/receiver listening to P0 to P5
//const uint64_t pipes[6] = { 0x7365727631LL, 0xF0F0F0F0E1LL, 0xF0F0F0F0E2LL, 0xF0F0F0F0E3LL, 0xF0F0F0F0E4LL, 0xF0F0F0F0E5LL };
// Example below using pipe5 for writing
//const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0x7365727631LL };

 const uint64_t pipes[2] = { 0xF0F0F0F0E2LL, 0xF0F0F0F0E2LL };
// const uint64_t pipes[2] = { 0xF0F0F0F0E3LL, 0xF0F0F0F0E3LL };
// const uint64_t pipes[2] = { 0xF0F0F0F0E4LL, 0xF0F0F0F0E4LL };
// const uint64_t pipes[2] = { 0xF0F0F0F0E5LL, 0xF0F0F0F0E5LL };
// Pipe0 is F0F0F0F0D2 ( same as reading pipe )

char receivePayload[32];
uint8_t counter=0;
uint16_t Data1,Data2 = 0;
//bool timer_flag=0;




void setup(void)
{

  // Setup LCD
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)


    // initialize device
   // Serial.println("Initializing I2C devices...");
    accelgyro.initialize();
    mag.initialize();
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

  //Timer1.initialize(5000); // set a timer of length 100000 microseconds (or 0.1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
  //Timer1.attachInterrupt( timerIsr );
}

void loop(void)
{
      
char outBuffer[32]=""; 
char outBuffer2[32]=""; 
  char temp[9];
  bool timeout=0;
  uint16_t nodeID = pipes[0] & 0xff; // Use the last 2 pipes address as nodeID
  // Clear the outBuffer before every loop
  //unsigned long send_time, rtt = 0;
    
    // Get readings from sensors, change codes below to read sensors
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    mag.getHeading(&mx, &my, &mz); 

//        Serial.print("S");
//        Serial.print(ax); Serial.print(",");
//        Serial.print(ay); Serial.print(",");
//        Serial.print(az); Serial.print(",");
//        Serial.print(gx); Serial.print(",");
//        Serial.print(gy); Serial.print(",");
//        Serial.println(gz); //Serial.print(",");
//        Serial.print(mx); Serial.print(",");
//        Serial.print(my); Serial.print(",");
//        Serial.println(mz); 
    
    sprintf(outBuffer,"B");  
    sprintf(temp,"%d",ax);  
    strcat(outBuffer,temp);  
    strcat(outBuffer,",");
    sprintf(temp,"%d",ay);  
    strcat(outBuffer,temp); 
    //Serial.println(outBuffer);  
    strcat(outBuffer,",");
    sprintf(temp,"%d",az);  
    strcat(outBuffer,temp);  
    strcat(outBuffer,",");
    sprintf(temp,"%d",gx);  
    strcat(outBuffer,temp);  
    strcat(outBuffer,",");
    sprintf(temp,"%d",gy);  
    strcat(outBuffer,temp);  
    strcat(outBuffer,",");
    sprintf(temp,"%d",gz);  
    strcat(outBuffer,temp);    
    radio.stopListening();   
    radio.write( outBuffer, strlen(outBuffer));
   // printf("outBuffer: %s len: %d\n\r",outBuffer, strlen(outBuffer));
    
    sprintf(outBuffer2,"G");
    sprintf(temp,"%d",mx);  
    strcat(outBuffer2,temp);  
    strcat(outBuffer2,",");
    sprintf(temp,"%d",my);  
    strcat(outBuffer2,temp);  
    strcat(outBuffer2,",");
    sprintf(temp,"%d",mz);  
    strcat(outBuffer2,temp);  
    radio.write( outBuffer2, strlen(outBuffer2));
   // printf("outBuffer: %s len: %d\n\r",outBuffer2, strlen(outBuffer2)); 
    
   
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
    
    //send_time = millis();
    

   // if ( radio.write(outBuffer, strlen(outBuffer)) ) {
   //    printf("Send successful\n\r"); 
//
   // }
   // else {
   //    printf("Send failed\n\r");
//
    //}  
    delay(5); 
timer_flag=0;
    
    radio.startListening();

  
    
  }


//void timerIsr()
//{
//    Data1 = counter++;
//   Data2 = analogRead(3);
//    timer_flag=1;
//}


