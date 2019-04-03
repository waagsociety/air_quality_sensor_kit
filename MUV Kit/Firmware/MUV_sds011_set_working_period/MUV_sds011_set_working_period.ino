/*
 * March 2019 - MUV project
 * This code is based on the example "customWorkingPeriod.ino" provided with the library SdsDustSensor.h
 * 
 * With this code, you program the SDS011 to send the data every 3minutes: 
 * -> 150 seconds pause (during this time the sensor doesn't measure)
 * -> 30 seconds measurements
 * -> send data
 * 
 * Connections:
 * Arduino pin 2 -> SDS pin TX
 * Arduino pin 3 -> SDS pin RX
 * Arduino pin 5V -> SDS pin 5V
 * Arduino pin GND -> SDS pin GND
 * 
*/

#include "SdsDustSensor.h" . //https://github.com/lewapek/sds-dust-sensors-arduino-library

int rxPin = 2;
int txPin = 3;
SdsDustSensor sds(rxPin, txPin);

void setup() {
  Serial.begin(9600);
  sds.begin();

  Serial.println(sds.queryFirmwareVersion().toString()); // prints firmware version
  Serial.println(sds.setActiveReportingMode().toString()); // ensures sensor is in 'active' reporting mode
  Serial.println(sds.setCustomWorkingPeriod(3).toString()); // sensor sends data every 3 minutes
}

void loop() {
  PmResult pm = sds.readPm();
  if (pm.isOk()) {
    Serial.print("PM2.5 = ");
    Serial.print(pm.pm25);
    Serial.print(", PM10 = ");
    Serial.println(pm.pm10);

    Serial.println(pm.toString());
  } else {
    // notice that loop delay is set to 5s (sensor sends data every 3 minutes) and some reads are not available
    Serial.print("Could not read values from sensor, reason: ");
    Serial.println(pm.statusToString());
  }
}
