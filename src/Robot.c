// Example ATmega2560 Project
// File: ATmega2560Project.c
// An example file for second year mechatronics project

// include this .c file's header file
#include "Robot.h"
#include "../lib/adc/adc.h"
#include "../lib/timer/milliseconds.h"
// static function prototypes, functions only called in this file

// GLOBAL VARS

uint16_t adcVal = 0;
char voltageString[60] = {};
volatile uint16_t voltageRead = 0;
uint16_t distance = 0;

int main(void)
{
  DDRA = 0xFF;         // put PORTA into output mode
  
  adc_init();     // initialise ADC
  serial0_init(); // Initialise serial
  _delay_ms(20);  // wait for initialisation to finalise

  while (1) // main loop
  {
    voltageRead = (int) (adc_read(0) / 5000 * 1024);
    
    if (voltageRead >= 1800)
    {
      PORTA = 1;
    }else
    {
      PORTA = 0;
    }
    _delay_ms(200);
  }

  return (1);

} // end main

