/** 
 * @file main.ino
 * @brief Controller for a digital clock based on Arduino architecture.
 * 
 * This file contains the main source for the control of a digital clock using
 * as the base an arduino nano.
 * 
 * @version 0.0.1+202601140057UTC
 * @date 2025-10-30 15:55 UTC
 * @author CBRA64
 * @copyright MIT License
 * @details
 * - Created: 2025-10-30 15:55 UTC
 * - Last modified: 2026-01-14 00:57 UTC
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

// Pinout declarations
const uint8_t
  CHAIN_B_N_SR_CLR = 12,  // ICC N_SR_CLR pin 10  // Unclears Serial
  CHAIN_B_N_OE = 11,      // ICC N_OE pin 13      // Disables Output
  CHAIN_B_SR_CLK = 10,    // ICC SR_CLK pin 11    // Reads Serial
  CHAIN_B_R_CLK = 9,      // ICC R_CLK pin 12     // Shifts Serial
  CHAIN_B_SER = 8,        // ICC SER pin 14       // Data Serial
  
  CHAIN_A_N_SR_CLR = 7,   // ICC N_SR_CLR pin 10  // Unclears Serial
  CHAIN_A_N_OE = 6,       // ICC N_OE pin 13      // Disables Output
  CHAIN_A_SR_CLK = 5,     // ICC SR_CLK pin 11    // Reads Serial
  CHAIN_A_R_CLK = 4,      // ICC R_CLK pin 12     // Shifts Serial
  CHAIN_A_SER = 3,        // ICC SER pin 14       // Data Serial

  PIN_LED = 13,           // On-board LED pin

  PIN_DHT = 2             // Digital pin connected to the DHT sensor
;

// DHT Sensor type
#define DHTTYPE DHT11     // DHT 11

// Time control constants
unsigned long
  SECOND_MS = 1000,                         // 1s
  TOGGLING_TIME_DURATION_MS = 4000,         // 4s
  TOGGLING_TEMPERATURE_DURATION_MS = 2000,  // 2s
  RESYNC_LAPSE_MS = 1800,                   // 30min
  BLINK_INTERVAL_MS = 250                   // 0.5s
;

// LIGHT INTENSITY
uint8_t light_intensity = 200; // 0-255

// DISPLAY CHARACTERS CONVERSION
#define DISPLAY_DOT 14
#define DISPLAY_FULL 13
#define DISPLAY_EMPTY 12
#define DISPLAY_DASH 11
#define DISPLAY_C 10
#define DISPLAY_9 9
#define DISPLAY_8 8
#define DISPLAY_7 7
#define DISPLAY_6 6
#define DISPLAY_5 5
#define DISPLAY_4 4
#define DISPLAY_3 3
#define DISPLAY_2 2
#define DISPLAY_1 1
#define DISPLAY_0 0

const uint16_t DISPLAY_BITS[15] = {
  0xFC, // 0
  0x60, // 1
  0xDA, // 2
  0xF2, // 3
  0x66, // 4
  0xB6, // 5
  0xBE, // 6
  0xE0, // 7
  0xFE, // 8
  0xF6, // 9
  0x9C, // C
  0x02, // -
  0x00, // empty
  0xFF, // full
  0x01  // .
};

// BIT SEGMENT POSITION
#define BIT_SEGMENT_A 7
#define BIT_SEGMENT_B 6
#define BIT_SEGMENT_C 5
#define BIT_SEGMENT_D 4
#define BIT_SEGMENT_E 3
#define BIT_SEGMENT_F 2
#define BIT_SEGMENT_G 1
#define BIT_SEGMENT_H 0



/*
  --------------------------------------------------------------------------
  Global Variables
  --------------------------------------------------------------------------
*/  
// Time control variables
uint16_t 
  current_time_ms = 0,
  toggled_time_ms = 0,
  resynced_time_ms = 0,
  ticked_time_ms = 0,
  blinked_time_ms = 0
;

bool show_temperature = false;

int16_t year, month, day, hour, minute, second; 

float temperature = 0.0;

uint16_t lineA[6] = {DISPLAY_DASH, DISPLAY_DASH, DISPLAY_DASH, DISPLAY_DASH, DISPLAY_DASH, DISPLAY_DASH};
uint16_t lineB[6] = {DISPLAY_DASH, DISPLAY_DASH, DISPLAY_DASH, DISPLAY_DASH, DISPLAY_DASH, DISPLAY_DASH};

/*
  --------------------------------------------------------------------------
  Global Objects
  --------------------------------------------------------------------------
*/  

Chained74HC595 chainA(
  CHAIN_A_SR_CLK, CHAIN_A_SER, 
  CHAIN_A_R_CLK, CHAIN_A_N_SR_CLR, 
  CHAIN_A_N_OE
),
chainB(
  CHAIN_B_SR_CLK, CHAIN_B_SER, 
  CHAIN_B_R_CLK, CHAIN_B_N_SR_CLR, 
  CHAIN_B_N_OE
);

RTC_DS1307 rtc;

DHT dht(PIN_DHT, DHTTYPE);

/*
  --------------------------------------------------------------------------
  Functions Declarations
  --------------------------------------------------------------------------
*/  

void syncTime(void);
void tickTime(void);
bool getTemperature(void);
void updateLines(void);
void setup(void);
void loop(void);

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

  Serial.println("SYNC COMPLETE");
  Serial.println(year);
  Serial.println(month);
  Serial.println(day);
  Serial.println(hour);
  Serial.println(minute);
  Serial.println(second);

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
        //hour = 0;
        syncTime();
      }
    }
  }
  updateLines();
}

bool getTemperature(void){
  float t = dht.readTemperature();
  if (isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return false;
  }
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" °C ");
  temperature = t;
  return true;
}

void updateLines(void){
  lineA[0] = DISPLAY_BITS[year/10 % 10];
  lineA[1] = DISPLAY_BITS[year % 10] + DISPLAY_BITS[DISPLAY_DOT];

  lineA[2] = DISPLAY_BITS[month/10 % 10];
  lineA[3] = DISPLAY_BITS[month % 10] + DISPLAY_BITS[DISPLAY_DOT];

  lineA[4] = DISPLAY_BITS[(day/10) % 10];
  lineA[5] = DISPLAY_BITS[day % 10];

  if (show_temperature){
    int temp_int = temperature * 10;
    lineB[0] = DISPLAY_BITS[DISPLAY_EMPTY];

    if (temperature < 0){
      lineB[1] = DISPLAY_BITS[DISPLAY_DASH];
      temp_int = -temp_int;
    } else {
      lineB[1] = DISPLAY_BITS[DISPLAY_EMPTY];
    }
    lineB[2] = DISPLAY_BITS[(temp_int/100) % 10];
    lineB[3] = DISPLAY_BITS[(temp_int/10) % 10] + DISPLAY_BITS[DISPLAY_DOT];
    lineB[4] = DISPLAY_BITS[temp_int % 10];
    lineB[5] = DISPLAY_BITS[DISPLAY_C];
  } else{
    int temp_hour = hour%12;
    if (temp_hour == 0) temp_hour = 12;
    if (temp_hour < 10) lineB[0] = DISPLAY_BITS[DISPLAY_EMPTY];
    else lineB[0] = DISPLAY_BITS[DISPLAY_1];
    lineB[1] = DISPLAY_BITS[temp_hour % 10] + DISPLAY_BITS[DISPLAY_DOT];

    lineB[2] = DISPLAY_BITS[(minute/10) % 10] + DISPLAY_BITS[DISPLAY_DOT];
    lineB[3] = DISPLAY_BITS[minute % 10] + DISPLAY_BITS[DISPLAY_DOT];

    lineB[4] = DISPLAY_BITS[(second/10) % 10] + DISPLAY_BITS[DISPLAY_DOT];
    lineB[5] = DISPLAY_BITS[second % 10];
  }

  // SOLVED PROBLEM WITH DRIVERS
  if (lineA[0] & (1<<BIT_SEGMENT_E)){
    lineA[0] &= 0xFF ^ (1<<BIT_SEGMENT_F);
    lineA[0] += (1<<BIT_SEGMENT_F);
  }
  if (lineA[0] & (1<<BIT_SEGMENT_G)){
    lineA[0] += (1<<BIT_SEGMENT_H);
  }
  if (lineA[2] & (1<<BIT_SEGMENT_D)){
    lineA[2] += (1<<BIT_SEGMENT_G);
  }
  if (lineA[4] & (1<<BIT_SEGMENT_F)){
    lineA[4] += (1<<BIT_SEGMENT_H);
  }
  if (lineA[4] & (1<<BIT_SEGMENT_G)){
    lineB[5] += (1<<BIT_SEGMENT_H);
  }

  digitalWrite(CHAIN_A_N_OE, HIGH); // Indicate updating display
  digitalWrite(CHAIN_B_N_OE, HIGH); // Indicate updating display
  for (uint8_t i = 0; i < 6; i++){
    chainA.byteShift(lineA[5 - i]);
    chainB.byteShift(lineB[5 - i]);
  }
  analogWrite(CHAIN_A_N_OE, 255-light_intensity); // Restore LED brightness
  analogWrite(CHAIN_B_N_OE, 255-light_intensity); // Restore LED brightness


}

void setup(){
  chainA.disableOutput();
  chainB.disableOutput();
  pinMode(PIN_LED, OUTPUT);

  Serial.begin(115200);
  Serial.println("SETUP STARTING");

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) {
      digitalWrite(PIN_LED, !digitalRead(PIN_LED));
      delay(100);
    }
  }

  syncTime();
  dht.begin();
  Serial.println("SETUP COMPLETE");
  Serial.println(year);
  Serial.println(month);
  Serial.println(day);
  Serial.println(hour);
  Serial.println(minute);
  Serial.println(second);
}

void loop(){
  current_time_ms = millis();
  if (current_time_ms - resynced_time_ms >= RESYNC_LAPSE_MS){
    syncTime();
    resynced_time_ms = current_time_ms;
  }

  if (show_temperature){
    if (current_time_ms - toggled_time_ms >= TOGGLING_TEMPERATURE_DURATION_MS){
      show_temperature = false;
      toggled_time_ms = current_time_ms;
    }
  } else {
    current_time_ms = millis();
    if (current_time_ms - toggled_time_ms >= TOGGLING_TIME_DURATION_MS){
      show_temperature = getTemperature();
      toggled_time_ms = current_time_ms;
      tickTime(); 
    } else {
      if (current_time_ms - ticked_time_ms >= SECOND_MS){
        tickTime();
        ticked_time_ms = current_time_ms;
      }
    }
  }

  current_time_ms = millis();
  if (current_time_ms - blinked_time_ms >= BLINK_INTERVAL_MS){
    digitalWrite(PIN_LED, !digitalRead(PIN_LED));
    blinked_time_ms = current_time_ms;
  }
}