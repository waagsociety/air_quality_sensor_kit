# Air quality sensor kit for MUV

<p align="center"><img src="images/sensor_kit.jpg" width="500"></p>

This repository is about the Sensors Kit developed for the project [MUV](https://www.muv2020.eu/) and contains the following parts:
- this page: you will find on this page the instructions on how it works.
- [Sensors](https://github.com/waagsociety/air_quality_sensor_kit/tree/master/MUV%20Kit/Sensors) folder: it contains the list of sensors that can be connected to the Kit.
- [Hardware](https://github.com/waagsociety/air_quality_sensor_kit/tree/master/MUV%20Kit/Hardware) folder: it contains block diagram, digital files, original design files, layout view, instruction on how to produce the pcbs in a fablab and BOM.
- [Firmware](https://github.com/waagsociety/air_quality_sensor_kit/tree/master/MUV%20Kit/Firmware) folder: it contains the sketches used in the Kit.
- [Case](https://github.com/waagsociety/air_quality_sensor_kit/tree/master/MUV%20Kit/Case) folder: it contains the instructions for the case.

## Intro
The MUV Sensors kit is a device developed to measure the following environmental quantities:
* dust
* temperature/humidity/pressure sensor
* noise
* nitrogen dioxide and ozone.

The kit has a WiFi module that collects the data from the sensors and send them to the MUV MQTT-server. If the WiFi is missing the kit saves the data on an SD-card.

NOTE: when you power on the kit, it must be able to connect to a WiFi otherwise it will not start the measurements.

Once the Kit is powered on and connected to the WiFi it will run automatically.
