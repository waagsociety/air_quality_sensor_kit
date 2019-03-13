The firmware are two and they are developed in Arduino IDE, version 1.8.5.

### Code for Feather M0 board
The code for the Feather M0 is called: "M0_main.ino".
This [tutorial](https://learn.adafruit.com/adafruit-feather-m0-adalogger/setup) explains how to configure Arduino IDE for the Feather M0.

The sketch includes the following libraries:
* ArduinoSound
* math
* Arduino
* wiring_private


### Code for Feather HUZZAH ESP8266 board
It doesn't include the reading of the Ozone sensor.
The code for Feather HUZZAH ESP8266 is called: "MUV_ESP8266_main.ino", too upload it to your kit, you need the SiLabs CP2104 Driver and the ESP8266 Board Package in Arduino IDE: [tutorial](https://learn.adafruit.com/adafruit-feather-huzzah-esp8266/using-arduino-ide).

The sketch includes the following libraries:
* esp8266 core: https://github.com/esp8266/Arduino
* WiFiManager library: https://github.com/tzapu/WiFiManager
* ArduinoJson library: https://github.com/bblanchon/ArduinoJson
* PubSubClient library: https://github.com/knolleary/pubsubclient
* Adafruit_NeoPixel library: https://github.com/adafruit/Adafruit_NeoPixel

Change the maximum payload size in the library file "PubSubClient.h": <br>
'#define MQTT_MAX_PACKET_SIZE 256'<br>

When you upload the code to this board the following requirements are necessary:
- plug in top of the esp board the 'Adalogger FeatherWing RTC + SD' and the 'interface board'
- don't connect any other boards to it.

#### First upload
This is a recommendation, the First Upload is not mandatory but it formats the System files in the esp8266 and reset previously saved WiFi settings.
Uncomment the block called '//Format FS, reset Wifi settings, for testing':
<br>
//Format FS, reset Wifi settings, for testing<br>
  Serial.print("Formatting FS...");<br>
  SPIFFS.format();<br>
  Serial.println("Done.");<br>
  Serial.print("Reset WiFi settings...");<br>
  wifiManager.resetSettings();  //****<br>
  Serial.println("Done.");<br>
  while(1) {<br>
    delay(1000);<br>
    Serial.println("loop..."); //i block the code<br>
  } <br>


#### Second and further uploads
In the Arduino sketch comment the block '//Format FS, reset Wifi settings, for testing':<br>
//  //Format FS, reset Wifi settings, for testing<br>
//  Serial.print("Formatting FS...");<br>
//  SPIFFS.format();<br>
//  Serial.println("Done.");<br>
//  Serial.print("Reset WiFi settings...");<br>
//   wifiManager.resetSettings();  //**** <br>
//  Serial.println("Done."); <br>
//  while(1) { <br>
//    delay(1000); <br>
//    Serial.println("loop..."); //i block the code <br>
//  } <br>

  Be sure that the information to access the MUV server are correct:
  char mqtt_server[41] = "<my_server>\0";<br>
  char mqtt_portStr[21] = "<my_port>\0";<br>
  char mqtt_username[21] = "<my_username>\0";<br>
  char mqtt_password[128] = "<my_password>\0";<br>
  char mqtt_topic[21] = "<my_topic>\0";<br>

  To get the MUV credentials and MUV password, please [contact](https://github.com/waagsociety/air_quality_sensor_kit/tree/master/MUV%20Kit#contacts).

#### Uplinks
The Sensor kit can send two messages, at the start-up and when the data are ready.

- Start-up message:<br> {"message":"startup","id":11705757,"ts":"2019:1:23/15:47:58","location":"Waag","rssi":-47}<br>
message: startup <br>
id: chipID, unique identifier of the esp <br>
ts: timestamp YEAR:MONTH:DAY/HOUR/MINUTE/SECOND <br>
location: location of the kit, to be inserted during configuration mode <br>
rssi: rssi <br>

- Data message:<br> {"id":11705757,"ts":"2019:1:2315:49:21","t":22.17,"h":25.4,"p":1002.21,"p25":17.9,"p10":20.7,"no2op1":1291,"no2op2":1196,"dB":61}<br>
id: chipID, unique identifier of the esp <br>
ts: timestamp YY:MM:DD/Hour:Min:Sec <br>
t: temperature (Celsius)<br>
h: relative humidity (Percent)<br>
p: pressure<br>
p25: concentration of pm2.5<br>
p10: concentration of pm10 <br>
no2op1: op1 output of the NO2 sensor <br>
no2op2: op2 output of the NO2 sensor <br>
dB: noise (SPL dB)


The Data uplink happens once every 3 minutes.

## Future development

For the moment the data transmission does not use TLS, this is a possible future development
