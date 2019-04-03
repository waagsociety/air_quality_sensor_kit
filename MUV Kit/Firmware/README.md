The firmwares are three and they are written in Arduino IDE, version 1.8.5.
1- code to set the measurement period of the sds sensor kit
2- code for Feather M0 board
3- code for Feather HUZZAH ESP8266 board

### Code for setting the SDS011
The data collection is based on the working period of the SDS011.
It is necessary to configure the SDS011 to collect and send data every 3 minutes.
Connect an Arduino UNO to the sensor, the connections are the following: <br>

* Arduino pin 2 -> SDS pin TX
* Arduino pin 3 -> SDS pin RX
* Arduino pin 5V -> SDS pin 5V
* Arduino pin GND -> SDS pin GND

Upload the code 'MUV_sds011_set_working_period.ino' to Arduino UNO with the sensor connected and the Arduino will take care to configure the SDS011.
The sketch includes the following library:
* SdsDustSensor.h: https://github.com/lewapek/sds-dust-sensors-arduino-library


### Code for Feather M0 board
The code for the Feather M0 is called: "M0_main.ino".
This [tutorial](https://learn.adafruit.com/adafruit-feather-m0-adalogger/setup) explains how to configure Arduino IDE for the Feather M0.

The sketch includes the following libraries:
* ArduinoSound.h: https://github.com/arduino-libraries/ArduinoSound
* math.h: part of the Arduino IDE Core
* Arduino.h: part of Arduino IDE Core
* wiring_private: part of Arduino IDE Core

The code must be uploaded when the Feather M0 is NOT connected to the sensors pcb with the grove cable.

### Code for Feather HUZZAH ESP8266 board
The sketches for Feather HUZZAH ESP8266 are two:
* 'MUV_ESP8266_format.ino': described in First upload section
* 'MUV_ESP8266_main.ino': described in Second upload section

To upload them to your kit, you need the SiLabs CP2104 Driver and the ESP8266 Board Package in Arduino IDE: [tutorial](https://learn.adafruit.com/adafruit-feather-huzzah-esp8266/using-arduino-ide).


#### First upload
This is a recommendation, the First Upload is not mandatory but it formats the System files in the esp8266 and reset previously saved WiFi settings. You just need to upload the code 'MUV_ESP8266_format.ino'.
Once you upload the code, open the Serial Monitor and when it displays: "loop...", you can proceed with the next step.

The 'MUV_ESP8266_format.ino' includes the library:
* FS.h: part of the esp8266 core https://github.com/esp8266/Arduino
* WiFiManager library: https://github.com/tzapu/WiFiManager


#### Second and further uploads
The sketch 'MUV_ESP8266_main.ino' is responsible to read the sensors data and send them to the cloud; it includes the following libraries:

* esp8266 core: https://github.com/esp8266/Arduino
* FS library: part of the esp8266 core
* DNSServer library: part of the esp8266 core
* ESP8266WebServer library: part of the esp8266 core
* WiFiManager library: https://github.com/tzapu/WiFiManager
* ArduinoJson library: https://github.com/bblanchon/ArduinoJson
* PubSubClient library: https://github.com/knolleary/pubsubclient
* Adafruit_NeoPixel library: https://github.com/adafruit/Adafruit_NeoPixel
* SD library: https://github.com/arduino-libraries/SD
* SoftwareSerial library: part of Arduino core
* Wire library: part of Arduino core
* RTClib library: https://github.com/adafruit/RTClib
* Adafruit_Sensor library: https://github.com/adafruit/Adafruit_Sensor
* Adafruit_BME280 library: https://github.com/adafruit/Adafruit_BME280_Library
* Adafruit_ADS1X15 library: https://github.com/adafruit/Adafruit_ADS1X15

Change the maximum payload size in the library file "PubSubClient.h": <br>
'#define MQTT_MAX_PACKET_SIZE 256'<br>

In the sketch you must select the sensors that are deployed in the kit, changing the variables sensors variables:

//  SENSORS  //<br>
const boolean NOISE = false;<br>
const boolean NO2  = true;<br>
const boolean O3 = true;<br>
const boolean BME = true;<br>

If the sensor is not used, the variable must be set as false. <br>
If the sensor is uses, the variable must be set as true.<br>


When you upload the code to this board the following requirements are necessary:
- insert the battery in the battery holder of the board 'Adalogger FeatherWing RTC + SD'
- insert the SD-card in the SD-card holder of the board 'Adalogger FeatherWing RTC + SD'. The card must be formatted as FAT16 or FAT32, [link](https://learn.adafruit.com/adafruit-adalogger-featherwing/using-the-sd-card).
- plug, on top of the esp board, the 'Adalogger FeatherWing RTC + SD' and the 'interface board'
- don't connect any other boards to it.

  Be sure that the information to access the MUV server are correct:
  char mqtt_server[41] = "<my_server>\0";<br>
  char mqtt_portStr[21] = "<my_port>\0";<br>
  char mqtt_username[21] = "<my_username>\0";<br>
  char mqtt_password[128] = "<my_password>\0";<br>
  char mqtt_topic[21] = "<my_topic>\0";<br>

  To get the MUV credentials and MUV password, please [contact](https://github.com/waagsociety/air_quality_sensor_kit/tree/master/MUV%20Kit#contacts).

Now you can upload the code.
Once the code is uploaded, unplug the USB cable and assemble the kit; the kit is ready to run. On the [this page](https://github.com/waagsociety/air_quality_sensor_kit/tree/master/MUV%20Kit) you find the instructions to connect the kit to the local WiFi.

#### Uplinks
The Sensor kit can send two messages, at the start-up and when the data are ready.

- Start-up message:<br> {"message":"startup","id":1340471,"ts":"2019:4:2/15:35:50","location":"Castello","rssi":-69}<br>
message: startup <br>
id: chipID, unique identifier of the esp <br>
ts: timestamp YEAR:MONTH:DAY/HOUR/MINUTE/SECOND <br>
location: location of the kit, to be inserted during configuration mode <br>
rssi: rssi <br>

- Data message:<br>
{"id":1340471,"ts":"2019:4:2/15:53:57","t":23.27,"h":35.4,"p":1001.02,"p25":9.7,"p10":13.8,"no2op1":1057,"no2op2":1100,"o3op1":1567,"o3op2":1457,"dB":49}<br>
id: chipID, unique identifier of the esp <br>
ts: timestamp YY:MM:DD/Hour:Min:Sec <br>
t: temperature (Celsius)<br>
h: relative humidity (Percent)<br>
p: pressure<br>
p25: concentration of pm2.5<br>
p10: concentration of pm10 <br>
no2op1: op1 output of the NO2 sensor <br>
no2op2: op2 output of the NO2 sensor <br>
o3op1: op1 output of the O3 sensor <br>
o3op2: op2 output of the O3 sensor <br>
dB: noise (SPL dB)

The Data uplink happens once every 3 minutes.

## Future development

For the moment the data transmission does not use TLS, this is a possible future development
