#ifndef MUV_ESP8266_MAIN_OTA
#define MUV_ESP8266_MAIN_OTA

/*
 * Author: Emma Pareschi
 * Date: March 2019
 * Project MUV
 * Code for Feather HUZZAH ESP8266
 * Version v0.1
 * 

 * Rewrite and additional features: 
 * - More robust Wifi connection and configuration
 * - OTA firmware update
 * - NTP synchronisation of the RTC
 * - Better error handling and logging to the SD card
 * - Removal of sound sensor support
 * Author: Dave Gönner
 * Date: Oktober 2019
 * Version v2.0
*/

#define FIRMWARE_VERSION "2.0-7"  // Increment this version number on every release! Crucial for the Over The Air software update to work properly. 

#define FS_NO_GLOBALS             //allow spiffs to coexist with SD card, define BEFORE including FS.h

#include <Arduino.h>
#include <stdarg.h>               // for the variable length arguments of the log function
#include <ESP8266WiFi.h>          // https://github.com/esp8266/Arduino
#include <ESP8266WebServer.h>     // part of the esp8266 core
#include <ESP8266httpUpdate.h>    // part of the esp8266 core
#include <DNSServer.h>            // part of the esp8266 core
#include <WiFiManager.h>          // https://github.com/tzapu/WiFiManager --- Version 0.14.0
#include <PubSubClient.h>         // https://github.com/knolleary/pubsubclient
#include <Adafruit_NeoPixel.h>    // https://github.com/adafruit/Adafruit_NeoPixel
#include <FS.h>                   // part of the esp8266 core, make sure ESP library 2.1.0 or higher is installed
#include <SD.h>                   // https://github.com/arduino-libraries/SD
#include <ArduinoJson.h>          // https://github.com/bblanchon/ArduinoJson (Version 6!)
#include <Wire.h>                 // part of Arduino core
#include <RTClib.h>               // https://github.com/adafruit/RTClib
#include <ezTime.h>               // https://github.com/ropg/ezTime
#include <Adafruit_Sensor.h>      // https://github.com/adafruit/Adafruit_Sensor
#include <Adafruit_BME280.h>      // https://github.com/adafruit/Adafruit_BME280_Library
#include <Adafruit_ADS1015.h>     // https://github.com/adafruit/Adafruit_ADS1X15

WiFiManager wifiManager;
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// *** NEOPIXEL *** //
#define Pixel_pin 0               //Neopixel pin
#define LedConfig 0
byte brightness = 15;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, Pixel_pin, NEO_RGB + NEO_KHZ800);
uint32_t red = pixels.Color(200, 0, 0);
uint32_t green = pixels.Color(0, 255, 0);
uint32_t orange = pixels.Color(200,80,0);
uint32_t off = pixels.Color(0,0,0);
uint32_t aqua = pixels.Color(0,255,255);
uint32_t blue = pixels.Color(0,0,255);
uint32_t magenta = pixels.Color(255, 0, 255);

#define SwConfig_pin A0           // D0, button to enter wifi manager
bool shouldSaveConfig = false;    // flag for saving data

// *** SD CARD *** // 
int chipSelect = 15;             //chip select pin for the MicroSD Card Adapter
File file;                       // file object that is used to read and write data
char file_name[16];              // make it long enough to hold your longest file name, plus a null terminator


// This will be read from config on the SD-card
char mqtt_server[41];
char mqtt_portStr[21];
int mqtt_port;
char mqtt_username[21];
char mqtt_password[128];
char mqtt_topic[21];
char street_address[41];
char firmware_server[41];
char firmware_portStr[6];
int firmware_port;
char firmware_path[41];

// *** SENSORS *** //
// this will be read from config on the SD-card
boolean NO2  = false;
boolean O3 = false;
boolean BME = false;

// *** RTC *** //
RTC_PCF8523 rtc;
String timestamp;

// *** SDS011 Particulate sensor *** //
unsigned int Pm25 = 0;
unsigned int Pm10 = 0;
int  Status_sds = -1;
int LastError_sds = 0;
float p10 = -1.0;
float p25 = -1.0;
int ecnt = 0;

// *** BME280 *** //
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme; // I2C
float temp = 0;
float hum = 0;
float pres = 0;

// *** NO2 and O3 gas *** //
Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
Adafruit_ADS1115 ads_o3(0x4A);
int op1 = 0;
int op2 = 0;
int op1_sum =0;
int op2_sum =0;
int op1_avg =0;
int op2_avg =0;
int gas_count =0;
int o3_op1 = 0;
int o3_op2 = 0;
int o3_op1_sum =0;
int o3_op2_sum =0;
int o3_op1_avg =0;
int o3_op2_avg =0;

long chipid;

// logging functionality
boolean log_to_sd;
boolean log_to_serial;

//enum loglevel{ LOG_INFO, LOG_WARN, LOG_ERR }; 

// *** general *** //
boolean collect_data_sensors = false;
uint8_t status_publish = 0;
boolean status_write = 0;


//==== FUNCTIONS ===============================================================
void log(const char *format, ...);  // just the prototype, declaration will be further down below


void setPixel(uint32_t color) {
  pixels.setPixelColor(LedConfig, color);
  pixels.show();  
}


// Get the current time from the RTC and form a timestamp from it.
String get_rtc_data_time() {
  DateTime now = rtc.now();
  return now.timestamp();
}


void setup_sd_card() {
  // ************************** INITIALIZE CARD ** //
  pinMode(chipSelect, OUTPUT); // chip select pin must be set to OUTPUT mode
  if (!SD.begin(chipSelect)) { // Initialize SD card
    log("%s", "[E] Could not initialize SD card"); // if return value is false, something went wrong.
    return;    
  } else {
    log("%s", "[i] SD card initialized");   
  }

  int n = 0;
  snprintf(file_name, sizeof(file_name), "data%03d.txt", n); // includes a three-digit sequence number in the file name
  
  while(SD.exists(file_name)) {
    n++;
    snprintf(file_name, sizeof(file_name), "data%03d.txt", n);
  }

  log("%s %s", "[i] File name:", file_name); 

  // ** Write Log File Header 
  file = SD.open(file_name, FILE_WRITE); // open "file.txt" to write data
  if (file) {
    file.println(" "); //Just a leading blank line, in case there was previous data
    const char *header = "ID,TimeStamp,Temp,Hum,Press,P2.5,PM10,NO2op1,NO2op2,O3op1,O3op2,Noise(SPLdB)\0";
    file.println(header);
    file.close();
    log("%s %s", "Header:", header);
  } else {
    log("%s", "[E] Couldn't open log file");
  }
}


// save the currently collected sensor readings to a file on the sd-card.
void log_sd_card(char * buffer) {
  // ** Write Log File Header 
  file = SD.open("log.txt", FILE_WRITE); // open file to write data
  if (file) {
    file.println(buffer);
    file.close();
  } else {
    Serial.println("Couldn't open log file");
  }
}


void log(const char *format, ...) {
  char buffer[256];
  va_list args;
  va_start ( args, format );           
  vsprintf (buffer,format, args);

  if (log_to_serial) {
    Serial.println(buffer);
  }
  if (log_to_sd) {
    char sd_buffer[256];
    char data_time[20];
    get_rtc_data_time().toCharArray(data_time, 20);
    snprintf(sd_buffer, 256, "%s %s", data_time, buffer);
    log_sd_card(sd_buffer);
  }
}


void setupMQTT() {
  mqttClient.setServer(mqtt_server, mqtt_port);  
  char mqtt_clientid[15];
  snprintf (mqtt_clientid, 14, "ESP%lu", chipid);
  if (!mqttClient.connect(mqtt_clientid, mqtt_username, mqtt_password)) {         
    log("%s", "[i] Connecting to MQTT");        

    while (!mqttClient.connect(mqtt_clientid, mqtt_username, mqtt_password)) {    
      setPixel(magenta);
      delay(500);
      Serial.print(".");
      setPixel(off);
      delay(500);     
    } 
  } else { 
    log("%s", "[i] MQTT connected"); 
    setPixel(magenta);
  }  
}


//function that send the MQTT startup message
void send_mqtt_startup_message() {
  log("%s", "[i] Trying to send MQTT startup message");

  String timeString = get_rtc_data_time();
  char mqtt_clientid[15];
  snprintf (mqtt_clientid, 14, "ESP%lu", chipid);

  if (mqttClient.connect(mqtt_clientid, mqtt_username, mqtt_password)) {
    long rssi = WiFi.RSSI(); 

    // send proper JSON startup message
    DynamicJsonDocument json(1024);
    json["message"] = "startup";
    json["id"] = chipid;
    json["ts"] = timeString;
    json["location"] = street_address;
    json["rssi"] = rssi;     
    char buf[110];
    serializeJson(json, buf, sizeof(buf));

    log("%s %s", "[i] Publish message: ", buf);
    char topic_buf[50];
    sprintf(topic_buf, mqtt_topic, chipid);
    mqttClient.publish(topic_buf, buf);
  } else {
    log("%s", "[E] Failed to send, MQTT not connected");
  }
}


void setupRTC() {
  log("%s", "[i] Initialising RTC");
  
  if (! rtc.begin()) {
    log("%s", "[E] Couldn't find RTC");
    while (1);
  } else {
    log("%s", "[i] RTC ready ");
  }

  if (!rtc.initialized()) {
    log("%s", "[E] RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
     rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
     delay(5);
  }  

  char data_time[20];
  get_rtc_data_time().toCharArray(data_time, 20);
  log("%s", data_time);
}


// ------------ NTP ----------------------
void setupNTP() {
  log("%s", "[i] Waiting for NTP sync");
  waitForSync();
  char date_time[40];
  UTC.dateTime().toCharArray(date_time, 40);
  log("UTC: %s", date_time );
  rtc.adjust(DateTime(now()));
}


//  This function is called when the Wifi configuration manager is started
void configModeCallback (WiFiManager *myWiFiManager) {
  log("%s", "[i] Entered WiFi config mode");

  char ip_buffer[16];
  WiFi.softAPIP().toString().toCharArray(ip_buffer, 16);
  log("%s", ip_buffer);
  
  char ssid_buffer[50];
  myWiFiManager->getConfigPortalSSID().toCharArray(ssid_buffer, 50);
  log("%s", ssid_buffer);
  setPixel(blue);
}


//callback notifying us of the need to save config
void saveConfigCallback () {
  log("%s", "[i] Should save WiFi config");
  shouldSaveConfig = true;
}

// Save the config variables to the SD-card
void saveConfigJsonSD() {
  log("%s", "[i] Saving config to SD-card");
  if (SD.exists("config.json")) {
    log("%s", "[i] Delete existing config.json");
    SD.remove("config.json");
  }
  File config_file = SD.open("config.json", FILE_WRITE); // creating new config file for writing
  if (config_file) {
    log("%s", "[i] config.json opened");

    DynamicJsonDocument json(1024);
    json["mqtt_server"] = mqtt_server;
    json["mqtt_port"] = mqtt_portStr;
    json["mqtt_username"] = mqtt_username;
    json["mqtt_password"] = mqtt_password;
    json["mqtt_topic"] = mqtt_topic;
    json["address"] = street_address;
    json["firmware_server"] = firmware_server;
    json["firmware_port"] = firmware_portStr;
    json["firmware_path"] = firmware_path;
    if (NO2) { json["sensor_no2"] = "True"; } else { json["sensor_no2"] = "False"; }  
    if (O3) { json["sensor_o3"] = "True"; } else { json["sensor_o3"] = "False"; }  
    if (BME) { json["sensor_bme"] = "True"; } else { json["sensor_bme"] = "False"; }  

    serializeJson(json, Serial);
    Serial.println();
    serializeJson(json, config_file);
    config_file.close();
    log("%s", "[i] config.json written succesfully");
  } else {
    log("%s", "[E] Failed to open config.json file for writing");
  }
}


// Read temperature an humidity values from the BME280 sensor and 
// store in the global temp, hum and pres variables.
void data_bme() {
  temp = 0.00;
  hum = 0.00;
  pres = 0.00;

  if(BME){ 
    temp = bme.readTemperature();
    hum = bme.readHumidity();
    pres = bme.readPressure() / 100.0F;

    temp = round(temp*100);
    temp = temp/100;

    hum = round(hum*100);
    hum = hum/100;

    pres = round(pres*100);
    pres= pres/100;
  }
}


// Read PM25 and PM10 values from the SDS sensor 
// and store in the global p25 and p10 variables.
void data_sds(){
  uint8_t mData = 0;
  uint8_t i = 0;
  uint8_t mPkt[10] = {0};
  uint8_t mCheck = 0;

  Status_sds = -1;  // Default: no data availabe ...
  p10 = -1.0;
  p25 = -1.0;

  while (Serial.available() > 0) {
    // from www.inovafitness.com
    // packet format: AA C0 PM25_Low PM25_High PM10_Low PM10_High 0 0 CRC AB

    Status_sds = 0; // There is data!

    mData = Serial.read();
    delay(10);//wait until packet is received

    if (mData == 0xAA) {  // Head1 ok?
      mPkt[0] =  mData;
      mData = Serial.read();

      if (mData == 0xc0) {  // Head2 ok?
        mPkt[1] =  mData;
        mCheck = 0;
        for (i = 0; i < 6; i++) {  // Get the content of the package!
          mPkt[i + 2] = Serial.read();
          delay(2);
          mCheck += mPkt[i + 2];
        }

        mPkt[8] = Serial.read();
        delay(5);

        mPkt[9] = Serial.read();
        if (mPkt[9] != 0xAB) {
          log("%s %X", "[W] mData[9] != 0xAB,  but ", mPkt[9]);
          Status_sds = 4; LastError_sds = Status_sds; ecnt++;
        }

        if (mCheck == mPkt[8]) {  // Check CRC
          Serial.flush();
          //Serial.write(mPkt,10);

          Pm25 = (uint16_t)mPkt[2] | (uint16_t)(mPkt[3] << 8);
          Pm10 = (uint16_t)mPkt[4] | (uint16_t)(mPkt[5] << 8);

          if (Pm25 > 9999)
            Pm25 = 9999;
          if (Pm10 > 9999)
            Pm10 = 9999;

          p25 = Pm25 / 10.0;
          p10 = Pm10 / 10.0;

          while (Serial.available() > 0) {  // Zap buffer ...
            mData = Serial.read();
          }

          return;  // We now have one good packet :-))

        } else {  // CRC ??
          log("%s %X %s %X", "[i] CRC != OK, ", mCheck, "  !=  ", mPkt[8]);
          Status_sds = 3; LastError_sds = Status_sds; ecnt++;
        }
      } else {  // Head2 ??
        log("%s %X", "[W] mData != 0xc0,  but ", mData);
        Status_sds = 2; LastError_sds = Status_sds; ecnt++;
      }
    } else {  // Head1 ??
      log("%s %X", "[W] mData != 0xAA,  but ", mData);
      Status_sds = 1; LastError_sds = Status_sds; ecnt++;
    }
  }
}


// read the values from the NO2 and O3 gas sensors 
// and store in the global op1, op2, o3_op1 and o3_op2 variables.
void data_gas1(){
  if(NO2) {
    op1 = ads.readADC_Differential_0_1();
    op2 = ads.readADC_Differential_2_3();
  } else {
    op1 = 0;
    op2 = 0;
  }
    
  if(O3) {
    o3_op1 = ads_o3.readADC_Differential_0_1();
    o3_op2 = ads_o3.readADC_Differential_2_3();
  } else {
    o3_op1 = 0;
    o3_op2 = 0;
  }
}


void setup_BME() {
  // BME280 
  bool status;   
  log("%s", "[i] Starting BME280 sensor");  
  status = bme.begin();  
  if (!status) {
    log("%s", "[E] Could not find a valid BME280 sensor, check wiring!");
  } else {
    log("%s", "[i] BME280 sensor is OK");
  }
}


void setup_gas() {
  // ADS1115 converters for NO2 and O3 sensors
  log("%s", "[i] Starting ADS converters");  
  ads.begin();
  ads_o3.begin();
}


void setup_Wifi(){
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setSaveConfigCallback(saveConfigCallback);
    
  boolean startConfigPortal = false;
  
  if ( analogRead(SwConfig_pin) <= 500 ) {
    startConfigPortal = true;
  }

  if (startConfigPortal) {
    WiFiManagerParameter custom_address("address", "address", "insert address", 41);
    wifiManager.addParameter(&custom_address);
    wifiManager.startConfigPortal("MUV_air");
    
    if (shouldSaveConfig) {
      //save the config
      log("%s", "[i] Saving new wifi config");

      mqtt_port = atoi(mqtt_portStr);
      strcpy(street_address, custom_address.getValue());
      saveConfigJsonSD();

      shouldSaveConfig = false;
      setPixel(off);
    }
  }

  WiFi.mode(WIFI_STA);
  if (WiFi.SSID()) {
    log("%s", "[i] Using saved WiFi credentials");
    WiFi.begin();
    char ssid_buffer[50];
    WiFi.SSID().toCharArray(ssid_buffer,50);
    log("[i] WiFi SSID: %s", ssid_buffer);
    char pass_buffer[50];
    WiFi.psk().toCharArray(pass_buffer, 50);
    log("[i] WiFi Password: %s", pass_buffer);

    uint8_t wifi_status = WiFi.status();
    while (wifi_status != WL_CONNECTED) {
      setPixel(red);
      delay(500);
      Serial.print(".");
      setPixel(off);
      delay(500);
      if (wifi_status == WL_CONNECT_FAILED) {
        log("%s", "[E] Wifi password incorrect");
      }
      if (wifi_status == WL_NO_SSID_AVAIL) {
        log("%s", "[E] Wifi SSID can not be reached");
      }
      wifi_status = WiFi.status();
    }
  } else {
    log("%s", "[W] No saved credentials");
    startConfigPortal = true;
  }

  WiFi.waitForConnectResult();
  if (WiFi.status() != WL_CONNECTED) {
    log("%s", "[E] Failed to connect Wifi");
    startConfigPortal = true;
  } else { 
    pixels.setPixelColor(LedConfig, green);
    pixels.show();
  }

  WiFi.printDiag(Serial);
  log("%s", "[i] Wifi connected");
  // End wifi section, from here on we are connected 
}


void check_firmware_update() {
  log("%s", "[i] Checking for newer firmware");
  ESPhttpUpdate.rebootOnUpdate(false);
  t_httpUpdate_return ret = ESPhttpUpdate.update(espClient, firmware_server, firmware_port, firmware_path, FIRMWARE_VERSION);

  switch(ret) {
    case HTTP_UPDATE_OK:
        log("%s", "[i] Firmware update OK"); // may not be called since we reboot the ESP
        delay(500);
        ESP.reset();
        break;
    case HTTP_UPDATE_NO_UPDATES:
        log("%s", "[i] Firmware already at latest");
        break;
    case HTTP_UPDATE_FAILED:
        log("%s", "[E] Firmware update failed");
        break;
  }
}

void read_config_from_SD() {
  //read config.json from SD card
  log("%s", "[i] Opening config.json");

  if (SD.exists("config.json")) {
    File config_file = SD.open("config.json", FILE_READ); // open the config file for reading
    if (config_file) {
      log("%s", "[i] Reading config.json file");
      DynamicJsonDocument json(1024);
      DeserializationError error = deserializeJson(json, config_file);
      serializeJson(json, Serial);  //print the content of json object on serial.
      Serial.println();

      if (!error) {
        log("%s", "[i] Parsed json, from config.json");
        strcpy(mqtt_server, json["mqtt_server"]);
        strcpy(mqtt_portStr, json["mqtt_port"]);
        mqtt_port = atoi(mqtt_portStr);
        strcpy(mqtt_username, json["mqtt_username"]);
        strcpy(mqtt_password, json["mqtt_password"]);
        strcpy(mqtt_topic, json["mqtt_topic"]);
        strcpy(street_address, json["address"]);
        strcpy(firmware_server, json["firmware_server"]);
        strcpy(firmware_portStr, json["firmware_port"]);
        firmware_port = atoi(firmware_portStr);
        strcpy(firmware_path, json["firmware_path"]);

        String no2Str = json["sensor_no2"];
        no2Str.toLowerCase();
        if (no2Str.equals("true")) { NO2 = true; } else { NO2 = false; }

        String o3Str = json["sensor_o3"];
        o3Str.toLowerCase();
        if (o3Str.equals("true")) { O3 = true; } else { O3 = false; }

        String bmeStr = json["sensor_bme"];
        bmeStr.toLowerCase();
        if (bmeStr.equals("true")) { BME = true; } else { BME = false; }

      } else {
        log("%s", "[E] Failed to parse config.json");
      }

      config_file.close();
    } else {
      log("%s", "[E] Can't open config.json");
    }
  } else {
    log("%s", "[E] No config.json on SD card");
  }
}

void setup() {
  Serial.begin(9600); // has to be 9600 because of RX from SDS sensor
  delay(500);
  Serial.println("\n");
  log_to_sd = false;
  log_to_serial = true;
  log("%s %s", "[i] Starting, running firmware version", FIRMWARE_VERSION);

  setupRTC();
  setup_sd_card();
  log_to_sd = true;
  read_config_from_SD();

  log("%s %s %s %s", "[i] Installed sensors:", (NO2 ? "NO2": ""), (O3 ? "O3": ""), (BME ? "BME": ""));

  pinMode(SwConfig_pin, INPUT); 

  pixels.begin();
  delay(1);
  pixels.setBrightness(brightness);
  setPixel(off);

  chipid = ESP.getChipId();

  setup_Wifi();
  check_firmware_update();

  setupNTP();
  setupMQTT();
  send_mqtt_startup_message();

  setup_BME();
  setup_gas();

  // end of setup
  setPixel(off);
}  


// Pack the currently collected sensor readings in a JSON string
// and send it to the MQTT server.
void send_mqtt_payload() {
  status_publish = 0;

  char mqtt_clientid[15];
  snprintf (mqtt_clientid, 14, "ESP%lu", chipid);

  if (mqttClient.connect(mqtt_clientid, mqtt_username, mqtt_password)) {
    log("%S", "[i] MQTT connected.");

    // send proper JSON startup message
    DynamicJsonDocument json(1024);
    json["id"] = chipid;
    json["ts"] = timestamp;
    if (BME) { 
      json["t"] = temp;
      json["h"] = hum;
      json["p"] = pres;
    }  
    json["p25"] = p25;
    json["p10"] = p10;
    if (NO2) {
      json["no2op1"] = op1_avg;
      json["no2op2"] = op2_avg;
    }
    if (O3) {
      json["o3op1"] = o3_op1_avg;
      json["o3op2"] = o3_op2_avg;
    }  

    char buf[200];
    serializeJson(json, buf, sizeof(buf));

    log("%s %s", "[i] Publish message:", buf);

    char topic_buf[50];
    sprintf(topic_buf, mqtt_topic, chipid);
    status_publish = mqttClient.publish(topic_buf, buf);

  } else {
    log("%s", "[E] MQTT not connected, could not send");
  }
}


// save the currently collected sensor readings to a file on the sd-card.
void save_data_sdcard() {
  status_write = false;
  
  // ** Write Log File Header 
  file = SD.open(file_name, FILE_WRITE); // open "file.txt" to write data
  if (file) {
    Serial.println("Log file opened");
    file.print(chipid); file.print(",");
    file.print(timestamp); file.print(",");
    if (BME) { 
      file.print(temp); file.print(",");
      file.print(hum); file.print(",");
      file.print(pres); file.print(",");
    }  
    file.print(p25); file.print(",");
    file.print(p10); file.print(",");
    if (NO2) {
      file.print(op1_avg); file.print(",");
      file.print(op2_avg); file.print(",");
    }  
    if (O3) {
      file.print(o3_op1_avg); file.print(",");
      file.print(o3_op2_avg); file.println();
    }  

    file.close();
    delay(10);
    status_write = true;
  } else {
    log("%s", "[E] Couldn't open log file");
    status_write = false;
  }
}

void loop() {
  mqttClient.loop();

  data_sds();  //check sds data

  //are there good data?
  if (p25 > -1.0 && p10 > -1.0) {
    collect_data_sensors = true;    //if there are data => flag collect data
    gas_count = ecnt;
    ecnt =0;
  } else {
    ecnt++;
    data_gas1();          //continuosly read the gases
    op1_sum += op1;       //sum the readings of NO2
    op2_sum += op2;
    o3_op1_sum += o3_op1;       //sum the readings of O3
    o3_op2_sum += o3_op2;
  }

  if (collect_data_sensors){

    status_publish = 0;     //before sending, clean the status publish flag

    Serial.println("");
    timestamp = get_rtc_data_time();         //get the timestamp
    Serial.print("Timestamp : "); Serial.println(timestamp);
    
    Serial.print("PM2.5 : "); Serial.println(p25);
    Serial.print("PM10 : "); Serial.println(p10);
       
    if (BME) {    
      data_bme();                             //get temp/hum/pres
      Serial.print("Temperaure (C) : "); Serial.println(temp);
      Serial.print("Humidity (%) : "); Serial.println(hum);
      Serial.print("Pressure (hPa) : "); Serial.println(pres);
    }  

    if (NO2) {
      op1_avg = op1_sum/gas_count;            //average the op1
      op2_avg = op2_sum/gas_count;            //average the op2
      Serial.print("NO2 op1 avg : "); Serial.println(op1_avg);
      Serial.print("NO2 op2 avg : "); Serial.println(op2_avg);
    }  

    if (O3) {
      o3_op1_avg = o3_op1_sum/gas_count;            //average the op1
      o3_op2_avg = o3_op2_sum/gas_count;            //average the op2
      Serial.print("O3 op1 avg : "); Serial.println(o3_op1_avg);
      Serial.print("O3 op2 avg : "); Serial.println(o3_op2_avg);
    }  

    Serial.println();

    send_mqtt_payload();  //send payload

    //if the publish was success => blink green
    if(status_publish == 1){
      log("%s", "[i] The payload reached the server");
      setPixel(green);
      delay(100);
      setPixel(off);
    } else {
      setPixel(orange);
      delay(1);
      save_data_sdcard(); //if the publish not good => save in sd card
    }

    //if it was possible to open the file => blink orange
    if(status_write == true && status_publish == 0){
      log("%s", "[W] The payload saved on sd-card");
      setPixel(off);
      delay(100);
      setPixel(orange);
    } else if (status_write == false && status_publish == 0) { //if there is not card => Led orange and stays red until the next try
      setPixel(red);
    }
    
    op1_avg =0;
    op2_avg =0;
    op1_sum=0;
    op2_sum=0;
    o3_op1_avg =0;
    o3_op2_avg =0;
    o3_op1_sum=0;
    o3_op2_sum=0;
    
    gas_count = ecnt;
    collect_data_sensors = false;         //un-flag collect data
  }

  Serial.print(get_rtc_data_time()+" "); Serial.print("ECOUNT: "); Serial.println(ecnt); 

  #ifdef DO_DEBUG
    Serial.print("\tECOUNT= "); Serial.println(ecnt); 
    Serial.print("\tNO2 op1= "); Serial.print(op1);  
    Serial.print("\tNO2 op2= "); Serial.print(op2);
    Serial.print("\tO3 op1= "); Serial.print(o3_op1);  
    Serial.print("\tO3 op2= "); Serial.println(o3_op2);
  #endif
    
  delay(1000);
}

#endif