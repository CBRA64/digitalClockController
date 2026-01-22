/** 
 * @file main.ino
 * @brief Controller for a digital clock based on Arduino architecture.
 * 
 * This file contains the main source for the control of a digital clock using
 * as the base an arduino nano.
 * 
 * @version 1.0.0
 * @date 2026-01-22 13:40 UTC
 * @author CBRA64
 * @copyright MIT License
 * @details
 * - Created: 2025-10-30 15:55 UTC
 * - Last modified: 2026-01-22 13:40 UTC
 * - Repository: https://github.com/CBRA64/digitalClockController.git
*/

#include "chained_74hc595.h"
#include "RTClib.h"
#include "DHT.h"

/*
  --------------------------------------------------------------------------
  Global Constants
  --------------------------------------------------------------------------
*/  

#define DEBUG true

/*
  --------------------------------------------------------------------------
  Global Variables
  --------------------------------------------------------------------------
*/  
// Date/Time variables
int16_t year, month, day, hour, minute, second; 

// Temperature variables
float temperature = 0.0;

uint16_t lineA[6] = {DISPLAY_DASH, DISPLAY_DASH, DISPLAY_DASH, DISPLAY_DASH, DISPLAY_DASH, DISPLAY_DASH};
uint16_t lineB[6] = {DISPLAY_DASH, DISPLAY_DASH, DISPLAY_DASH, DISPLAY_DASH, DISPLAY_DASH, DISPLAY_DASH};

/*
  --------------------------------------------------------------------------
  Global Objects
  --------------------------------------------------------------------------
*/

/*
  --------------------------------------------------------------------------
  Functions Declarations
  --------------------------------------------------------------------------
*/
void syncTime(void); // Fetches time from the rtc module.
void tickTime(void); // Increases time value by a second.
bool getTemperature(void); // Fetches temperature from sensor if possible.
void solveDriverFailures(void); // Solves driver failures that were rerouted.
void updateDisplayA(void); // Updates date line from the clock.
void updateDisplayB(void); // Update time/temperature line from the clock.
void setup(void); // Setup system.
void loop(void); // Looping function.

/*
  --------------------------------------------------------------------------
  Functions Definitions
  --------------------------------------------------------------------------
*/
void syncTime(void){
  DateTime now = rtc.now();

  year = now.year();
  month = now.month();
  day = now.day();
  hour = now.hour();
  minute = now.minute();
  second = now.second();

  #if DEBUG
    Serial.println("SYNC COMPLETE");
    Serial.println(year);
    Serial.println(month);
    Serial.println(day);
    Serial.println(hour);
    Serial.println(minute);
    Serial.println(second);
  # endif
}

void tickTime(void){
  second++;
  if(second >= 60){
    second = 0;
    minute++;
    if(minute >= 60){
      minute = 0;
      hour++;
      if(hour >= 24){
        syncTime();
      }
    }
  }
}


bool getTemperature(void){
  temperature = dht.readTemperature();

  if (isnan(temperature)) {
    #if DEBUG
      Serial.println("Failed to read from DHT sensor!");
    #endif
    return false;
  }

  #if DEBUG
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println(" °C ");
  #endif

  return true;
}

void solveDriverFailures(void){
  uint16_t lineA_temp[6] = {lineA[0], lineA[1], lineA[2], lineA[3], lineA[4], lineA[5]};
  uint16_t lineB_temp[6] = {lineB[0], lineB[1], lineB[2], lineB[3], lineB[4], lineB[5]};

  // SOLVED PROBLEM WITH DRIVERS
  
  if (lineA_temp[0] & (1<<BIT_SEGMENT_E)){
    lineA[0] &= 0xFF ^ (1<<BIT_SEGMENT_F);
    lineA[0] += (1<<BIT_SEGMENT_F);
  }
  if (lineA_temp[0] & (1<<BIT_SEGMENT_G)){
    lineA[0] += (1<<BIT_SEGMENT_H);
  }
  if (lineA_temp[2] & (1<<BIT_SEGMENT_D)){
    lineA[2] += (1<<BIT_SEGMENT_G);
  }
  if (lineA_temp[4] & (1<<BIT_SEGMENT_F)){
    lineA[4] += (1<<BIT_SEGMENT_H);
  }
  if (lineA_temp[4] & (1<<BIT_SEGMENT_G)){
    lineB[5] += (1<<BIT_SEGMENT_H);
  }

}

void updateDisplayA(void);
void updateDisplayB(void);
void setup(void);
void loop(void);
