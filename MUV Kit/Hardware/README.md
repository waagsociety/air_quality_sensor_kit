
## Fab pcbs
#### Sensors pcb
| top view  | bottom view |
| ------------- | ------------- |
| <img src="images/sensors_pcb_top_view.png" width="300">| <img src="images/sensors_pcb_bottom_view.png" width="300"> |
| Components placed on the top layer <br> <img src="images/sensors_pcb_top_components.png" width="300"> <br> JP1: female THT 1x12 pin header, 2.54pitch <br> JP2: female THT 1x16 pin header 2.54pitch <br> J-GAS, J-DB. J-BME : Grove - Universal 4 Pin connector <br> 05-JST-B5B: Connector Wire-To-Board, B5B-XH-A (LF)(SN) <br> traces: jumper wires| Components placed on the bottom layer <br> <img src="images/sensors_pcb_bottom_components.png" width="300"> <br> Q1/Q2 : BSS138 mosfet, also the NDS355AN from the fab inventory is ok. <br> R1/R2/R3/R4 : 10 Kohm <br> <br><br><br><br>|
| <img src="images/sensors_pcb_top.jpg" width="300">  | <img src="images/sensors_pcb_bottom.jpg" width="300">|

On the sensors pcb, it is not necessary to use all the connectors.
Depending on the sensors you want you use you can solder the required sockets. The J-GAS-NO1 and J-GAS-NO2 are dedicated to the Alphasense gas input devices. In this repo we are using only one sensor, the NO2 detector and it is connected to J-GAS-NO1.

#### Interface pcb
| top view  | bottom view |
| ------------- | ------------- |
|<img src="images/interface_pcb_top_view.png"  width="300">  | |
|Components placed on the top layer <br> <img src="images/interface_pcb_top.jpg"  width="300">  <br> SW_RST, SW_CONFIG: push button, smd, 6mm <br> LED1: WS2812B smd <br> R1: 10K res, smd, 1206 <br> R2: 470 ohm res, smd, 1206 <br> C1: 0.1uF cap, smd, 1206| Components placed on the top layer <br> <img src="images/interface_pcb_bottom.jpg"  width="300"> <br> JP3: female THT 1x12 pin header, 2.54pitch <br> JP1: female THT 1x16 pin header 2.54pitch <br><br><br><br>|

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
* an I2C real time clock (PCF8523) with 32KHz crystal and battery backup
* microSD socket that connects to the SPI port pins. A CR1220 coin cell is required to use the RTC battery-backup capabilities.

As for the feather esp8266, it can be used in the kit, with the female stacking headers that don't come with the board but they must be separately bought and soldered.

 <p align="center"> <img src="images/sd_rtc.jpg" width="200"> </p>


#### BME280 breakout board
| <img src="images/bme.jpg" width="300"> ||
| ------------- | ------------- |

#### ADS1115 breakout board
| <img src="images/ads1115.jpg" width="300"> ||
| ------------- | ------------- |

#### Adafruit Feather M0 basic
| <img src="images/feather_M0_1.jpg" width="300"> |<img src="images/feather_M0_2.jpg" width="300"> |
| ------------- | ------------- |
| <img src="images/feather_M0_3.jpg" width="300"> | <img src="images/feather_M0_4.jpg" width="300"> |

#### ICS-43432 breakout board
| <img src="images/mic.jpg" width="300"> ||
| ------------- | ------------- |




## Modules and assembly
#### Central module
| <img src="images/central_module_1.jpg" width="300"> | |
| ------------- | ------------- |
| <img src="images/central_module_2.jpg" width="300"> | |
| <img src="images/central_module_3.jpg" width="300"> | |
| <img src="images/central_module_4.jpg" width="300"> | |

#### Temp/Hum module
| <img src="images/temp_module_1.jpg" width="300"> | |
| ------------- | ------------- |
| <img src="images/temp_module_2.jpg" width="300"> | |

#### Gas module
| <img src="images/gas_module_1.jpg" width="300"> | |
| ------------- | ------------- |
| <img src="images/gas_module_2.jpg" width="300"> | |
| <img src="images/gas_module_3.jpg" width="300"> | |
| <img src="images/gas_module_4.jpg" width="300"> | |

#### Noise module
| <img src="images/noise_module_1.jpg" width="300"> | |
| ------------- | ------------- |
| <img src="images/noise_module_2.jpg" width="300"> | |

#### Final Assembly
<img src="images/final_assembly.jpg">
