# arduino-nbiot-rdk

This library contains examples for the [NB-IoT Rapid Development Kit]().

> It is to be used in conjunction with the [arduino-nbiot-sdk](https://github.com/allthingstalk/arduino-nbiot-sdk) which contains all basic functionality.

External libraries are included in this repository for use of all needed sensors

* [AirQuality2](https://github.com/MikeHg/AirQualitySensor/tree/d6cadaf21c6beae99fdd65bb037424ce6f855db1)
* [MMA7660](https://github.com/Seeed-Studio/Accelerometer_MMA7660) (Accelerometer)
* [TPH2](http://support.sodaq.com/sodaq-one/tph-v2/) Adafruit_Sensor and Adafruit_BME280
* GPS

## Installation

Download the source code and copy the content of the zip file to your arduino libraries folder (usually found at /libraries) or import the .zip file directly using the Arduino IDE.

## Experiment sketches

> Make sure your device credentials are set, either globally in the **keys.h** file of the _arduino-nbiot-sdk_ or locally in your sketch.

* `ATT_NBIOT device("your_device_id", "your_device_token");` will use the provided local credentials.
* `ATT_NBIOT device;` will use the global credentials from the **keys.h** file

Three experiments are provided

* `guard-your-stuff` get notified and track a valuable object when it moves
* `count-visits` count visits for better facility maintenance
* `environmental-sensing` measure your surrounding environment

> By default, the experiments will use Cbor to send data. You can toggle to a [binary payload]() (and decoding file at the receiving end) by (un)commenting your preferred method.

```
// Select your preferred method of sending data
#define CBOR
//#define BINARY
```