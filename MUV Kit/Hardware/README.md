
## Fab pcbs
#### Sensors pcb
| top view  | bottom view |
| ------------- | ------------- |
| <img src="images/sensors_pcb_top_view.png" width="300">| <img src="images/sensors_pcb_bottom_view.png" width="300"> |
| Components placed on the top layer  <br> but soldered on the bottom! <br> <img src="images/sensors_pcb_top_components.png" width="300"> <br> JP1: female THT 1x12 pin header, 2.54pitch <br> JP2: female THT 1x16 pin header 2.54pitch <br> J-GAS, J-DB. J-BME : Grove - Universal 4 Pin connector <br> 05-JST-B5B: Connector Wire-To-Board, B5B-XH-A (LF)(SN) <br> red traces: jumper wires between vias| Components placed on the bottom layer <br> and soldered on the bottom side <br> <img src="images/sensors_pcb_bottom_components.png" width="300"> <br> Q1/Q2 : BSS138 mosfet, also the NDS355AN from the fab inventory is ok. <br> R1/R2/R3/R4 : 10 Kohm <br> D1/D2 : diodes <br><br><br>|
| <img src="images/sensors_pcb_top.jpg" width="300">  | <img src="images/sensors_pcb_bottom.jpg" width="300">|

On the sensors pcb, it is not necessary to use all the connectors.
Depending on the sensors you want you use you can solder the required sockets. The J-GAS-NO1 and J-GAS-NO2 are dedicated to the Alphasense gas input devices. In this repo we are using only one sensor, the NO2 detector and it is connected to J-GAS-NO1.

#### Interface pcb
| top view  | bottom view |
| ------------- | ------------- |
|<img src="images/interface_pcb_top_view.png"  width="300">  | |
|Components placed on the top layer <br> <img src="images/interface_pcb_top.jpg"  width="300">  <br> SW_RST, SW_CONFIG: push button, smd, 6mm <br> LED1: WS2812B smd <br> R1: 10K res, smd, 1206 <br> R2: 470 ohm res, smd, 1206 <br> C1: 0.1uF cap, smd, 1206| Components placed on the bottom layer <br> <img src="images/interface_pcb_bottom.jpg"  width="300"> <br> JP3: female THT 1x12 pin header, 2.54pitch <br> JP1: female THT 1x16 pin header 2.54pitch <br><br><br><br>|

#### Temp/Hum pcb
| top view  | bottom view |
| ------------- | ------------- |
| <img src="images/bm2_pcb_top_view.png"  width="300">  | <img src="images/bm2_pcb_bottom_view.png"  width="300">|
| Components placed on the top layer <br> <img src="images/bme_pcb_top.jpg"  width="300">  <br> J3: female THT 1x6 pin header, 2.54pitch| Components placed on the bottom layer <br> <img src="images/bme_pcb_bottom.jpg"  width="300"> <br> J1: Grove - Universal 4 Pin connector|

The temp/hum pcb is a 2 layers board, the vias can be filled in with solid core wire (diameter < 0.8 mm). More information about the production of a double side pcb are in the [Double side milling](https://github.com/waagsociety/air_quality_sensor_kit/tree/master/MUV%20Kit/Hardware/Double%20side%20milling) section.


#### Gas pcb
| top view  | bottom view |
| ------------- | ------------- |
| <img src="images/gas_pcb_top_view.png"  width="300">  | <img src="images/gas_pcb_bottom_view.png"  width="300">|
| Components placed on the top layer <br> <img src="images/gas_pcb_top.jpg"  width="300">  <br> J2: Grove - Universal 4 Pin connector <br> JP1: female THT 1x10 pin header, 2.54pitch <br><br>| Components placed on the bottom layer <br> <img src="images/gas_pcb_bottom.jpg"  width="300"> <br> R1 or R2: 0 ohm resistor, smd, 1206 <br> C1: 100nF capacitor, smd, 1206 <br> C2: 1uF capacitor, smd, 1206 |

Note: this board can be used for both NO2 and O3 sensor but it is necessary to solder the zero ohm resistor in the different places depending on the gas. For the NO2 sensor the zero ohm resistor must be placed on R1, for the O3 sensor the zero ohm resistor must be placed on R2.


#### Noise pcb
| top view  | bottom view |
| ------------- | ------------- |
| <img src="images/noise_pcb_top_view.png"  width="300">  | |
| Components placed on the top layer <br> <img src="images/noise_pcb_top.jpg"  width="300"> <br> J1: Grove - Universal 4 Pin connector <br> OTS1_short: female THT 1x12 pin header, 2.54pitch <br> OTS1_long: female THT 1x16 pin header, 2.54pitch | There are no components placed on the bottom layer <br> <img src="images/noise_pcb_bottom.jpg"  width="300"> <br><br><br><br>|

## Off-the-shelf pcbs

#### Adafruit Feather esp8266
The [Adafruit Feather HUZZAH ESP8266](https://learn.adafruit.com/adafruit-feather-huzzah-esp8266?view=all) is an 'all-in-one' ESP8226 WiFi development board. The module can be supplied through USB cable with the following specifications:
* Measures 2.0" x 0.9" x 0.28" (51mm x 23mm x 8mm) without headers soldered in
* Light as a (large?) feather - 6 grams
* ESP8266 @ 80MHz or 160 Mhz with 3.3V logic/power
* 4MB of FLASH (32 Mbit)
* 3.3V regulator with 500mA peak current output
* CP2104 USB-Serial converter onboard with 921600 max baudrate for uploading
* Auto-reset support for getting into bootload mode before firmware upload
* 9 GPIO pins - can also be used as I2C and SPI
* 1 x analog inputs 1.0V max
* Pin #0 red LED for general purpose blinking. Pin #2 blue LED for bootloading debug & general purpose blinking
* Power/enable pin
* 4 mounting holes
* Reset button<br>

In this application the feather esp8266 is responsible to collect the measurements of the sensors and send them, via WiFi, to the dedicated server.
When the Wifi network is not available it can save the data on a SD-card, embedded on a different pcb, see section below.

The feather esp8266 requires female stacking headers so it can be plugged on the sensors pcb and it can host, on top, the RTC/SD-card pcb. The headers must be bought separately and they must be solder on the feather esp8266 board.

<p align="center"> <img src="images/feather_esp_1.jpg" width="200"> <img src="images/feather_esp_2.jpg" width="200"> </p>

#### Adafruit Feather RTC and SD-card
The [Adalogger FeatherWing - RTC + SD](https://learn.adafruit.com/adafruit-adalogger-featherwing/overview) is compatible with the feather esp8266 and it features:
* an I2C real time clock (PCF8523) with 32KHz crystal and battery backup.
* microSD socket that connects to the SPI port pins. A CR1220 coin cell is required to use the RTC battery-backup capabilities.

As for the feather esp8266, it needs the female stacking headers that don't come with the board but they must be separately bought and soldered.

 <p align="center"> <img src="images/sd_rtc.jpg" width="200"> </p>


#### BME280 breakout board
The [bme280](https://www.bosch-sensortec.com/bst/products/all_products/bme280) sensor is so small (2.5x2.5 mm) that it is easier to work with its breakout board. The sensor works at 3.3V and it can communicate directly with the feather esp8266 that is also run at 3.3V, it means no need of level shifters between the two. Depending on where you buy the device, the board could already have the pin header, otherwise it is necessary to solder a 1x6 male header, 2.54mm pitch.

<p align="center"><img src="images/bme.jpg" width="200">
</p>

#### ADS1115 breakout board
The [ADS1115 board](https://learn.adafruit.com/adafruit-4-channel-adc-breakouts/) from Adafruit is the breakout board of the high-resolution Analog-to-Digital converter ADS1115. It is used in the reading of small voltages from the Gas sensors. The board can run from 2ßV up to 5V, in the kit it is supplied at 5V, same domain of the gas sensor, and it executes a two differential input readings.<br>
The [ADS1115](http://www.ti.com/lit/ds/symlink/ads1115.pdf) features:
* Resolution: 16 Bits
* Programmable Sample Rate: 8 to 860 Samples/Second
* Power Supply/Logic Levels: 2.0V to 5.5V
* Low Current Consumption: Continuous Mode: Only 150µA * Single-Shot Mode: Auto Shut-Down
* Internal Low-Drift Voltage Reference
* Internal Oscillator
* Internal PGA: up to x16
* I2C Interface: 4-Pin-Selectable Addresses
* Four Single-Ended or 2 Differential Inputs
* Programmable Comparator  

In the sensor kit, the ADS1115 board needs a 1x10 male pin header, pitch 2.54mm.

<p align="center"> <img src="images/ads1115.jpg" width="200"> </p>


#### Adafruit Feather M0 Basic Proto
The Feather M0 Basic Proto has the ATSAMD21G18 ARM Cortex M0 processor as core, it clocked at 48MHz with 3,3V logic. Some specs:

* Measures 2.0" x 0.9" x 0.28" (51mm x 23mm x 8mm) without headers soldered in
* Light as a (large?) feather - 4.6 grams
* ATSAMD21G18 @ 48MHz with 3.3V logic/power
* 256KB of FLASH + 32KB of RAM
* 32.768 Khz crystal for clock generation & RTC
* 3.3V regulator with 500mA peak current output
* USB native support, comes with USB bootloader and * serial port debugging
* 20 GPIO pins
* Hardware Serial, hardware I2C, hardware SPI support
PWM outputs on all pins
* 6 x 12-bit analog inputs
* 1 x 10-bit analog output (DAC)
* Built in 100mA lipoly charger with charging status indicator LED, not in used in sensor kit.
* Pin #13 red LED for general purpose blinking
* Power/enable pin
* 4 mounting holes
* Reset button

In the kit the feather M0 is responsible to communicate with mic for the noise measurements and it sends the data to the feather esp8266 over serial.
The connect the fetaher M0 to pheripheral devices is necessary to solder:
* two 1x3 female pin headers on the proto part of the feather, as socket for the mic. See image below for connection.
* the pin headers: one 1x12 male pin and one 1x16 male pin header, both 2.54mm pitch.

As it is shown in the images below, the socket for the mic is connected to three GPIO pins of the feather M0.
It is handy to NOT solder the long on these three GPIO pads and keep the pads available for the wires of the mic.

<p align="center"> <img src="images/feather_M0_1.jpg" width="200"> <img src="images/feather_M0_2.jpg" width="200"><br>
<img src="images/feather_M0_3.jpg" width="200"> <img src="images/feather_M0_4.jpg" width="200"></p>

#### ICS-43432 breakout board
The ICS-43432 is the mic used to measure noise, [see](https://github.com/waagsociety/air_quality_sensor_kit/tree/master/MUV%20Kit#Noise%sensor).

Due to the small dimensions, it is handy to work with the breakout board released buy [Arduino](https://store.arduino.cc/ics43432-i2s-digital-microphone). The pcb need two 1x3 male pin headers.
The biggest advantage of this module is the I2S audio as a stream of 24-bit serial words that can be directly read by your mcu board with an I2S port. No CODEC is required to decode the audio data!

<p align="center"> <img src="images/mic.jpg" width="300"> </p>



## Modules and assembly
#### Central module
| <img src="images/central_module_1.jpg" width="300"> |<img src="images/central_module_2.jpg" width="300">  |
| ------------- | ------------- |
| <img src="images/central_module_3.jpg" width="300">|<img src="images/central_module_4.jpg" width="300"> |

#### Temp/Hum module
| <img src="images/temp_module_1.jpg" width="300"> |<img src="images/temp_module_2.jpg" width="300"> |
| ------------- | ------------- |


#### Gas module
| <img src="images/gas_module_1.jpg" width="300"> | <img src="images/gas_module_2.jpg" width="300"> |
| ------------- | ------------- |
|  <img src="images/gas_module_3.jpg" width="300"> | <img src="images/gas_module_4.jpg" width="300"> |

#### Noise module
| <img src="images/noise_module_1.jpg" width="300"> |<img src="images/noise_module_2.jpg" width="300"> |
| ------------- | ------------- |

#### Final Assembly
The SDS011 sensor connects to the sensors pcb with its cable. All the other modules use Grove - Universal 4 Pin Unbuckled 20cm Cable.
<img src="images/final_assembly.jpg">

## Power Supply

## BOM notes
Many items in the BOM can be bought only in minimum amount, when you actually are going to use one or two component. The total cost is therefore expressed as:
* total purchase: the cost of the purchase, buying the minimum amount per items
* cost of 1 kit: considering the cost of only the component used in the kit.

for example: if you buy the "feather header 1" from Farnell, you have to buy 10 components even if you are going to use only one. In the purchase cost the item "feather header 1" is 12€ but in the single cost of one kit, the item value is 1,2€.

The kit showed in this repo doesn't have the O3 sensor, but the you can find indication about the boards and how to connect it to the kit.
