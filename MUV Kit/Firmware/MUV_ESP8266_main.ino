/*
 * Author: Emma Pareschi
 * Date: Janury 23 2019
 * Project MUV
 * Code for Feather HUZZAH ESP8266
 * 
*/

#define FS_NO_GLOBALS
#include <FS.h>                   // Make sure ESP library 2.1.0 or higher is installed
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson
#include <PubSubClient.h>         //https://github.com/knolleary/pubsubclient

#include <SD.h> //Load SD library
#include <SoftwareSerial.h>
#include <Wire.h> //include Wire.h library
#include "RTClib.h" //include Adafruit RTC library

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include <Adafruit_ADS1015.h>

  char mqtt_server[41] = "<my_server>\0";<br>
  char mqtt_portStr[21] = "<my_port>\0";<br>
  char mqtt_username[21] = "<my_username>\0";<br>
  char mqtt_password[128] = "<my_password>\0";<br>
  char mqtt_topic[21] = "<my_topic>\0";<br>
  char address[41] = "insert address\0";

String location = "";

int mqtt_port = atoi(mqtt_portStr);

#define SwConfig_pin A0              // D0, button to enter wifi manager
boolean SwConfig_state = 1;         // current state of the button
boolean lastSwConfig_state = 1;     // previous state of the button

WiFiManager wifiManager;
WiFiClient espClient;
PubSubClient mqttClient(espClient);

long chipid;
bool shouldSaveConfig = false;    //flag for saving data

// for Neopixel
#include <Adafruit_NeoPixel.h>

#define Pixel_pin            0 //Neopixel pin
#define LedConfig 0

byte brightness = 50;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, Pixel_pin, NEO_GRB + NEO_KHZ800);

uint32_t red = pixels.Color(255, 0, 0);
uint32_t green = pixels.Color(0, 255, 0);
uint32_t orange = pixels.Color(255,100,0);
uint32_t off = pixels.Color(0,0,0);
uint32_t aqua = pixels.Color(0,255,255);
uint32_t blue = pixels.Color(0,0,255);
uint32_t magenta = pixels.Color(255, 0, 255);

// *** SD CARD *** // 
 int chipSelect = 15; //chip select pin for the MicroSD Card Adapter
 File file; // file object that is used to read and write data
 char file_name[16]; // make it long enough to hold your longest file name, plus a null terminator

// *** RTC *** //
 RTC_PCF8523 rtc;
 String timestamp;


// *** BME *** //
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme; // I2C

float temp = 0;
float hum = 0;
float pres = 0;


// *** SDS011 *** //
unsigned int Pm25 = 0;
unsigned int Pm10 = 0;
int  Status_sds = -1;
int LastError_sds = 0;
float p10 = -1.0;
float p25 = -1.0;
int ecnt = 0;


// *** NO2 gas *** //
 Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */

 int op1 = 0;
 int op2 = 0;
 int op1_sum =0;
 int op2_sum =0;
 int op1_avg =0;
 int op2_avg =0;
 int gas_count =0;



// *** NOISE *** //
long noise_dB = 0;
int request_ok = 0;
String readStr;
char c;
int start_time_noise = 0;
int noise_flag = 0;


// *** general *** //
boolean collect_data_sensors = false;
uint8_t status_publish = 0;
boolean status_write = 0;

#define DEBUG 1

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void saveConfigJson() {
  //save the custom parameters to FS
  Serial.println("saving config");
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["mqtt_server"] = mqtt_server;
  json["mqtt_port"] = mqtt_portStr;
  json["mqtt_username"] = mqtt_username;
  json["mqtt_password"] = mqtt_password;
  json["mqtt_topic"] = mqtt_topic;
  json["address"] = address;

  fs::File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("failed to open config file for writing");
  }

  json.printTo(Serial);
  Serial.println("\n");
  json.printTo(configFile);
  configFile.close();
  //end save
}


void setup() {
 
 Serial.begin(9600);
 
  //--------------------------------------------
//  //Format FS, reset Wifi settings, for testing
//  Serial.print("Formatting FS...");
//  SPIFFS.format();
//  Serial.println("Done.");
//  Serial.print("Reset WiFi settings...");
//   wifiManager.resetSettings();  //****
//  Serial.println("Done.");
//  while(1) {
//    delay(1000);
//    Serial.println("loop..."); //i block the code
//  }
  //--------------------------------------------

 pinMode(SwConfig_pin, INPUT);

  pixels.begin();
  pixels.setBrightness(brightness);
  pixels.setPixelColor(LedConfig, off);
  pixels.show();

  //read configuration from FS json
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      fs::File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);  //print the content of json object on serial.
        if (json.success()) {
          Serial.println("\nparsed json, from setup");

          strcpy(mqtt_server, json["mqtt_server"]); //save the block "mqtt_server" of json object in mqtt_server variable
          strcpy(mqtt_portStr, json["mqtt_port"]);
          mqtt_port = atoi(mqtt_portStr);
          strcpy(mqtt_username, json["mqtt_username"]);
          strcpy(mqtt_password, json["mqtt_password"]);
          strcpy(mqtt_topic, json["mqtt_topic"]);
          strcpy(address, json["address"]);
          

        } else {
          Serial.println("failed to load json config");
        }
      }
    } else {
      Serial.println("/config.json does not exist, creating");
      saveConfigJson(); // saving the hardcoded default values
    }
  } else {
    Serial.println("failed to mount FS");
  }
  //end read


  wifiManager.setSaveConfigCallback(saveConfigCallback);
    
  boolean startConfigPortal = false;
  
  if ( analogRead(SwConfig_pin) <= 500 ) {
    startConfigPortal = true;
  }

  WiFi.mode(WIFI_STA);
  if (WiFi.SSID()) {
    Serial.println("Using saved credentials");
    ETS_UART_INTR_DISABLE();
    wifi_station_disconnect();
    ETS_UART_INTR_ENABLE();
    WiFi.begin();
  } else {
    Serial.println("No saved credentials");
    startConfigPortal = true;
  }

  WiFi.waitForConnectResult();
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Failed to connect Wifi");
    startConfigPortal = true;
  } else { 
  }

  if (startConfigPortal) {

    pixels.setPixelColor(LedConfig, blue);
    pixels.show();

    WiFiManagerParameter custom_address("address", "address", "insert address", 41);
    wifiManager.addParameter(&custom_address);

    wifiManager.startConfigPortal("MUV_air");
    
    if (shouldSaveConfig) {

      mqtt_port = atoi(mqtt_portStr);
      strcpy(address, custom_address.getValue());
      location = custom_address.getValue();

      saveConfigJson();
      shouldSaveConfig = false;
    }
    
  }


  mqttClient.setServer(mqtt_server, mqtt_port);  
  char mqtt_clientid[15];
  snprintf (mqtt_clientid, 14, "ESP%u", chipid);
  while (!mqttClient.connect(mqtt_clientid, mqtt_username, mqtt_password)) {         
     Serial.println("CAN'T CONNECTED");        
     delay(1000);
     
  } 
   
    Serial.println("CONNECTED"); 
  
  
  //mqttClient.setServer(mqtt_server, mqtt_port);
  pixels.setPixelColor(LedConfig, green);
  pixels.show();
  chipid = ESP.getChipId();

  pixels.setPixelColor(LedConfig, off);
  pixels.show();


// ************************** INITIALIZE CARD ** //
  pinMode(chipSelect, OUTPUT); // chip select pin must be set to OUTPUT mode
  
  if (!SD.begin(chipSelect)) { // Initialize SD card
    Serial.println("Could not initialize SD card."); // if return value is false, something went wrong.
    return;    
  } else {
    Serial.println("");  
    Serial.println("card initialized.");   
    Serial.println(""); 
  }

  int n = 0;
  snprintf(file_name, sizeof(file_name), "data%03d.txt", n); // includes a three-digit sequence number in the file name
  
  while(SD.exists(file_name)) {
    n++;
    snprintf(file_name, sizeof(file_name), "data%03d.txt", n);
  }

  Serial.println("***FILE NAME********************************************************");
  Serial.println(file_name);
  Serial.println("");  


  // ** Write Log File Header 
  file = SD.open(file_name, FILE_WRITE); // open "file.txt" to write data
  if (file)
  {
    file.println(" "); //Just a leading blank line, in case there was previous data
    String header = "Timestamp";
    file.println("ID,TimeStamp,Temp,Hum,Press,P2.5,PM10,NO2op1,NO2op2,Noise(SPLdB)");
    file.close();
    Serial.println("***HEADER***********************************************");
    Serial.println(header);
    Serial.println("********************************************************");
    Serial.println();
  }
  else
  {
    Serial.println("Couldn't open log file");
  }

  // ************************** INITIALIZE RTC ** //
  
   if (! rtc.begin()) {
   Serial.println("Couldn't find RTC");
   while (1);
  } else {
   Serial.println("RTC ready ");
   Serial.println("********************************************************");
   Serial.println();
  }

 // uncomment this line to set the RTC to the date & time this sketch was compiled
 // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  if (! rtc.initialized()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
     rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }  

    startup_message();
    Serial.println("********************************************************");
    Serial.println();


//******** BME **********
    bool status;   
    // default settings
    // (you can also pass in a Wire library object like &Wire2)
    status = bme.begin();  
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
    } else {
      Serial.println("BME280 sensor is OK");
      Serial.println("********************************************************");
      Serial.println();
    }


// ******* GAS - ADS1115 ********
    ads.begin();

  delay(500); //wait before to start
}


//function that send the startup message
void startup_message() {

   // ** get the timestamp
        DateTime now = rtc.now();
        int YY = now.year();
        int MM = now.month();
        int DD = now.day();

        int hh = now.hour();
        int mm = now.minute();
        int ss = now.second();

        String timeString = String(YY) + ":" + String(MM) + ":" + String(DD) + "/" + String(hh) + ":"  + String(mm) + ":" + String(ss);

  char mqtt_clientid[15];
  snprintf (mqtt_clientid, 14, "ESP%u", chipid);

  if (mqttClient.connect(mqtt_clientid, mqtt_username, mqtt_password)) {
      Serial.println("MQTT connected.");
      
      long rssi = WiFi.RSSI();

      // send proper JSON startup message
      DynamicJsonBuffer jsonBuffer;
      JsonObject& json = jsonBuffer.createObject();
      json["message"] = "startup";
      json["id"] = chipid;
      json["ts"] = timeString;
      json["location"] = address;
      json["rssi"] = rssi;     
      char buf[110];
      json.printTo(buf, sizeof(buf));

      Serial.print("Publish message: ");
      Serial.println(buf);

      char topic_buf[50];
      sprintf(topic_buf, mqtt_topic, chipid);
      mqttClient.publish(topic_buf, buf);
  
  }

}


//**********************LOOP
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
    data_gas1();          //continuosly read the NO2
    op1_sum += op1;       //sum the readings of NO2
    op2_sum += op2;
  }

  // ask to feather M0 for noise
  if (collect_data_sensors) {
   Serial.println(""); 
   Serial.println("NOISE");
   noise_flag = 1;
   request_ok = 0;

  }
   
  //get the answer from feather M0 within 500ms
  if (noise_flag == 1){  

    start_time_noise = millis();
      
    while(millis()-start_time_noise <500){    //read on serial for 500ms
      
      if (Serial.available() > 0) {
        
        char c = Serial.read();  //gets one byte from Serial buffer

          if (c == 10) { //if there is a new line
        
            request_ok = 1;

            noise_dB = readStr.toInt();
            readStr = "";
            noise_flag = 0;
      
          } else {

            readStr += c;
            request_ok = 0;

          }
       
      }
  }

  }

  
    if (request_ok == 1){ //if the request was ok => reset the flags
        noise_flag = 0;
        request_ok = 0;
    } else {
        noise_dB = 0;     //if the asnwer from feather M0 is not ok => set the noise to zero
    }

  
  if (collect_data_sensors){

    status_publish = 0;     //before to send, clean the status publish flag

    Serial.println("");
    timestamp = data_time();         //get the timestamp
    Serial.print("Timestamp : "); Serial.println(timestamp);
    
    Serial.print("PM2.5 : "); Serial.println(p25);
    Serial.print("PM10 : "); Serial.println(p10);
       
    data_bme();                             //get temp/hum/pres
    Serial.print("Temperaure (C) : "); Serial.println(temp);
    Serial.print("Humidity (%) : "); Serial.println(hum);
    Serial.print("Pressure (hPa) : "); Serial.println(pres);

    op1_avg = op1_sum/gas_count;            //average the op1
    op2_avg = op2_sum/gas_count;            //average the op2
    Serial.print("NO2 op1 avg : "); Serial.println(op1_avg);
    Serial.print("NO2 op2 avg : "); Serial.println(op2_avg);
    

    Serial.print("Noise (SPLdB) : "); Serial.println(noise_dB);
    Serial.println();

    //send payload
    send_payload();

    //if the publish was success => blink green
    if(status_publish == 1){
      Serial.println("The payload reached the server. yeah");

      pixels.setPixelColor(LedConfig, green);
      pixels.show();
      delay(100);
      pixels.setPixelColor(LedConfig, off);
      pixels.show();
      
    } else {
      save_data_sdcard(); //if the publish not good => save in sd card
    }

    //if it was possible to open the file => blink orange
    if(status_write == true && status_publish == 0){
      Serial.println("The payload reached the server. yeah");

      pixels.setPixelColor(LedConfig, orange);
      pixels.show();
      delay(100);
      pixels.setPixelColor(LedConfig, off);
      pixels.show();
      
    } else if (status_write == false && status_publish == 0) { //if there is not card => Led RED and stays red until the next try
      pixels.setPixelColor(LedConfig, red);
      pixels.show();
    }
    
    op1_avg =0;
    op2_avg =0;
    op1_sum=0;
    op2_sum=0;
    
    gas_count = ecnt;
    collect_data_sensors = false;         //un-flag collect data
  }

  #if DEBUG
    Serial.print("\tECOUNT= "); Serial.print(ecnt); Serial.print("\tGAS op1= "); Serial.print(op1);  Serial.print("\tGAS op2= "); Serial.println(op2);
  #endif
    
  delay(1000);

}





String data_time(){

  // ** get the timestamp
        DateTime now = rtc.now();
        int YY = now.year();
        int MM = now.month();
        int DD = now.day();

        int hh = now.hour();
        int mm = now.minute();
        int ss = now.second();

        String timeString = String(YY) + ":" + String(MM) + ":" + String(DD) + "/" + String(hh) + ":"  + String(mm) + ":" + String(ss);

        return timeString;
  
}


void data_bme(){
  
  temp = 0.00;
  hum = 0.00;
  pres = 0.00;
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


void data_sds(){
  uint8_t mData = 0;
  uint8_t i = 0;
  uint8_t mPkt[10] = {0};
  uint8_t mCheck = 0;

  Status_sds = -1;  // Default: no data availabe ...
  p10 = -1.0;
  p25 = -1.0;

  while (Serial.available() > 0)
  {
    // from www.inovafitness.com
    // packet format: AA C0 PM25_Low PM25_High PM10_Low PM10_High 0 0 CRC AB

    Status_sds = 0; // There is data!

    mData = Serial.read();
    delay(10);//wait until packet is received

    if (mData == 0xAA) // Head1 ok?
    {
      mPkt[0] =  mData;
      mData = Serial.read();

      if (mData == 0xc0) // Head2 ok?
      {
        mPkt[1] =  mData;
        mCheck = 0;
        for (i = 0; i < 6; i++) // Get the content of the package!
        {
          mPkt[i + 2] = Serial.read();
          delay(2);
          mCheck += mPkt[i + 2];
        }

        mPkt[8] = Serial.read();
        delay(5);

        mPkt[9] = Serial.read();
        if (mPkt[9] != 0xAB) {
          Serial.print("\nmData[9] != 0xAB,  but ");
          Serial.println(mPkt[9]);
          Status_sds = 4; LastError_sds = Status_sds; ecnt++;
        }

        if (mCheck == mPkt[8]) // Check CRC
        {
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

        } // CRC ??
        else {
          Serial.print("\nCRC != OK,  ");
          Serial.print(mCheck);
          Serial.print("  !=  ");
          Serial.println(mPkt[8]);
          Status_sds = 3; LastError_sds = Status_sds; ecnt++;
        }

      } // Head2 ??
      else {
        Serial.print("\nmData != 0xc0,  but ");
        Serial.println(mData);
        Status_sds = 2; LastError_sds = Status_sds; ecnt++;
      }

    } // Head1 ??
    else {
      Serial.print("\nmData != 0xAA,  but ");
      Serial.println(mData);
      Status_sds = 1; LastError_sds = Status_sds; ecnt++;
    }
  }
}

void data_gas1(){
  op1 = ads.readADC_Differential_0_1();
  op2 = ads.readADC_Differential_2_3();
}

void send_payload() {

  char mqtt_clientid[15];
  snprintf (mqtt_clientid, 14, "ESP%u", chipid);

  if (mqttClient.connect(mqtt_clientid, mqtt_username, mqtt_password)) {
      Serial.println("MQTT connected.");
      
      long rssi = WiFi.RSSI();

      // send proper JSON startup message
      DynamicJsonBuffer jsonBuffer;
      JsonObject& json = jsonBuffer.createObject();
      json["id"] = chipid;
      json["ts"] = timestamp;
      json["t"] = temp;
      json["h"] = hum;
      json["p"] = pres;
      json["p25"] = p25;
      json["p10"] = p10;
      json["no2op1"] = op1_avg;
      json["no2op2"] = op2_avg;
      json["dB"] = noise_dB;
//      json["rssi"] = rssi;     
      char buf[200];
      json.printTo(buf, sizeof(buf));

      Serial.print("Publish message: ");
      Serial.println(buf);

      char topic_buf[50];
      sprintf(topic_buf, mqtt_topic, chipid);
      status_publish = mqttClient.publish(topic_buf, buf);
      Serial.print("feedback publish :"); Serial.println(status_publish);
  }

}


void save_data_sdcard(){

  status_write = false;
  
  // ** Write Log File Header 
  file = SD.open(file_name, FILE_WRITE); // open "file.txt" to write data
  if (file)
  {
    Serial.println("I opened the log file");
    file.print(chipid); file.print(",");
    file.print(timestamp); file.print(",");
    file.print(temp); file.print(",");
    file.print(hum); file.print(",");
    file.print(pres); file.print(",");
    file.print(p25); file.print(",");
    file.print(p10); file.print(",");
    file.print(op1_avg); file.print(",");
    file.print(op2_avg); file.print(",");
    file.println(noise_dB);
    file.close();
    delay(10);
    status_write = true;
    
  }
  else
  {
    Serial.println("Couldn't open log file");
    status_write = false;
  }
  
}

