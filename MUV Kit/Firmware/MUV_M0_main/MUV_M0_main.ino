/*
 * Author: Emma Pareschi
 * Date: Janury 23 2019
 * Project MUV
 * Code for Feather M0 proto board
*/

int delta_time = 0;
int start_time = 0;
int count =0;
int  meanval_long =0;
char readStr[1024];
char str[] = "-NOISE";
int request_ok = 0;
int send_flag = 0;
int index_string = 0;

#include <ArduinoSound.h>
#include <math.h>
#include <Arduino.h>   // required before wiring_private.h
#include "wiring_private.h" // pinPeripheral() function
 
Uart Serial2 (&sercom1, 11, 10, SERCOM_RX_PAD_0, UART_TX_PAD_2);
void SERCOM1_Handler()
{
  Serial2.IrqHandler();
}

// create an amplitude analyzer to be used with the I2S input
AmplitudeAnalyzer amplitudeAnalyzer;


void setup() {
  Serial2.begin(9600);

  pinPeripheral(10, PIO_SERCOM);
  pinPeripheral(11, PIO_SERCOM);

  // setup the I2S audio input for 44.1 kHz with 32-bits per sample
  if (!AudioInI2S.begin(44100, 32)) {
    Serial.println("Failed to initialize I2S input!");
    while (1); // do nothing
  }

  // configure the I2S input as the input for the amplitude analyzer
  if (!amplitudeAnalyzer.input(AudioInI2S)) {
    Serial.println("Failed to set amplitude analyzer input!");
    while (1); // do nothing
  }

}

#define SAMPLES 256 //multipli di 128

void loop() {

int samples[SAMPLES];

  // check if a new analysis is available
  if (amplitudeAnalyzer.available()) {
     for (int i=0; i<SAMPLES; i++) {

        int amplitude = amplitudeAnalyzer.read();
        samples[i] = amplitude;
      
     }

  }

  float meanval = 0;
  for (int i=0; i<SAMPLES; i++) {
     meanval += samples[i];
    }

     count++;
     meanval /= SAMPLES;
     meanval_long += meanval;

     if (request_ok){
        //Serial.println("got request");
        request_ok = 0;

        
        meanval_long /= count;
        delay(10);
        
        int amp_dBSLP = (120 + (20*log10((meanval_long)/(pow(2,23)-1))));
        delay(10);
        Serial2.print(amp_dBSLP);
        Serial2.print("\n");
      

        count = 0;
        delay(100);

        for(int i=0; i < sizeof(readStr); i++){       
          readStr[i]=0 ;
        }  


      } 

  
  if (Serial2.available() >0) {
    
     char c = Serial2.read();  //gets one byte from Serial buffer

      if (c == 10) { //if there is a new line
             
        for(int i=1; i<4; i++){  
               
          if(readStr[i]==str[i]){
            request_ok = 1;
          } else {
            request_ok = 0;
          }
        
        }

        
      // stampa su serial monitor i dati raccolti da serial monitor
         for(int i=0; i < sizeof(readStr); i++){       
             readStr[i]=0 ;
        }  
       
       index_string = 0;
      
      } else {
        
        index_string++;
        
        readStr[index_string] = c; //makes the string readString

      }
   
}
  
}
