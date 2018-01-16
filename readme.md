# arduino-lorawan-rdk

This repository contains experiment sketches for the AllThingsTalk NB-IoT Rapid Development Kit.

> It is to be used in conjunction with the [arduino-nbiot-sdk](https://github.com/allthingstalk/arduino-nbiot-sdk) which contains all basic functionality.

External libraries are included in this repository for use of all sensors

* [AirQuality2](https://github.com/MikeHg/AirQualitySensor/tree/d6cadaf21c6beae99fdd65bb037424ce6f855db1)
* [MMA7660](https://github.com/Seeed-Studio/Accelerometer_MMA7660) (Accelerometer)
* [TPH2](http://support.sodaq.com/sodaq-one/tph-v2/) Adafruit_Sensor and Adafruit_BME280

## Hardware

This library has been developed for:

* Sodaq Mbili
* Sodaq UBEE (ublox SARA-N211 nb-iot radio)

## Installation

Download the source code and copy the content of the zip file to your arduino libraries folder (usually found at /libraries) or import the .zip file directly using the Arduino IDE.

## Experiment sketches

* `guard-your-stuff` lock your object and track its movement when it gets stolen
* `count-visits count visits` for facility maintanence
* `environmental-sensing` measure your environment