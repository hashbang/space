// main.c
// 
// A simple blinky program for ATtiny85
// Connect red LED at pin 2 (PB3)
//
// electronut.in

#include <avr/io.h>
#include <avr/interrupt.h>
//#include <avr/pgmspace.h>

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define F_CPU  8000000UL

/***** Configure IO *****/
#define CONFIG_AS_INPUT(bit)    {bit ## _DDR &= ~(1 << bit);}
#define CONFIG_AS_OUTPUT(bit)   {bit ## _DDR |= (1 << bit);}

// This globally disables pullups and tristates the pin in the argument. Saves power but use carefully. This also makes the macros header less generic
//#define CONFIG_AS_DISABLED(bit) {MCUCR |= (1 << PUD); bit ## _DDR &= ~(1 << pin); bit ## _PORT |= (1 << pin);}

#define PULLUP_ON(bit)          {bit ## _PORT |= (1 << bit);}
#define PULLUP_OFF(bit)         {bit ## _PORT &= ~(1 << bit);}

/***** Manipulate Outputs *****/
#define SET_HIGH(bit)           {bit ## _PORT |= (1 << bit);}
#define SET_LOW(bit)            {bit ## _PORT &= ~(1 << bit);}
#define SET_TOGGLE(bit)         {bit ## _PORT ^= (1 << bit);}

/***** Test Inputs *****/
#define IS_HIGH(bit)            (bit ## _PIN & (1 << bit))
#define IS_LOW(bit)             (! (bit ## _PIN & (1 << bit)))


#define GRN_LED       PB0
#define GRN_LED_PORT  PORTB
#define GRN_LED_DDR   DDRB
#define GRN_LED_PIN   PINB

#define RED_LED       PB1
#define RED_LED_PORT  PORTB
#define RED_LED_DDR   DDRB
#define RED_LED_PIN   PINB

#define BLU_LED       PB3
#define BLU_LED_PORT  PORTB
#define BLU_LED_DDR   DDRB
#define BLU_LED_PIN   PINB

#define WHT_LED       PB4
#define WHT_LED_PORT  PORTB
#define WHT_LED_DDR   DDRB
#define WHT_LED_PIN   PINB

#define IR_RCVR       PB2
#define IR_RCVR_PORT  PORTB
#define IR_RCVR_DDR   DDRB
#define IR_RCVR_PIN   PINB



int main (void)
{
  // Globally disable interrupts
  cli();

  // Output Pin Setup
  CONFIG_AS_OUTPUT(GRN_LED);
  SET_LOW(GRN_LED);
  CONFIG_AS_OUTPUT(RED_LED);
  SET_LOW(RED_LED);
  CONFIG_AS_OUTPUT(BLU_LED);
  SET_LOW(BLU_LED);
  CONFIG_AS_OUTPUT(WHT_LED);
  SET_LOW(WHT_LED);

  // Input Pin Setup
  CONFIG_AS_INPUT(IR_RCVR);
  PULLUP_OFF(IR_RCVR);

  // General Interrupt Flag Register bits be cleared by writing one to them
  GIFR = 0xFF;
  
  while (1)
  {
    
    // flash# 1:
    // set PB3 high
    PORTB = 0b00001000; 
    _delay_ms(20);
    // set PB3 low
    PORTB = 0b00000000;
    _delay_ms(20);

    // flash# 2:
    // set PB3 high
    PORTB = 0b00001000; 
    _delay_ms(200);
    // set PB3 low
    PORTB = 0b00000000;
    _delay_ms(200);
  }
 
  return 1;
}
