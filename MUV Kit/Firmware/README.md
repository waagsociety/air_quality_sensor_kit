## MUV Firmware

There are two different firmwares in this directory and they are written in Arduino IDE, version 1.8.5.
1. code to set the measurement period of the sds sensor kit
2. code for Feather HUZZAH ESP8266 board

Because the compilation of the code takes a long time in the Arduino IDE, the latest code for the ESP8266 was developed in Visual Studio Code (v1.39.2) using Platform.io (v4.0.3). The code should compile in the Arduino IDE, but it is much easier to just flash the prepared binary file to the board using the esptool explained below.

### Code for setting the SDS011
The data collection is based on the working period of the SDS011. It is necessary to configure the 
SDS011 to collect and send data every 3 minutes. Connect an Arduino UNO to the sensor, 
the connections are the following: 

* Arduino pin 2 -> SDS pin TX
* Arduino pin 3 -> SDS pin RX
* Arduino pin 5V -> SDS pin 5V
* Arduino pin GND -> SDS pin GND

Upload the code 'MUV_sds011_set_working_period.ino' to Arduino UNO with the sensor connected and the Arduino will take care to configure the SDS011.
The sketch includes the following library:
* SdsDustSensor.h: https://github.com/lewapek/sds-dust-sensors-arduino-library

### Code for Feather HUZZAH ESP8266 board
The sketches for Feather HUZZAH ESP8266 are the following:
* 'MUV_ESP8266_format.ino': described in First upload section
* 'MUV_ESP8266_main_OTA.ino': described in Second upload section

To upload them to your kit, you need the SiLabs CP2104 Driver and the ESP8266 Board Package in Arduino IDE: [tutorial](https://learn.adafruit.com/adafruit-feather-huzzah-esp8266/using-arduino-ide).


#### First upload
This is a recommendation, the First Upload is not mandatory but it formats the System files in the esp8266 and reset previously saved WiFi settings. You just need to upload the code 'MUV_ESP8266_format.ino'. Once you upload the code, open the Serial Monitor and when it displays: "loop...", you can proceed with the next step.

The 'MUV_ESP8266_format.ino' includes the library:
* FS.h: part of the esp8266 core https://github.com/esp8266/Arduino
* WiFiManager library: https://github.com/tzapu/WiFiManager


#### Second and further uploads
The sketch 'MUV_ESP8266_main_OTA.ino' is responsible to read the sensors data and send them to the cloud; it includes the following libraries:

* esp8266 core: https://github.com/esp8266/Arduino
* FS library: part of the esp8266 core
* DNSServer library: part of the esp8266 core
* ESP8266WebServer library: part of the esp8266 core
* WiFiManager library: https://github.com/tzapu/WiFiManager --- Version 0.14.0
* ArduinoJson library: https://github.com/bblanchon/ArduinoJson (Version 6!)
* PubSubClient library: https://github.com/knolleary/pubsubclient
* SD library: https://github.com/arduino-libraries/SD
* SoftwareSerial library: part of Arduino core
* Wire library: part of Arduino core
* RTClib library: https://github.com/adafruit/RTClib
* ezTime library: https://github.com/ropg/ezTime
* Adafruit_Sensor library: https://github.com/adafruit/Adafruit_Sensor
* Adafruit_BME280 library: https://github.com/adafruit/Adafruit_BME280_Library
* Adafruit_ADS1X15 library: https://github.com/adafruit/Adafruit_ADS1X15
* Adafruit_NeoPixel library: https://github.com/adafruit/Adafruit_NeoPixel

Change the maximum payload size in the library file "PubSubClient.h":
```
#define MQTT_MAX_PACKET_SIZE 256
```

When you upload the code to this board the following requirements are necessary:
- insert the battery in the battery holder of the board 'Adalogger FeatherWing RTC + SD'
- insert the SD-card in the SD-card holder of the board 'Adalogger FeatherWing RTC + SD'. The card must be formatted as FAT16 or FAT32, [link](https://learn.adafruit.com/adafruit-adalogger-featherwing/using-the-sd-card).
- plug, on top of the esp board, the 'Adalogger FeatherWing RTC + SD' and the 'interface board'
- don't connect any other boards to it.

#### config.json file on the SD-card

In previous versions, several configuration values such as the MQTT credentials were hardcoded in the source code. Because of the new Over The Air firmware updates, these configuration values were moved to a separate configuration file, located on the SD-card.

The SD-card must contain file in the root directory named "config.json". This file contains the login credentials for the MQTT server, the credentials for the firmware server and the configuration of which sensors are on board this Monitoring Station. It is essential that this file is configured and formatted correctly, otherwise the firmware will not work.

The format of this file is as follows:
```
{ "mqtt_server":"mqtt host",
  "mqtt_port":"mqtt port",
  "mqtt_username":"mqtt usename",
  "mqtt_password":"mqtt password",
  "mqtt_topic":"sensor/%u/data",
  "address":"sensor street address",
  "firmware_server":"firmware host",
  "firmware_port":"firmware port",
  "firmware_path":"/esp/update/index.php",
  "sensor_no2":"True",
  "sensor_o3":"False",
  "sensor_bme":"True"  }
```

Depending on which sensors are on board your Monitoring Station, set the values for `sensor_no2`, `sensor_o3` and `sensor_bme` to `True` or `False`.<br>
If the sensor is not used, the variable must be set to `False`. <br>
If the sensor is used, the variable must be set to `True`.<br>

Be sure that the information to access the MUV server and the firmware update server are correct.
  
To get the MUV credentials and MUV password, please contact [insert contact information]
  
Now you can flash the firmware.

#### Flashing the firmware

Since the new firmware has the capability to be updated from a remote server, prebuilt binaries of the firmware are available.
You do not have to compile the code yourself. The easiest way to flash the prebuilt firmware binary to the ESP8266 is to use 
the esptool that is bundled with the Arduino ESP8266 Core. 

Open a terminal window and go to the directory where the Arduino esptool is installed. On my Windows this is located in
C:\Users\Windows\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\2.5.2\tools\esptool\

You can also install the esptool separately using pip, if you have python installed:

`$ pip install esptool`

See the esptool [github]https://github.com/espressif/esptool page for more information.

Next copy the latest firmware image (located in the builds directory) to this directory. 
After that you can flash the firmware using this command:

`$ python esptool.py write_flash 0x0000 firmware_2.0-4.bin`

Because the SDS dust sensor shares the serial connection that is also used for the firmware upload, It is possible for a firmware upload to fail because of interference from the sensor. If this happens, just retry the upload and it should succeed, or disconnect the dust sensor.

Once the code is uploaded, unplug the USB cable and assemble the kit; the kit is ready to run. On the [this page](https://github.com/waagsociety/air_quality_sensor_kit/tree/master/MUV%20Kit) you find the instructions to connect the kit to the local WiFi.

#### Uplinks
The Sensor kit can send two messages, at the start-up and when the data are ready.

- Start-up message:
```
{"message":"startup","id":1340471,"ts":"2019-10-25T15:35:50","location":"Groningen","rssi":-69}
```
message: startup <br>
id: chipID, unique identifier of the esp <br>
ts: timestamp in ISO 8601 format: YEAR-MONTH-DAY T HOUR:MINUTE:SECOND <br>
location: location of the kit, to be inserted during configuration mode <br>
rssi: rssi <br>

- Data message:
```
{"id":1340471,"ts":"2019-10-25T15:40:05","t":23.27,"h":35.4,"p":1001.02,"p25":9.7,"p10":13.8,"no2op1":1057,"no2op2":1100,"o3op1":1567,"o3op2":1457}```

id: chipID, unique identifier of the esp <br>
ts: timestamp in ISO 8601 format: YEAR-MONTH-DAY T HOUR:MINUTE:SECOND <br>
t: temperature (Celsius)<br>
h: relative humidity (Percent)<br>
p: pressure<br>
p25: concentration of pm2.5<br>
p10: concentration of pm10 <br>
no2op1: op1 output of the NO2 sensor <br>
no2op2: op2 output of the NO2 sensor <br>
o3op1: op1 output of the O3 sensor <br>
o3op2: op2 output of the O3 sensor <br>

The Data uplink happens once every 3 minutes. This is determined by the working period of the SDS011 sensor. Please configure this correctly as described at the top of this document.

## Future development

For the moment the data transmission does not use TLS, this is a possible future development
