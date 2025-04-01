// Example ATmega2560 Project
// File: ATmega2560Project.c
// An example file for second year mechatronics project

// include this .c file's header file
#include "Robot.h"
// static function prototypes, functions only called in this file
uint16_t PWMOFFSET = 989; // Offset for the PWM Duty Cycle, this will allow for a Pulse Width of approximately 1-2ms when offset is applied to the value of ADC Joystick inputs.
int main(void)
{


  /*-----------------------------------------------------------------------*/
  /*                           Initialisation Phase                        */
  /*-----------------------------------------------------------------------*/

  /*
Arduino Mega2560 operates with a 16MHz clock

fIC1 = 16000000/2*N*TOP
Where, N = prescaler



50Hz = 16000000/2*8*20000

*/

  /*
  Set Timer/Counter1 Register TCCR1A = 0, as we do not need anything here
  Set Waveform Generation Mode Bits for PWM, Phase and Freq Correct ICR1 - WGM10 - WGM13 in Timer/Counter1 Register TCCR1B
  Set Clock Select CS11:0 for prescaler of 8 in TCCR1B
  Set Waveform Generation Mode Bits for PWM, Phase and Freq Correct ICR3 - WGM30 - WGM33 in Timer/Counter1 Register TCCR3B
  Set Clock Select CS31:0 for prescaler of 8 in TCCR3B
  Set Timer/Counter1 TCNT1 = 0 to start counter at Zero
  Set Input Capture Register ICR1 = 19999
  Set Timer/Counter1 Interrupt Mask TIMSK1 bit 5 ICIE1 to enable Input Capture Interrupt
  */

  cli();                                // Disable interrupts globally for initialistion

  DDRB |= (1 << PB5);                   // Set PB5 to output for OC1A PWM
  DDRE |= (1 << PE3);                   // Set PE3 to output for OC3A PWM

  TCCR1A |= (1 << COM1A1);              // Configure non-inverting PWM for OC1A in TCCR1A
  TCCR1B |= (1 << WGM13) | (1 << CS11); // Set T/C1 to mode 8 PWM, Phase and Freq Correct ICR. Set prescaler to 8.
  ICR1 = 20000;                         // Set to 20000 to give us a base freq of 50Hz
  OCR1A = 1500;                         // Set OCR1A to 1500 to establish PWM Duty Cycle of 7.5%, resulting in a Pulse Width of 1.5ms. This should set Servo to 0 degrees.

  TCCR3A |= (1 << COM3A1);              // Configure non-inverting PWM for OC3A in TCCR3A
  TCCR3B |= (1 << WGM33) | (1 << CS31); // Set T/C3 to mode 8 PWM, Phase and Freq Correct ICR. Set prescaler to 8.
  ICR3 = 20000;                         // Set to 20000 to give us a base freq of 50Hz
  OCR3A = 1500;                         // Set OCR3A to 1500 to establish PWM Duty Cycle of 7.5%, resulting in a Pulse Width of 1.5ms. This should set Servo to 0 degrees.

  sei();                                // Enable interrupts globally post initialisation

  serial0_init();                       // Initialise Serial 0
  adc_init();                           // Initialise ADC
  _delay_ms(50);                        // Delay to ensure any initialisations complete

  /*-----------------------------------------------------------------------*/
  /*                         End Initialisation Phase                      */
  /*-----------------------------------------------------------------------*/

  // Main Vars
  uint16_t joyXPos, joyYPos = 0; // 16 bit uints to store 10bit ADC state of Joystick Axis
  // Core Loop
  while (1)
  {
    joyXPos = adc_read(0); // Read the state of the X Axis
    OCR1A = joyXPos + PWMOFFSET; // Write the position of the X Axis + Offset to the T/C1 Output Compare Register 1A to control the duty cycle of PWM Output.
    joyYPos = adc_read(1); // Read the state of the Y Axis
    OCR3A = joyYPos + PWMOFFSET; // Write the position of the Y Axis + Offset to the T/C3 Output Compare Register 3A to control the duty cycle of PWM Output.
  }
} // end main
