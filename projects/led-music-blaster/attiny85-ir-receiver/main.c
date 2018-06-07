
#include "main.h"

// LED outputs
//int LED[] = {1, 0, 3, 4};

volatile int NextBit;
volatile unsigned long RecdData;
//int Brightness;

// Demo routine
void ReceivedCode(bool Repeat) {
  // Check for correct remote control. Change this in future to allow
  // for custom IR blasters that use different addresses.
  if ((RecdData & 0xFFFF) != 0xFF00) return;
  
  // Read key pressed and run it through switch statement to determine action
  int key = RecdData >> 16 & 0xFF;
  switch(key)
  {
    case 0x45:  // A2 reversed
    SET_TOGGLE(RED_LED);
    break;
  }  
    
  SET_TOGGLE(GRN_LED);
}

// Interrupt service routine - called on every falling edge of PB2
// This code works but I'm not a huge fan of the magic numbers. I don't yet know what they all mean
ISR(INT0_vect) {
  uint8_t Time = TCNT0;
  uint8_t Overflow = TIFR & (1 << TOV0);
  
  // Keep looking for AGC pulse and gap
  if (NextBit == 32)
  {
    if ((Time >= 194) && (Time <= 228) && (Overflow == 0))
    {
      RecdData = 0; NextBit = 0;
    }
    else if ((Time >= 159) && (Time <= 193) && (Overflow == 0))
    {
      ReceivedCode(1);
    }
  // Data bit
  }
  else
  {
    if ((Time > 44) || (Overflow != 0))
    {
      NextBit = 32; // Invalid - restart
    }
    else
    {
      if (Time > 26)
      {
        RecdData = RecdData | ((unsigned long) (1 << NextBit));
      }
      if (NextBit == 31)
      {
        ReceivedCode(0);
      }
      NextBit++;
    }
  }

  TCNT0 = 0;                  // Clear counter
  TIFR = TIFR | 1<<TOV0;      // Clear overflow
  GIFR = GIFR | 1<<INTF0;     // Clear INT0 flag
}



ISR(TIMER1_COMPA_vect)
{
  //if ( !(TIFR & (1 << TOV1)) )
  //{
  SET_HIGH(WHT_LED);
  //}
}

ISR(TIMER1_OVF_vect)
{
  SET_LOW(WHT_LED);
}

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

  // Set up Timer/Counter0 (assumes 1MHz clock)

  // Configure counter/timer0 for fast PWM on PB0 and PB1
  //TCCR0A = 0;               // No compare matches
  TCCR0A = (3 << COM0A0) | (3 << COM0B0) | ( 3 << WGM00);
  TCCR0B = (3 << CS00); // clk_io/64 (From prescaler)
  //TCCR0B = 0<<WGM02 | 3<<CS00; // Optional; already set

  // Configure counter/timer1 for fast PWM on PB4
  GTCCR = 1<<PWM1B | 3<<COM1B0;
  //TCCR1 = 3<<COM1A0 | 7<<CS10;
  TCCR1 = (1 << CTC1) | (1 << PWM1A) | ( 3 << COM1A0) | (7 << CS10);
  //GTCCR = (1 << PWM1B) | (3 << COM1B0);

  // Interrupts on OC1A match and overflow
  TIMSK = TIMSK | (1 << OCIE1A) | (1 << TOIE1);

  // Set up INT0 interrupt on PB2
  //MCUCR = MCUCR | 2<<ISC00;   // Interrupt on falling edge
  //GIMSK = GIMSK | 1<<INT0;    // Enable INT0
  
  //NextBit = 32;               // Wait for AGC start pulse
  
  // Globally enable interrupts 
  sei();

  while (1)
  {
    for (int i=-255; i <= 254; i++)
    {
      OCR0A = abs(i);
      OCR0B = 255-abs(i);
      OCR1A = abs(i);
      OCR1B = 255-abs(i);
      _delay_ms(10);
    }

    // flash# 1:
    // set PB3 high
    //PORTB = 0b00001000; 
    //_delay_ms(20);
    // set PB3 low
    //PORTB = 0b00000000;
    //_delay_ms(20);

    // flash# 2:
    // set PB3 high
    //PORTB = 0b00001000; 
    //_delay_ms(200);
    // set PB3 low
    //PORTB = 0b00000000;
    //_/delay_ms(200);
  }
 
  return 1;
}
