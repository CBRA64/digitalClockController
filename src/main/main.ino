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
  CHAIN_A_N_SR_CLR = 12,  // ICC N_SR_CLR pin 10  // Unclears Serial
  CHAIN_A_SR_CLK = 11,    // ICC SR_CLK pin 11    // Reads Serial
  CHAIN_A_R_CLK = 10,     // ICC R_CLK pin 12     // Shifts Serial
  CHAIN_A_N_OE = 9,       // ICC N_OE pin 13      // Disables Output
  CHAIN_A_SER = 8,        // ICC SER pin 14       // Data Serial
  
  CHAIN_B_N_SR_CLR = 7,   // ICC N_SR_CLR pin 10  // Unclears Serial
  CHAIN_B_SR_CLK = 6,     // ICC SR_CLK pin 11    // Reads Serial
  CHAIN_B_R_CLK = 5,      // ICC R_CLK pin 12     // Shifts Serial
  CHAIN_B_N_OE = 4,       // ICC N_OE pin 13      // Disables Output
  CHAIN_B_SER = 3,        // ICC SER pin 14       // Data Serial

  PIN_LED = 13,           // On-board LED pin

  PIN_DHT = 2             // Digital pin connected to the DHT sensor
;

// DHT Sensor type
#define DHTTYPE DHT11     // DHT 11

// Time control constants
unsigned long
  SECOND_MS = 1000,               // 1s
  TIME_DURATION_MS = 4000,        // 4s
  TEMPERATURE_DURATION_MS = 1000, // 1s
  RESYNC_LAPSE_MS = 120000        // 2h
;




/*
  --------------------------------------------------------------------------
  Global Variables
  --------------------------------------------------------------------------
*/  
// Time control variables
uint16_t 
  delay_time_ser_us = 20,
  delay_time_out = 1000/2
;

int16_t year, month, day, hour, minute, second; 

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

DHT dht(DHTPIN, DHTTYPE);

/*
  --------------------------------------------------------------------------
  Functions Declarations
  --------------------------------------------------------------------------
*/  

void getTime(void);
bool getTemperature(void);
void tickTime(void);
void setup(void);
void loop(void);

/*
  --------------------------------------------------------------------------
  Functions Definitions
  --------------------------------------------------------------------------
*/  

void getTime(void){
  DateTime now = rtc.now();

  year = now.year();
  month = now.month();
  day = now.day();
  hour = now.hour();
  minute = now.minute();
  second = now.second()-4;
}

void setup(){
  chainA.disableOutput();
  chainB.disableOutput();
  pinMode(LED, OUTPUT);

  Serial.begin(115200);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  getTime();
  dht.begin();


}

void loop(){
  // for(unsigned char i = 0; i < 10; i++){
  //   chainA.byteShift(display_bits[i]);
  //   chainA.enableOutput();
  //   digitalWrite(LED, HIGH);
  //   delay(delay_time_out);
  //   chainA.disableOutput();
  //   // digitalWrite(LED, LOW);
  //   delay(delay_time_out);

  // }
  // chainA.disableOutput();
  // chainA.byteShift(display_bits[display_empty]);
  // chainA.enableOutput();
  // delay(2000);
  // chainA.disableOutput();
  // chainA.byteShift(display_bits[display_full]);
  // chainA.enableOutput();
  delay(2000);
}