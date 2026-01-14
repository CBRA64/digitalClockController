
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

// --------------------------------------------------------------------------
// Global Constants
// --------------------------------------------------------------------------

// uint8_t
//   SR_CLK = 12,  // ICC SR_CLK pin 11  // Reads Serial
//   SER = 11,     // ICC SER pin 14     // Data Serial
//   R_CLK = 10,   // ICC R_CLK pin 12   // Shifts Serial
//   N_SR_CLR = 9, // ICC N_SR_CLR pin 10 // Unclears Seial
//   N_OE = 8      // ICC N_OE pin 13     // Diasables Output
// ;

uint8_t
  N_SR_CLR = 12,  // ICC N_SR_CLR pin 10 // Unclears Seial
  SR_CLK = 11,    // ICC SR_CLK pin 11  // Reads Serial
  R_CLK = 10,     // ICC R_CLK pin 12   // Shifts Serial
  N_OE = 9,       // ICC N_OE pin 13     // Diasables Output
  SER = 8,        // ICC SER pin 14     // Data Serial
  LED = 13
;


uint16_t 
  delay_time_ser_us = 20,
  delay_time_out = 1000/2
;

Chained74HC595 chainA(SR_CLK, SER, R_CLK, N_SR_CLR, N_OE);

void setup(){
  chainA.disableOutput();
  pinMode(LED, OUTPUT);
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
    chainA.disableOutput();
  chainA.byteShift(display_bits[display_empty]);
  chainA.enableOutput();
  delay(2000);
    chainA.disableOutput();
  chainA.byteShift(display_bits[display_full]);
  chainA.enableOutput();
  delay(2000);
}