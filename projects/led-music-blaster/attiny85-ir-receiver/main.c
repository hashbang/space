// main.c
// 
// A simple blinky program for ATtiny85
// Connect red LED at pin 2 (PB3)
//
// electronut.in

#include "main.h"

#define GREEN_DIR 
 
int main (void)
{
  // Output Pin Setup
  // PB0 = OC0A
  // PB1 = OC0B
  // PB3 = /OC1B
  // PB4 = OC1B

  // Input Pin Setup
  // PB2 = INT0

	DDRB = 0b00011011;
  
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
