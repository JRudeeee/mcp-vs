// Example ATmega2560 Project
// File: ATmega2560Project.c
// An example file for second year mechatronics project

// include this .c file's header file
#include "Robot.h"
// static function prototypes, functions only called in this file

// GLOBAL VARS
uint8_t DEBOUNCEDELAY = 200; // Debounce delay, increase if flickering
volatile uint32_t previousTime = 0;
volatile uint32_t stopwatchCounter = 0; // used to count every 0.1ms
volatile bool stopwatchStart = false;   // used to start and stop stopwatch, initially stopped
volatile bool stopwatchReset = false;   // used to reset stopwatch, initially not flagged

int main(void)
{
  /*-----------------------------------------------------------------------*/
  /*                           Initialisation Phase                        */
  /*-----------------------------------------------------------------------*/

  /*
Arduino Mega2560 operates with a 16MHz clock

fIC1 = 16000000/2*N*(1+ICR1)
Where, N = prescaler

Interrupt is captured twice per period of fIC1

500Hz = 16000000/2*8*(1+1999)
which will give us an interrupt frequency of 1kHz, or a resolution of 1ms
*/

  /*
  Configure Data Direction Register E for input on PE4-5
  Ensure PORTE has 0 written to whole register
  Configure PORTE to enable pullup resistors on PE4-5, write 1 to locations
  Configure External Interrupt Control Register B EIRCB for falling edge trigger mode on ISC4 and ISC5 (ISC41 - 1 | ISC40 - 0 | ISC51 - 1 | ISC50 - 0)
  Enable INT4 and INT5 in External Interrupt Mask EIMSK
  Set Timer/Counter1 Register TCCR1A = 0, as we do not need anything here
  Set Waveform Generation Mode Bits for CTC ICR1 - WGM12 - WGM13 in Timer/Counter1 Register TCCR1B
  Set Clock Select CS12:0 for desired prescaler in TCCR1B (calculate this)
  Set Timer/Counter1 TCNT1 = 0 to start counter at Zero
  Set Input Capture Register ICR1 = some value, calculated with the prescaler for desired frequency
  Set Timer/Counter1 Interrupt Mask TIMSK1 bit 5 ICIE1 to enable Input Capture Interrupt
  */

  cli(); // Disable interrupts globally for initialistion

  DDRE |= (0 << PE4) | (0 << PE5);                                    // set PE4-5 to input mode
  PORTE = 0;                                                          // Ensure no data in register
  PORTE |= (1 << PE4) | (1 << PE5);                                   // Enable pullup resistors on PE4-5
  EICRB |= (1 << ISC41) | (1 << ISC51) | (0 << ISC40) | (0 << ISC50); // Set INT4-5 edge mode to falling edge
  EIMSK |= (1 << INT4) | (1 << INT5);                                 // Enable INT4-5

  TCCR1A = 0;                                          // Disable all features in TCCR1A
  TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1 << CS11); // Set T/C1 to mode 12 CTC ICR. Set prescaler to 8.
  TCNT1 = 0;                                           // Start Timer/Counter 1 at zero
  ICR1 = 1999;                                         // Set to 1999 to give us a period of 500Hz, and an intterupt every 1ms
  TIMSK1 = (1 << ICIE1);                               // Enable Input Capture Interrupt for ICR1.

  sei();               // Enable interrupts globally post initialisation
  serial0_init();      // Ininitalise serial0 for terminal output
  milliseconds_init(); // Inintialise milliseconds timer (this functions much the same as this initialisation, but will not be stopped)

  _delay_ms(50); // Delay to ensure any initialisations complete

  /*-----------------------------------------------------------------------*/
  /*                         End Initialisation Phase                      */
  /*-----------------------------------------------------------------------*/

  // Main Vars
  char timerString[60] = {};            // Character array for building timer output
  uint32_t minutes, seconds, milli = 0; // Used for conversion of stopwatchCounter to time components

  // Core Loop
  while (1)
  {
    // If the Stopwatch is not running, and the reset button has been pressed, reset the counter and remove reset flag.
    if (!stopwatchStart && stopwatchReset)
    {
      stopwatchCounter = 0;   // Set counter back to zero
      stopwatchReset = false; // Remove Reset Flag
    }

    // If the stopwatch is started, conduct conversions to and print formatted string to serial0 monitor. MM:SS.mSS
    while (stopwatchStart == true)
    {
      milli = stopwatchCounter;                                             // store current counter value into local variable
      minutes = milli / 60000;                                              // 1 minute = 60,000ms, find total minutes elapsed
      milli = milli - 60000 * minutes;                                      // Remove minutes counted
      seconds = milli / 1000;                                               // 1 second = 1,000ms, find seconds elapsed from remainder
      milli = milli - 1000 * seconds;                                       // Remove seconds counted
      sprintf(timerString, "%.2lu:%.2lu.%.3lu\n", minutes, seconds, milli); // Build output string MM:SS.mSS
      serial0_print_string(timerString);                                    // Print timer to serial
      _delay_ms(100);                                                       // Delay to make serial monitor more readable
    }
  }

} // end main

// Interrupt opperates every 1ms, if stopwatchStart is flagged, counter is increased.
ISR(TIMER1_CAPT_vect)
{
  if (stopwatchStart)
  {
    stopwatchCounter++;
  }
} // end TIMER1_CAPT_vect

/*
Interrupt opperates every time a logic zero is detected on PE4 (Arduino Pin D2)
due to a button press to invert the state of stopwatchStart flag
A debounce protocol filters signal noise
*/
ISR(INT4_vect)
{
  // Local Vars
  uint32_t currentTime = milliseconds_now(); // Get time since initialisation
  uint8_t buttonRead = PINE & (1 << PE4);    // Get logic state of PE4
  uint8_t buttonState = 1;                   // Comparison state, set to 1 to enter loop as we expect a Zero or 16 on PE4

  // If the debounce delay has passed since last button press, button is considdered stable
  if ((currentTime - previousTime) > DEBOUNCEDELAY)
  {
    // Ensure button state has changed since last detection for additional filtering
    if (buttonRead != buttonState)
    {
      buttonState = buttonRead; // Update the known state of the button to the detected input
      // If the state of the button is a logic zero, invert the stopwatchStart flag.
      if (buttonState == 0)
      {
        stopwatchStart = !stopwatchStart;
      }
    }
  }
  previousTime = currentTime; // Update the previous input detection time.
}

/*
Interrupt opperates every time a logic zero is detected on PE5 (Arduino Pin D3)
due to a button press to invert the state of stopwatchReset flag
A debounce protocol filters signal noise
*/
ISR(INT5_vect)
{
  uint32_t currentTime = milliseconds_now(); // Get time since initialisation
  uint8_t buttonRead = PINE & (1 << PE5);    // Get logic state of PE5
  uint8_t buttonState = 1;                   // Comparison state, set to 1 to enter loop as we expect a Zero or 32 on PE5

  if ((currentTime - previousTime) > DEBOUNCEDELAY)
  {
    // If the debounce delay has passed since last button press, button is considdered stable
    if (buttonRead != buttonState)
    {
      buttonState = buttonRead; // Update the known state of the button to the detected input
      // If the state of the button is a logic zero AND the stopwatch isn't running, invert the stopwatchReset flag.
      if ((buttonState == 0) && !stopwatchStart)
      {
        stopwatchReset = !stopwatchReset;
      }
    }
  }
  previousTime = currentTime; // Update the previous input detection time.
}