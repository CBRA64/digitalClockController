/** 
 * @file chained_74hc595.h
 * @brief Control for daisy-chained array of 74HC595.
 * 
 * This file contains the script used to control send data to a daisy-chained
 * array of 74HC595. 
 * Based on the datasheet information from: 
 * https://www.ti.com/lit/ds/symlink/sn74hc595.pdf
 * 
 * @version 0.0.1+202510301555UTC
 * @date 2025-10-30 15:55 UTC
 * @author CatBookshelf
 * @copyright MIT License
 * @details
 * - Created: 2025-10-30 13:11 UTC
 * - Last modified: 2025-10-30 15:55 UTC
 * - Repository: https://github.com/CatBookshelf/digitalClockController.git
*/

#ifndef CHAINED_74HC595_H
#define CHAINED_74HC595_H

// --------------------------------------------------------------------------
// Global Constants
// --------------------------------------------------------------------------

const uint16_t CYCLES_MICROSECONDS = 16;
const uint16_t CYCLES_TRANSITION_DELAY = 10;

// --------------------------------------------------------------------------
// Classes
// --------------------------------------------------------------------------

/**
 * @class Chained74HC595
 * @brief Represents a chain in series of 74HC595.
 *
 * The Chained74HC595 class provides methods to send data, enable and disable
 * outputs of the whole chain at the same time. The chain can be 1 IC long.
 */
class Chained74HC595{
  private:
    uint8_t
      SR_CLK,   // ICC SR_CLK pin 11  // Reads Serial
      SER,      // ICC SER pin 14     // Data Serial
      R_CLK,    // ICC R_CLK pin 12   // Shifts Serial
      N_SR_CLR, // ICC N_SR_CLR pin 10 // Unclears Serial
      N_OE      // ICC N_OE pin 13     // Disables Output
    ;

    void transitionDelay(uint16_t cycles);

  public:
    Chained74HC595(
      uint8_t SR_CLK, uint8_t SER, uint8_t R_CLK,
      uint8_t N_SR_CLR, uint8_t N_OE
    );

    void byteShift(uint8_t data, uint16_t delay_time_ser_us);
    void disableOutput(void);
    void enableOutput(void);
};

Chained74HC595::Chained74HC595(
  uint8_t SR_CLK, uint8_t SER, uint8_t R_CLK,
  uint8_t N_SR_CLR, uint8_t N_OE
){
  // Define the pins to use for each instance.
  this->SR_CLK = SR_CLK;
  this->SER = SER;
  this->R_CLK = R_CLK;
  this->N_SR_CLR = N_SR_CLR;
  this->N_OE = N_OE;

  // Define pinMode for the IC pins.
  pinMode(this->SR_CLK, OUTPUT);
  pinMode(this->SER, OUTPUT);
  pinMode(this->R_CLK, OUTPUT);
  pinMode(this->N_SR_CLR, OUTPUT);
  pinMode(this->N_OE, OUTPUT);

  // Define start pin state and clears outputs on the IC.
  digitalWrite(this->SR_CLK, LOW);
  digitalWrite(this->SER, LOW);
  digitalWrite(this->R_CLK, LOW);
  digitalWrite(this->N_SR_CLR, LOW);
  digitalWrite(this->N_OE, HIGH);
  
  transitionDelay(CYCLES_MICROSECONDS);
  digitalWrite(this->N_SR_CLR, HIGH);
  
  transitionDelay(CYCLES_MICROSECONDS);
}


void Chained74HC595::transitionDelay(uint16_t cycles){
  for (uint16_t i = 0; i < cycles; i++){
    __asm__("nop");
  }
}

void Chained74HC595::byteShift(uint8_t data, uint16_t delay_time_ser_us){
  for (uint8_t bit = 0; bit < 8; bit++){
    // delayMicroseconds(delay_time_ser_us);

    // Write bit data.
    digitalWrite(this->SER, data & (1<<bit));
    this->transitionDelay(CYCLES_TRANSITION_DELAY);

    // Rising edge of Serial clock.
    digitalWrite(this->SR_CLK, HIGH);
    this->transitionDelay(
      CYCLES_TRANSITION_DELAY +
      CYCLES_MICROSECONDS * delay_time_ser_us
    );

    // Falling edge of Serial and Rising of Register clocks. 
    digitalWrite(this->SR_CLK, LOW);
    digitalWrite(this->R_CLK, HIGH);
    this->transitionDelay(
      CYCLES_TRANSITION_DELAY +
      CYCLES_MICROSECONDS * delay_time_ser_us
    );
    digitalWrite(this->R_CLK, LOW);
  }
}

void Chained74HC595::disableOutput(void){
  digitalWrite(this->N_OE, HIGH);
}

void Chained74HC595::enableOutput(void){
  digitalWrite(this->N_OE, LOW);
}

// --------------------------------------------------------------------------


#endif