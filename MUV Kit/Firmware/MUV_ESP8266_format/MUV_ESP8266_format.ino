/*
 * Author: Emma Pareschi
 * Date: March 2019
 * Project MUV
 * Code for Feather HUZZAH ESP8266
 * 
*/

#include <FS.h>                   // Make sure ESP library 2.1.0 or higher is installed
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

WiFiManager wifiManager;

void setup() {
 
 Serial.begin(9600);
  //Format FS, reset Wifi settings
  Serial.print("Formatting FS...");
  SPIFFS.format();
  Serial.println("Done.");
  Serial.print("Reset WiFi settings...");
   wifiManager.resetSettings();  //****
  Serial.println("Done.");
  while(1) {
    delay(1000);
    Serial.println("loop..."); //i block the code
  }
}

void loop() {

}


