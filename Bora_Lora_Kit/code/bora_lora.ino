
//Bora Lora Kit

/*******************************************************************************
Copyright (c) <2017> <Emma Pareschi>

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
 *******************************************************************************/


/*******************************************************************************

This code uses the IBM lmic.h library: https://github.com/matthijskooijman/arduino-lmic

define radio type in config.h file,
for RFM95W: #define CFG_sx1276_radio 1   

 *******************************************************************************/

#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include "DHT.h"

#define DEBUG 1

//---------------------------
// Dust Sensor, Shinyei PD42

//source: https://github.com/dustduino/DustDuinoSerial
//By Matthew Schroyer, MentalMunition.com

// DESCRIPTION
// Outputs particle concentration per cubic foot, 
// from a Shinyei particuate matter sensor.

// THEORY OF OPERATION
// Sketch measures the width of pulses through
// boolean triggers, on each channel.
// Pulse width is converted into a percent integer
// of time on, and equation uses this to determine
// particle concentration.

float pm10 = 0.0;
float pm2_5 = 0.0;

unsigned long starttime;

unsigned long triggerOnP1;
unsigned long triggerOffP1;
unsigned long pulseLengthP1;
unsigned long durationP1;
boolean valP1 = HIGH;
boolean triggerP1 = false;

unsigned long triggerOnP2;
unsigned long triggerOffP2;
unsigned long pulseLengthP2;
unsigned long durationP2;
boolean valP2 = HIGH;
boolean triggerP2 = false;

float ratioP1 = 0;
float ratioP2 = 0;
unsigned long sampletime_ms = 30000;
float countP1;
float countP2;

#define DUST_P1_PIN 3      // Dust sensor PM2.5 pin
#define DUST_P2_PIN 2      // Dust senspr PM10 pin


//---------------------------
// temperature and humidity sensor, DHT22

#define DHTPIN A3
#define DHTTYPE DHT22   // DHT 22  (AM2302)

DHT dht(DHTPIN, DHTTYPE);


//---------------------------
// NO2, Alphasense 
#define LOG_INTERVAL  925  // mills between entries (reduce to take more/faster data)

// VOOR DE ADC
// Averagigng period:
int  FLAVG = 5; //32;  // 60

Adafruit_ADS1115 ads1115;

float Vstep = 5.0 / 32768.0 * 1000.0;  // Assuming 16 bits adc, units is mV
int  iptr = 0;                          // iptr is the pointer to the table holding the second-values
// and can be used as the number of seconds in the minute.

int16_t results1;
int16_t results2;

uint32_t acc_op1 = 0;                       // accumulator 1 value
uint32_t acc_op2 = 0;                       // accumulator 2 value


//---------------------------
//VOOR LORA
// LoRaWAN NwkSKey, network session key
// This is the default Semtech key, which is used by the prototype TTN
// network initially.

//device 
  static const PROGMEM u1_t NWKSKEY[16] = { 0x36, 0x37, 0xC4, 0xF3, 0x56, 0x82, 0x2D, 0x9C, 0x28, 0xF9, 0xD3, 0x95, 0x28, 0x88, 0xDD, 0x26 };
  static const u1_t PROGMEM APPSKEY[16] = { 0x8D, 0xA3, 0xC9, 0xAF, 0xB7, 0x2E, 0x14, 0xC6, 0xE1, 0x6C, 0x38, 0xCD, 0x37, 0x33, 0x95, 0x97 };
  static const u4_t DEVADDR = 0x260112B4;

// These callbacks are only used in over-the-air activation, so they are
// left empty here (we cannot leave them out completely unless
// DISABLE_JOIN is set in config.h, otherwise the linker will complain).
void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }

byte mydata[12];
int8_t no2[4];
int8_t pm[4];

static osjob_t sendjob;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 1;

// Pin mapping with shield pnly radio
const lmic_pinmap lmic_pins = {
    .nss = 10,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 9,
    .dio = {7, 5, 6},
};

void onEvent (ev_t ev) {
  Serial.print(os_getTime());
  Serial.print(": ");
  switch (ev) {
    case EV_SCAN_TIMEOUT:
      Serial.println(F("EV_SCAN_TIMEOUT"));
      break;
    case EV_BEACON_FOUND:
      Serial.println(F("EV_BEACON_FOUND"));
      break;
    case EV_BEACON_MISSED:
      Serial.println(F("EV_BEACON_MISSED"));
      break;
    case EV_BEACON_TRACKED:
      Serial.println(F("EV_BEACON_TRACKED"));
      break;
    case EV_JOINING:
      Serial.println(F("EV_JOINING"));
      break;
    case EV_JOINED:
      Serial.println(F("EV_JOINED"));
      break;
    case EV_RFU1:
      Serial.println(F("EV_RFU1"));
      break;
    case EV_JOIN_FAILED:
      Serial.println(F("EV_JOIN_FAILED"));
      break;
    case EV_REJOIN_FAILED:
      Serial.println(F("EV_REJOIN_FAILED"));
      break;
      break;
    case EV_TXCOMPLETE:
      Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
      if (LMIC.dataLen) {
        // data received in rx slot after tx
        Serial.print(F("Data Received: "));
        Serial.write(LMIC.frame + LMIC.dataBeg, LMIC.dataLen);
        Serial.println();
      }
      // Schedule next transmission
      os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
      break;
    case EV_LOST_TSYNC:
      Serial.println(F("EV_LOST_TSYNC"));
      break;
    case EV_RESET:
      Serial.println(F("EV_RESET"));
      break;
    case EV_RXCOMPLETE:
      // data received in ping slot
      Serial.println(F("EV_RXCOMPLETE"));
      break;
    case EV_LINK_DEAD:
      Serial.println(F("EV_LINK_DEAD"));
      break;
    case EV_LINK_ALIVE:
      Serial.println(F("EV_LINK_ALIVE"));
      break;
    default:
      Serial.println(F("Unknown event"));
      break;
  }
}

////-------------------SENSORS FUNCTIONS

//DUST
int8_t * call_pm(){

  starttime = millis();
  durationP1 = 0;
  durationP2 = 0;

  // take samples for 30 seconds
  while ((millis() - starttime) < sampletime_ms) {
 
    valP1 = digitalRead(DUST_P1_PIN); // 3
    valP2 = digitalRead(DUST_P2_PIN);  // 2
  
    if (valP1 == LOW && triggerP1 == false) {
      triggerP1 = true;
      triggerOnP1 = micros();
    }
  
    if (valP1 == HIGH && triggerP1 == true) {
      triggerOffP1 = micros();
      pulseLengthP1 = triggerOffP1 - triggerOnP1;
      durationP1 = durationP1 + pulseLengthP1;
      triggerP1 = false;
    }
  
    if (valP2 == LOW && triggerP2 == false) {
      triggerP2 = true;
      triggerOnP2 = micros();
    }
  
    if (valP2 == HIGH && triggerP2 == true){
      triggerOffP2 = micros();
      pulseLengthP2 = triggerOffP2 - triggerOnP2;
      durationP2 = durationP2 + pulseLengthP2;
      triggerP2 = false;
    }
  }

  // sampling is over, calculate results
      
  ratioP1 = durationP1/(sampletime_ms*10.0);  // Integer percentage 0=>100
  ratioP2 = durationP2/(sampletime_ms*10.0);
  countP1 = 1.1*pow(ratioP1,3)-3.8*pow(ratioP1,2)+520*ratioP1+0.62;
  countP2 = 1.1*pow(ratioP2,3)-3.8*pow(ratioP2,2)+520*ratioP2+0.62;
  float PM10count = countP2;
  float PM25count = countP1 - countP2;

  int16_t pm10 = PM10count;
  int16_t pm2_5 = PM25count;

    pm[0] = pm2_5 >> 8;
    pm[1] = pm2_5 & 0xFF;
    pm[2] = pm10 >> 8 ;
    pm[3] = pm10 & 0xFF;

    return pm;
}


//NO2
int8_t * call_no2() {
  Serial.println("Start NO2 measurments...");
    iptr=0;
  
    while (iptr < FLAVG) {
    results1 = ads1115.readADC_Differential_0_1();    // Read ADC ports 0 and 1    
    results2 = ads1115.readADC_Differential_2_3();    // Read ADC ports 2 and 3
    
    acc_op1 += results1;
    acc_op2 += results2;

    iptr++;

    // delay for the amount of time we want between readings
    delay(LOG_INTERVAL);
  }

    //-------------------------------------------  

    int16_t no2_a = 0;
    int16_t no2_b = 0;
    
    no2_a = acc_op1 / FLAVG;
    no2_b = acc_op2 / FLAVG;

    iptr = 0;
    acc_op1 = 0;
    acc_op2 = 0;

    int16_t op1 = no2_a;
    int16_t op2 = no2_b;

    no2[0] = op1 >> 8;
    no2[1] = op1 & 0xFF;
    no2[2] = op2 >> 8 ;
    no2[3] = op2 & 0xFF;

#ifdef DEBUG
    Serial.print("no2[0]: ");
    Serial.println(no2[0]);
    Serial.print("no2[1]: ");
    Serial.println(no2[1]);
    Serial.print("no2[2]: ");
    Serial.println(no2[2]);
    Serial.print("no2[3]: ");
    Serial.println(no2[3]);
#endif
    
    return no2;   
}

//TEMPERATURE
float call_temp(){
  //float temp_random = random(0, 50)/1.7;
  //return temp_random;
  int16_t temp = 0;
  return temp = dht.readTemperature();
    
}

//HUMIDITY
int8_t call_hum(){
  int8_t hum = 0;
  return hum = dht.readHumidity();
  }


//---------------------------
// SENDING FUNCTION
void do_send(osjob_t* j) {
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println(F("OP_TXRXPEND, not sending"));
  } else {
    
    // Prepare upstream data transmission at the next possible time.
    int8_t *no2_aux = call_no2();
    int16_t temp_aux = (int16_t)(call_temp()*100);
    int8_t hum_aux = call_hum();
    int8_t *pm_aux = call_pm();
    
#ifdef DEBUG
    Serial.print("no2 array: ");
    for(int i = 0; i<4; i++){
    Serial.println(*(no2_aux+i));
    }
    Serial.print("temp random: ");
    Serial.println(temp_aux);
    Serial.print("hum random: ");
    Serial.println(hum_aux);
    Serial.print("pm array: ");
    for(int i = 0; i<4; i++){
    Serial.println(*(pm_aux+i));
    }
#endif

    //build the array
    //op1
    mydata[0] = *(no2_aux);
    mydata[1] = *(no2_aux+1);
    //op2
    mydata[2] = *(no2_aux+2);
    mydata[3] = *(no2_aux+3);   
    //temp
    mydata[4] = temp_aux >> 8;
    mydata[5] = temp_aux & 0xFF;
    //hum
    mydata[6] = hum_aux;
    //pm2.5
    mydata[7] = *(pm_aux);
    mydata[8] = *(pm_aux+1);
    //pm10
    mydata[9] = *(pm_aux+2);
    mydata[10] = *(pm_aux+3);
   
    LMIC_setTxData2(1, mydata, sizeof(mydata), 0);
    Serial.println(F("Packet queued"));

#ifdef DEBUG
    Serial.println(sizeof(mydata));
    for (int i = 0; i<11; i++){
    Serial.print("mydata[");
    Serial.print(i);
    Serial.print("]: ");
    Serial.println(mydata[i]);
    }
#endif

  }

}



void setup() {

  pinMode(DHTPIN, INPUT);
  dht.begin();

  pinMode(DUST_P1_PIN, INPUT); 
  pinMode(DUST_P2_PIN, INPUT); 
    
  Serial.begin(115200);
  Serial.println(F("Starting"));

#ifdef VCC_ENABLE
  // For Pinoccio Scout boards
  pinMode(VCC_ENABLE, OUTPUT);
  digitalWrite(VCC_ENABLE, HIGH);
  delay(1000);
#endif

  // LMIC init
  os_init();
  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();

  // Set static session parameters. Instead of dynamically establishing a session
  // by joining the network, precomputed session parameters are be provided.
#ifdef PROGMEM
  // On AVR, these values are stored in flash and only copied to RAM
  // once. Copy them to a temporary buffer here, LMIC_setSession will
  // copy them into a buffer of its own again.
  uint8_t appskey[sizeof(APPSKEY)];
  uint8_t nwkskey[sizeof(NWKSKEY)];
  memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
  memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));
  LMIC_setSession (0x1, DEVADDR, nwkskey, appskey);
#else
  // If not running an AVR with PROGMEM, just use the arrays directly
  LMIC_setSession (0x1, DEVADDR, NWKSKEY, APPSKEY);
#endif

  // Set up the channels used by the Things Network, which corresponds
  // to the defaults of most gateways. Without this, only three base
  // channels from the LoRaWAN specification are used, which certainly
  // works, so it is good for debugging, but can overload those
  // frequencies, so be sure to configure the full frequency range of
  // your network here (unless your network autoconfigures them).
  // Setting up channels should happen after LMIC_setSession, as that
  // configures the minimal channel set.
  LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI);      // g-band
  LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK,  DR_FSK),  BAND_MILLI);      // g2-band
  // TTN defines an additional channel at 869.525Mhz using SF9 for class B
  // devices' ping slots. LMIC does not have an easy way to define set this
  // frequency and support for class B is spotty and untested, so this
  // frequency is not configured here.

  // Disable link check validation
  LMIC_setLinkCheckMode(0);

  // Set data rate and transmit power (note: txpow seems to be ignored by the library)
  LMIC_setDrTxpow(DR_SF7, 14);

  do_send(&sendjob);

  ads1115.begin();

}

void loop() {

    os_runloop_once();
    delay(2200);
    
}


