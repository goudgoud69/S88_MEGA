#include "MEGA_S88.h"

#include <Arduino.h>

// pins allocation

const byte dataIn = 0;        // input from another Arduino
const byte dataOut = 1;       // output to another arduino or S88 data
const byte clockS88 = 2;     // S88 clock on pin 2, int0
const byte PSS88 = 3;        // S88 PS on pin 3, int1
// const byte DccPin = 20;   // reseved for Dcc use if neccesary
// const byte DccLed = 21;

// timing variable

int clockCounter = 0;     // 8/16 tops clock
long loopCounter = 0;     // for reset problem with ECOS

// number of 16 sensors blocs

int nbBlocs = 0;

// data buffer by bloc

unsigned int data1= 0xFFFF;
unsigned int data2= 0xFFFF;
unsigned int data3= 0xFFFF;
unsigned int data4= 0xFFFF;

// sensor by bloc

unsigned int sensors1=0;
unsigned int sensors2=0;
unsigned int sensors3=0;
unsigned int sensors4=0;

// internal variable

int i,j;

// PS signal from S88

void PS() {
  clockCounter = 0;                 // init clock counter
  data1 = sensors1; sensors1 = 0;
  switch (nbBlocs){
  case 2 : {data2 = sensors2; sensors2 = 0;break;}
  case 3 : {data2 = sensors2; sensors2 = 0;data3 = sensors3; sensors3 = 0;break;}
  case 4 : {data2 = sensors2; sensors2 = 0;data3 = sensors3; sensors3 = 0;data4 = sensors4; sensors4 = 0;break;}
  }
  loopCounter++;                    // for reset management
}

// clock signal from S88

void clock() {
 digitalWrite(dataOut, bitRead(data1,clockCounter));          // exit bits to next S88 in chain
 switch (nbBlocs) {
 case 1 : {bitWrite(data1, clockCounter, digitalRead(dataIn));break;}
 case 2 : {bitWrite(data1, clockCounter, bitRead(data2,clockCounter));
           bitWrite(data2, clockCounter, digitalRead(dataIn));break;}
 case 3 : {bitWrite(data1, clockCounter, bitRead(data2,clockCounter));
           bitWrite(data2, clockCounter, bitRead(data3,clockCounter));
           bitWrite(data3, clockCounter, digitalRead(dataIn));break;}
 case 4 : {bitWrite(data1, clockCounter, bitRead(data2,clockCounter));
           bitWrite(data2, clockCounter, bitRead(data3,clockCounter));
           bitWrite(data3, clockCounter, bitRead(data4,clockCounter));
           bitWrite(data4, clockCounter, digitalRead(dataIn));break;}
     }
 clockCounter = (clockCounter + 1) % 16;                      // bits 0 to 15
}

// setup

void S88_Setup(int NbBlocs) {
    nbBlocs = NbBlocs;
        for (i=4;i<20;i++) {pinMode(i,INPUT_PULLUP);}
        switch (nbBlocs) {
            case 2 :{for (i=22;i<38;i++) {pinMode(i,INPUT_PULLUP);};break;}
            case 3 :{for (i=22;i<54;i++) {pinMode(i,INPUT_PULLUP);};break;}
            case 4 :{for (i=22;i<70;i++) {pinMode(i,INPUT_PULLUP);};break;}
            }
    pinMode(clockS88,INPUT_PULLUP);          // S88 clock in input
    attachInterrupt(0,clock,RISING);      // interrupt int0
    pinMode(PSS88,INPUT_PULLUP);             // S88 PS in input
    attachInterrupt(1,PS,RISING);         // interrupt int1
    pinMode(dataOut, OUTPUT);             // init data out
    digitalWrite(dataOut,LOW);            // led off
    pinMode(dataIn,INPUT_PULLUP);         // data in
}

// loop

void S88_Loop() {

  // reset management for ECOS
if (loopCounter == 20) {
        bitSet(sensors1,0);
        switch (nbBlocs) {
            case 2 : {bitSet(sensors2,0);break;}
            case 3 : {bitSet(sensors3,0);break;}
            case 4 : {bitSet(sensors4,0);break;}
     }
    }

 // update all sensors by group
  for (i=4;i<20;i++){if (!digitalRead(i)){bitSet(sensors1,i-4);}}

  switch (nbBlocs){
  case 2 : {for (i=22;i<38;i++){{if (!digitalRead(i)){bitSet(sensors2,i-22);}}};break;}
  case 3 : {for (i=22;i<38;i++){{if (!digitalRead(i)){bitSet(sensors2,i-22);}}};
            for (i=38;i<54;i++){{if (!digitalRead(i)){bitSet(sensors3,i-38);}}};break;}
  case 4 : {for (i=22;i<38;i++){{if (!digitalRead(i)){bitSet(sensors2,i-22);}}};
            for (i=38;i<54;i++){{if (!digitalRead(i)){bitSet(sensors3,i-38);}}};
            for (i=54;i<70;i++){{if (!digitalRead(i)){bitSet(sensors4,i-54);}}};break;}
      }

 }




