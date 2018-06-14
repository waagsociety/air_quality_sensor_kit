# Component descriptions
![components](https://user-images.githubusercontent.com/31654421/40971480-1034dc5a-68be-11e8-9307-9f7e7ecde65f.jpg)

## NO2 sensor
The used NO2 sensor is the Alphasense no2-b43f. Previous tests with the specific sensors led to a formula to calculate the correct NO2 concentration from the measurements of the sensor. This formula can be found in   
http://www.alphasense.com/index.php/products/nitrogen-dioxide-2/

## DC motor
In this prototype we used a Jameco relia pro 253471. We started with a smaller 6V dc motor but it was not strong enough to rotate the bird. Therefore we switched to this stronger motor. Something remarkable about the use of the motor is that the motor itself is connected to the object, and the small pin is fixed. In this way the motor spins instead of the small pin coming out of the motor. 

## Power supply
The Jameco relia pro needs 12 V, this is why we needed a power supply that was strong enough. The adapter showed at the following picture is the one we used for this prototype. It's the SYS1308-2412-W2E of the brand Sunny. 

![img_2213klein](https://user-images.githubusercontent.com/31654421/40981644-bd8be674-68db-11e8-80a4-dcecf0c7a6ab.jpg)

In this case, it would be a better fit to have a wireless power supply, since the object rotates, and to be placed more easily. 

## A/D Converter
In the prototype we use the 16 bit ads1115 adafruit a/d converter. In the arduino code we used the library Adafruit_ADS1015.h.
Using this converter, we were able to read the small differences the NO2 sensor measures. The converter of the Arduino is not precise enough for these measurements.

https://cdn-learn.adafruit.com/downloads/pdf/adafruit-4-channel-adc-breakouts.pdf

## Motor driver
This driver is used to be able to control the speed, the direction and the duration of the rotation of the motor. Triple check if the wires are connected correctly! A small mix up could destroy the motor driver, by blowing up the protection diode. 

Hook-up guide motor driver:
https://learn.sparkfun.com/tutorials/tb6612fng-hookup-guide?_ga=2.230587740.1734036102.1528462719-1433206558.1520845841

## Circuit board
The circuit board is not as structured as I hoped but it works. The motor driver, the tilt switch and a resistor are on the back of the board while the dc motor and the power supply are connected to the front. 

![img_2211](https://user-images.githubusercontent.com/31654421/41039772-c35cd792-699a-11e8-8019-fc1010be7bb1.jpg)
![img_2208](https://user-images.githubusercontent.com/31654421/41039788-cc8b80f2-699a-11e8-8721-16809df29891.jpg)

This schematic circuit clearer shows what the circuit looks like:
![bird circuit](https://user-images.githubusercontent.com/31654421/41159503-04911dfc-6b2d-11e8-90c6-c00dcc495404.jpg)

## Tiltswitch
Using a tilt switch, the bird always knows in what position it's hanging.
This is the tilt switch used in the prototype:
https://uk.rs-online.com/web/p/non-mercury-tilt-switches/3615087/
This image (source: http://iamtechnical.com/tilt-switch) shows how tilt switches work:
![tilt-switch](https://user-images.githubusercontent.com/31654421/41160310-8938efba-6b2f-11e8-83e4-a8784ac12387.jpg)

