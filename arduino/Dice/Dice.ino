/*
  Electronic Dice Software
  
  Random number will display on Seven Segment Display depending on
  mode and accelerometer input (roll).
  
  Supported modes are for multiple dice and number of sides ranging
  from 3 to 20.
  
  Created by Brian Wilson February 2013
  
  SevenSeg library to illuminate displays
  Sparfun Advanced Accelerometer Sketch for accelerometer input

*/
#include "display.h"
#include "accel.h"
#include <math.h>


int accelCount[3]; // Stores the 12-bit signed value
float accelG[3];   // Stores the real accel value in g's
float accelmag;    // magnitude of acceleration
byte source;       // used for register value
int state;

const byte SCALE = 2;  // Sets full-scale range to +/-2, 4, or 8g. Used to calc real g values.
// Set the output data rate below. Value should be between 0 and 7
const byte dataRate = 0;  // 0=800Hz, 1=400, 2=200, 3=100, 4=50, 5=12.5, 6=6.25, 7=1.56
  
// Pin definitions
int int1Pin = 2;  // These can be changed, 2 and 3 are the Arduinos ext int pins
int int2Pin = 3;


int roll;          // random number

//SevenSeg seven; //Initialize library
int output[4] = { 10,10,10,10 }; //10 makes display blank

void setup() {
  Serial.begin(115200);
  //initialize accelerometer
  accelsetup(int1Pin, int2Pin, SCALE, dataRate);
  //initialize shift registers for Seven Segment Display
  initSevenSeg();
  //run boot up sequence
  for (int i1 = 0; i1 < 5; i1++) {  
          loaderanimation();       
    } 
  randomSeed(analogRead(0));

}

void loop() {
  state = 0;
  //Read accelerometer data for toss influenced random seed
  if (digitalRead(int1Pin)==1) {
      readAccelData(accelCount);
  
    //store data
    for (int i=0; i<3; i++) {
      accelG[i] = (float) accelCount[i]/((1<<12)/(2*SCALE));
    }
  } 
  
  //Detect a tap
  
  if (digitalRead(int2Pin)==1)  //if int2 goes high, either p/l has changed or there's been a tap
  {
    source = readRegister(0x22);  //Read interrupt
    if ((source & 0x80)==0x80)  //0x80 is the Event Active bit in the pulse source byte
    {
      //With Tap detected, start roll routine
      accelmag = sqrt(square(accelG[0]) + square(accelG[1]) + square(accelG[2])); 
      randomSeed(accelmag);
      roll = random(1,7);  //need to create function to allow for different roll configurations
      output[0] = roll;  //store roll
      state = 1;
    }
  
   //DETECT ACCELEROMETER ORIENTATION AND MAKE OUTPUT VERTICAL
  }
  //Run loader animation to show dice rolling
  if (state == 1) 
  {
      for (int i1 = 0; i1 < 3; i1++) {  
          loaderanimation();       
      } 
      delay(100);
      sevenSegWrite(output);  //display roll number
      delay(1000);  // 1 second delay
  
  } 
      
}


void loaderanimation() {
  int load[4] = { 0,0,0,0 };
  for (int i = 0; i<6; i++){
    load[0] = i;
    loading(load);
    delay(50);
  }
  blank();
}

