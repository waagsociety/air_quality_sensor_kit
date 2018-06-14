/******************************************************************************

The printstatements can be used to check whether the system is working. 
These can be omitted when everything works and the system isn't connected to the computer.

ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)

******************************************************************************/

#include <SparkFun_TB6612.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */

#define PWMA 3
#define AIN1 8
#define AIN2 9
#define STBY 10

// these constants are used to allow you to make your motor configuration 
// line up with function names like forward.  Value can be 1 or -1
const int offsetA = 1;
 
// Initializing motor.  The library will allow you to initialize as many
// motors as you have memory for.  If you are using functions like forward
// that take 2 motors as arguements you can either write new functions or
// call the function more than once.
Motor motor = Motor(AIN1, AIN2, PWMA, offsetA, STBY);

// initializing tilt switch
int tiltSwitch = 13;

void setup()
{   
   Serial.begin(9600);
   pinMode(tiltSwitch, INPUT);

   ads.begin();
}

void loop()
{
    int switchState = digitalRead(tiltSwitch);
    int threshold = 80;

    int16_t adc0, adc2;
    adc0 = ads.readADC_SingleEnded(0);
    adc2 = ads.readADC_SingleEnded(2);
  
    float gem = OneValue(adc0, adc2);
  
    Serial.println(gem); 

    // Use of the drive function which takes as arguements the speed
    // and optional duration.  A negative speed will cause it to go
    // backwards.  Speed can be from -255 to 255.  Also use of the 
    // brake function which takes no arguements.
    
    if (switchState == 0 && abs(gem) >= threshold){
      Serial.println("dying");
      motor.drive(100,720);
      motor.standby();
      delay(1000);
    }
    else if (switchState == 1 && abs(gem) <= threshold){
      Serial.println("getting alive");
      motor.drive(-100,720);
      motor.standby();
      delay(1000);
    }
    
    Serial.println("situation:   "); Serial.print(switchState);
    Serial.println("");
    delay(500);
}

// function to convert a value to milivoltage
float tomV (int value) {
  float converted = (value * 0.1875);
  return converted;
}

// function to convert the measured values to the concentration NO2 in ug/m3
// the formula is based on the outcomes of the research on the sensor 
float OneValue (int value1, int value2) {
  float gem = ((tomV(value1) - 232) - (tomV(value2) - 234)) / (-261);
  float toConcentration = (gem * 1000) * 0.0409 * 46.0055;
  return toConcentration;
}
