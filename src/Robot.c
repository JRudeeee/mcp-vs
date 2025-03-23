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

  cli(); // Disable interrupts globally for initialistion

  /*
  Set Timer/Counter1 Register TCCR1A = 0, as we do not need anything here
  Set Waveform Generation Mode Bits for CTC ICR1 - WGM12 - WGM13 in Timer/Counter1 Register TCCR1B
  Set Clock Select CS12:0 for desired prescaler in TCCR1B (calculate this)
  Set Timer/Counter1 TCNT1 = 0 to start counter at Zero.
  Set Input Capture Register ICR1 = some value, calculated with the prescaler for desired frequency.
  Set Timer/Counter1 Interrupt Mask TIMSK1 bit 5 ICIE1 to enable Input Capture Interrupt.
  */

  /*
  Arduino Mega2560 operates with a 16MHz clock

  fIC1 = 16000000/2*N*(1+ICR1)
  Where, N = prescaler

  Interrupt is captured twice per period of fIC1

  500Hz = 16000000/2*8*(1+1999)
  which will give us an interrupt frequency of 1kHz, or a resolution of 1ms
  */
  DDRE |= (0<<PE4)|(0<<PE5); // set PE4-5 to input mode 
  PORTE = 0;
  PORTE |= (1<<PE4)|(1<<PE5); // Enable pullup resistors on PE4-5

  TCCR1A = 0;
  TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1 << CS11); // Change the CS1x to the desired prescaler. Currently set to precaler of 8.
  TCNT1 = 0;
  ICR1 = 1999; // Set to 1999 to give us a period of 500Hz, and an intterupt every 1ms.
  TIMSK1 = (1 << ICIE1);

  EICRB |= (1<<ISC41)|(1<<ISC51)|(0<<ISC40)|(0<<ISC50);
  EIMSK |= (1<<INT4)|(1<<INT5);

  sei();         // Enable interrupts globally post initialisation
  serial0_init();
  milliseconds_init();

  _delay_ms(50); // Delay to ensure any initialisations complete

  /*-----------------------------------------------------------------------*/
  /*                         End Initialisation Phase                      */
  /*-----------------------------------------------------------------------*/

  //stopwatchStart = true;

  char timerString[60] = {};
  uint32_t minutes, seconds, milli = 0;

  while (1)
  {
    
    if (!stopwatchStart && stopwatchReset)
    {
      stopwatchCounter = 0;
      stopwatchReset = false;
    }

    while (stopwatchStart == true)
    {
      milli = stopwatchCounter;

      minutes = milli / 60000;
      milli = milli - 60000 * minutes;
      seconds = milli / 1000;
      milli = milli - 1000 * seconds;

      sprintf(timerString, "Stopwatch = %lu Timer = %.2lu:%.2lu:%.3lu\n", stopwatchCounter, minutes, seconds, milli);
      serial0_print_string(timerString);
      _delay_ms(50);
    }
  }

} // end main

ISR(TIMER1_CAPT_vect)
{
  if (stopwatchStart)
  {
    stopwatchCounter++;
  }
} // end TIMER1_CAPT_vect

ISR(INT4_vect)
{
  uint32_t currentTime = milliseconds_now();
  uint8_t buttonRead = PINE & (1<<PE4);
  uint8_t buttonState =1;

  if ((currentTime - previousTime) > DEBOUNCEDELAY)
  {
    if(buttonRead != buttonState)
    {
      buttonState = buttonRead;

      if (buttonState == 0)
      {
        stopwatchStart = !stopwatchStart;
      }
    }
  }
  previousTime = currentTime;
}

ISR(INT5_vect)
{
  uint32_t currentTime = milliseconds_now();
  uint8_t buttonRead = PINE & (1<<PE5);
  uint8_t buttonState =1;

  if ((currentTime - previousTime) > DEBOUNCEDELAY)
  {
    if(buttonRead != buttonState)
    {
      buttonState = buttonRead;

      if ((buttonState == 0) && !stopwatchStart)
      {
        stopwatchReset = !stopwatchReset;
      }
    }
  }
  previousTime = currentTime;
}