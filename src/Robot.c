//Example ATmega2560 Project
//File: ATmega2560Project.c
//An example file for second year mechatronics project

//include this .c file's header file
#include "Robot.h"
#include "../lib/adc/adc.h"
//static function prototypes, functions only called in this file

int main(void)
{
    DDRC = 0xFF; //put PORTC into output mode
    DDRB = (0<<1); //put PORTB1 into input mode
    PORTB = 0; //set output of PORTB low
    PORTB |= (1<<PB1); //enable PORTB1 internal pullup resistor

    adc_init(); //initialise ADC
    _delay_ms(20); //wait for initialisation to finalise  

    uint16_t adcVal = 0;
    uint8_t adcConverted = 0;
  
    while(1) //mail loop
    {
      PORTC = 0; //ensure all LED's are off.

      // Read the state of the joystick axis as selected by button
      if(PINB & (1<<PB1)) //if joystick button is not pressed
      {
        adcVal = adc_read(0); //read the state of the joystick x axis
      } else //if joystick button is pressed
      {
        adcVal = adc_read(1); //read the state of the joystick y axis
      }

      //Convert joystick input from 10bit to 8 bit
      adcConverted = (adcVal>>2); //2 LSB are shifted out of adcVal, leaving 8 bits to store in adcConverted. Essentially dividing adcVal by 4.

      //Illuminate LEDs depending on input
      if(adcConverted < 97)
      {
        PORTC |= (1<<PC3);
        if(adcConverted < 65)
        {
          PORTC |= (1<<PC2);
          if(adcConverted < 31)
          {
            PORTC |= (1<<PC1);
            if(adcConverted == 0)
            {
              PORTC |= (1<<PC0);
            }
          }
        }
      } else if(adcConverted > 159)
      {
        PORTC |= (1<<PC4);
        if(adcConverted > 191)
        {
          PORTC |= (1<<PC5);
          if(adcConverted < 233)
          {
            PORTC |= (1<<PC6);
            if(adcConverted == 255)
            {
              PORTC |= (1<<PC7);
            }
          }
        }
      }

    }
  
  
  // Lab 1
  // DDRC = 0xFF;//put PORTC into output mode
  // PORTC = 0; 
  // while(1)//main loop
  // {
  //   for(int i = 0; i<8: i++){
  //     PORTC |= (1<<i);
  //     if(i > 0){
  //       PORTC &= ~(1<<i);
  //     }
  //   }
  //   for(int i = 7; i>=0: i--){
  //     PORTC |= (1<<i);
  //     if(i < 7){
  //       PORTC &= ~(1<<i);
  //     }
  //   }
    


    // _delay_ms(500);     //500 millisecond delay
    // PORTA |= (1<<PA2);  // note here PA3 is just an alias for the number 3
                           // this line is equivalent to PORTA = PORTA | 0b00001000   which writes a HIGH to pin 3 of PORTA
    // _delay_ms(500); 
    // PORTA &= ~(1<<PA2); // this line is equivalent to PORTA = PORTA & (0b11110111)  which writes a HIGH to pin 3 of PORTA
  //}
  return(1);
}//end main 