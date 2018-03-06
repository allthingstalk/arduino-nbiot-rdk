/*    _   _ _ _____ _    _              _____     _ _     ___ ___  _  __
 *   /_\ | | |_   _| |_ (_)_ _  __ _ __|_   _|_ _| | |__ / __|   \| |/ /
 *  / _ \| | | | | | ' \| | ' \/ _` (_-< | |/ _` | | / / \__ \ |) | ' <
 * /_/ \_\_|_| |_| |_||_|_|_||_\__, /__/ |_|\__,_|_|_\_\ |___/___/|_|\_\
 *                             |___/
 *
 * Copyright 2018 AllThingsTalk
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
/****
 * Get notified when someone is tampering with one of your favorite paintings.
 * This experiment shows how a NB-IoT device can be used to detect unexpected
 * movement of an object and send out notifications to its owner.
 * Furthermore it shows how you can track the object using its GEO location.
 */
 
// Selected your preferred method of sending data
#define CBOR
//#define BINARY

#include "ATT_NBIOT.h"
#include <ATT_GPS.h>

#include <Wire.h>
#include <MMA7660.h>

// Mbili support
#define DEBUG_STREAM Serial
#define MODEM_STREAM Serial1
#define MODEM_ON_OFF_PIN 23

#define ACCEL_THRESHOLD 12  // Threshold for accelerometer movement
#define DISTANCE 30.0       // Minimal distance between two fixes to keep checking gps
#define FIX_DELAY 60000     // Delay (ms) between checking gps coordinates

ATT_NBIOT device;

#ifdef CBOR
  #include <CborBuilder.h>
  CborBuilder payload(device);
#endif

#ifdef BINARY
  #include <PayloadBuilder.h>
  PayloadBuilder payload(device);
#endif

ATT_GPS gps(20,21);  // Reading GPS values from debugSerial connection with GPS

MMA7660 accelerometer;
bool moving = false;  // Set given data from both accelerometer and gps

// variables for the coordinates (GPS)
float prevLatitude;
float prevLongitude;

int8_t prevX,prevY,prevZ;  // Keeps track of the previous accelerometer data

void setup() 
{
  accelerometer.init();  // Accelerometer is always running so we can check if movement started

  delay(100);

  DEBUG_STREAM.begin(57600);
  MODEM_STREAM.begin(9600);
  
  DEBUG_STREAM.println("Initializing and connecting... ");

  device.init(MODEM_STREAM, DEBUG_STREAM, MODEM_ON_OFF_PIN);
  
  if(device.connect())
    DEBUG_STREAM.println("Connected!");
  else
  {
    DEBUG_STREAM.println("Connection failed!");
    while(true) {}  // No connection. No need to continue the program
  }

  DEBUG_STREAM.println();
  DEBUG_STREAM.println("-- Guard your stuff NB-IoT experiment --");
  DEBUG_STREAM.println();

  DEBUG_STREAM.println("Getting initial GPS fix");
  if(!gps.readCoordinates(30))  // Try 30 times to get initial GPS fix
    DEBUG_STREAM.println("No fix found. Sending default");
  else
    DEBUG_STREAM.println("Sending initial fix");
  sendCoordinatesAndMotion(false);
  
  accelerometer.getXYZ(&prevX, &prevY, &prevZ);  // Get initial accelerometer state

  DEBUG_STREAM.println("Ready to guard your stuff");
}

unsigned long sendNextAt = 0;  // Keep track of time
void loop()
{
  if(!moving)  // If not moving, check accelerometer
  {
    moving = isAccelerating();
    delay(500);
  }

  if(moving && sendNextAt < millis())  // We waited long enough to check new fix
  {
    // Start looking for coordinates
    DEBUG_STREAM.println("Movement detected. Searching GPS fix");
    gps.readCoordinates();
    
    if(gps.calcDistance(prevLatitude, prevLongitude) <= DISTANCE)  // We did not move much. Back to checking accelerometer for movement
    {
      DEBUG_STREAM.print("Less than ");
      DEBUG_STREAM.print(DISTANCE);
      DEBUG_STREAM.println("m movement in last interval");
      
      sendCoordinatesAndMotion(false);  // Send fix and motion false
      
      // Reset parameters
      moving = false;
      gps.reset();
    }
    else  // Update and send new coordinates
    {
      prevLatitude = gps.latitude;
      prevLongitude = gps.longitude;
      sendCoordinatesAndMotion(true);  // Send fix and motion true
    }
    sendNextAt = millis() + FIX_DELAY;  // Update time
  }
}

// Check if acceleration is detected
bool isAccelerating()
{
  int8_t x,y,z;
  accelerometer.getXYZ(&x, &y, &z);
  bool result = (abs(prevX - x) + abs(prevY - y) + abs(prevZ - z)) > ACCEL_THRESHOLD;
  
  if(result == true)
  {
    prevX = x;
    prevY = y;
    prevZ = z;
  }

  return result; 
}

// Send GPS coordinates and motion to the AllThingsTalk cloud
void sendCoordinatesAndMotion(bool val)
{
  payload.reset();
  
  #ifdef CBOR  // Send data using Cbor
  payload.map(2);
  payload.addBoolean(val, "motion");
  payload.addGPS(gps.latitude, gps.longitude, gps.altitude, "gps");
  #endif
  
  #ifdef BINARY  // Send data using a Binary payload and our ABCL language
  payload.addBoolean(val);
  payload.addGPS(gps.latitude, gps.longitude, gps.altitude);
  #endif
    
  payload.send();
  
  DEBUG_STREAM.print("lng: ");
  DEBUG_STREAM.print(gps.longitude, 4);
  DEBUG_STREAM.print(", lat: ");
  DEBUG_STREAM.print(gps.latitude, 4);
  DEBUG_STREAM.print(", alt: ");
  DEBUG_STREAM.print(gps.altitude);
  DEBUG_STREAM.print(", time: ");
  DEBUG_STREAM.println(gps.timestamp);
}