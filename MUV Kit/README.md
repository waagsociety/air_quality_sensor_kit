# Air quality sensor kit for MUV
The kit developed for MUV is a modular system on which the following sensors can be attached:
* dust sensor
* temperature/humidity/pressure sensor
* noise sensor
* nitrogen dioxide sensor
* ozone sensor


The pcbs used in the kit are both off-the-shelf products and produced in the fablab.

## Dust sensor
The dust sensor is the [SDS011](http://aqicn.org/sensor/sds011/) from [inovafitness](http://inovafitness.com/en/a/chanpinzhongxin/95.html) and it uses the principle of laser scattering technology. The device is capable to measure the concentration of PM2.5, nominally it measures also PM10 but research papers shows the fact that the PM10 measurement is an extrapolation based on PM2.5 data.
Based on such results we will use the sensor to measure only PM2.5. The communication between the sensor and the main mcu is over serial.

The specifications are:
- Output: PM2.5, PM10
- Measuring Range: 0.0-999.9μg/m3
- Input Voltage:5V
- Maximum Current: 100mA
- Sleep Current:2mA
- Response Time1 second
- Serial Data Output Frequency: 1 time/second
- Particle Diameter Resolution:≤0.3μm
- Relative Error:10%
- Temperature Range:-20~50°C
- Physical Size: 71mm*70mm*23mm

The dust sensor doesn't need any support pcb, the cable you get with the sensor can be used to plug the device to the main sensors board, see hardware XXX.

## Temp/Hum/Pressure sensor
To measure temperature and humidity, the kit include the [BME280 sensor](https://www.bosch-sensortec.com/bst/products/all_products/bme280). The operation ranges are:<br>
Pressure: 300...1100 hPa<br>
Temperature: -40…85°C<br>

The average current consumption (1Hz data refresh rate) is:<br>
1.8 μA @ 1 Hz (H, T)<br>
2.8 μA @ 1 Hz (P, T)<br>
3.6 μA @ 1 Hz (H, P, T)<br>
and the average current consumption in sleep mode is 0.1 μA.

The Humidity sensor:<br>
Response time (τ63%): 1 s
Accuracy tolerance: ± 3 % relative humidity
Hysteresis: ≤ 2 % relative humidity

The Pressure sensor:<br>
RMS Noise: 0.2 Pa (equiv. to 1.7 cm)
Sensitivity Error: ± 0.25 % (equiv. to 1 m at 400 m height change)
Temperature coefficient offset: ±1.5 Pa/K (equiv. to ±12.6 cm at 1°C temperature change)


It uses I2C or SPI communication interface, in the kit it adopts the first one.
