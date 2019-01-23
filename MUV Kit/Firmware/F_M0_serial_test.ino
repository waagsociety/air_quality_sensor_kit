#include <Arduino.h>   // required before wiring_private.h
#include "wiring_private.h" // pinPeripheral() function
 
Uart Serial2 (&sercom1, 11, 10, SERCOM_RX_PAD_0, UART_TX_PAD_2);
void SERCOM1_Handler()
{
  Serial2.IrqHandler();
}
 
void setup() {
  Serial.begin(115200);
 
  Serial2.begin(9600);
  
  // Assign pins 10 & 11 SERCOM functionality
  pinPeripheral(10, PIO_SERCOM);
  pinPeripheral(11, PIO_SERCOM);
}
 
//uint8_t A=A;
void loop() {
  //Serial.print(i);
  Serial2.write("A\n");
//  if (Serial2.available()) {
//    Serial.print(" -> 0x"); Serial.print(Serial2.read(), HEX);
//  }
//  Serial2.println();
  
  delay(1000);
}
