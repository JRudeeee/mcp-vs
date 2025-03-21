// Example ATmega2560 Project
// File: ATmega2560Project.c
// An example file for second year mechatronics project

// include this .c file's header file
#include "Robot.h"
#include "../lib/adc/adc.h"
#include "../lib/timer/milliseconds.h"
// static function prototypes, functions only called in this file

// GLOBAL VARS
uint8_t DEBOUNCEDELAY = 50; // Debounce delay, increase if flickering
uint16_t adcVal = 0;
char voltageString[60] = {};
volatile uint8_t bounceCount = 0;
volatile uint16_t voltageRead = 0;
volatile bool lastButtonState = false;

int main(void)
{
  DDRC = 0xFF;         // put PORTC into output mode
  DDRE = (0 << PE4);   // put PORTE4 into input mode
  PORTE = 0;           // set output of PORTE low
  PORTE |= (1 << PE4); // enable PORTE4 internal pullup resistor

  adc_init();     // initialise ADC
  serial0_init(); // Initialise serial
  _delay_ms(20);  // wait for initialisation to finalise

  milliseconds_init();

  cli();
  EICRB |= (0 << ISC40) | (1 << ISC41);
  sei();
  EIMSK |= (1 << INT4);

  while (1) // main loop
  {
    if (voltageRead > 0)
    {

      sprintf(voltageString, "%u\n", voltageRead);
      serial0_print_string(voltageString);
    }
    else
    {
      voltageRead = 1;
      sprintf(voltageString, "%u\n", voltageRead);
      serial0_print_string(voltageString);
    }
    _delay_ms(200);
  }

  return (1);

} // end main

ISR(INT4_vect)
{
  bool buttonState = PE4;
  bool readComplete = false;
  static uint32_t previousTime = 0;

  while (readComplete == false)
  {
    if (buttonState != lastButtonState)
    {
      previousTime = milliseconds_now();
    }
  }
  if ((milliseconds_now() - previousTime) > DEBOUNCEDELAY)
  {
    lastButtonState = buttonState;
    readComplete = true;
  }
  if (buttonState == true)
  {
    voltageRead = adc_read(0);
  }
}