/* Four PWM Outputs */

/***** Configure IO *****/
#define CONFIG_AS_INPUT(bit)    {bit ## _DDR &= ~(1 << bit);}
#define CONFIG_AS_OUTPUT(bit)   {bit ## _DDR |= (1 << bit);}

/***** Manipulate Outputs *****/
#define SET_HIGH(bit)           {bit ## _PORT |= (1 << bit);}
#define SET_LOW(bit)            {bit ## _PORT &= ~(1 << bit);}
#define SET_TOGGLE(bit)         {bit ## _PORT ^= (1 << bit);}

/***** Test Inputs *****/
#define IS_HIGH(bit)            (bit ## _PIN & (1 << bit))
#define IS_LOW(bit)             (! (bit ## _PIN & (1 << bit)))

// Proabably should make new macro to make _port, _pin, etc more automatic
#define GRN_LED       PB1
#define GRN_LED_PORT  PORTB
#define GRN_LED_DDR   DDRB
#define GRN_LED_PIN   PINB

#define RED_LED       PB0
#define RED_LED_PORT  PORTB
#define RED_LED_DDR   DDRB
#define RED_LED_PIN   PINB

#define BLU_LED       PB4
#define BLU_LED_PORT  PORTB
#define BLU_LED_DDR   DDRB
#define BLU_LED_PIN   PINB

#define WHT_LED       PB3
#define WHT_LED_PORT  PORTB
#define WHT_LED_DDR   DDRB
#define WHT_LED_PIN   PINB

#define IR_RCVR       PB2
#define IR_RCVR_PORT  PORTB
#define IR_RCVR_DDR   DDRB
#define IR_RCVR_PIN   PINB

#define NEC_CODE_ON         0x45 // (A2)
#define NEC_CODE_OFF        0x47 // (E2)
#define NEC_CODE_WHITE      0x4A // (52)
#define NEC_CODE_RED        0x16 // (68)
#define NEC_CODE_GREEN      0x19 // (98)
#define NEC_CODE_BLUE       0x0D // (B0)
#define NEC_CODE_LTBLUE     0x5A // (5A)
#define NEC_CODE_AQUA       0x18 // (18)
#define NEC_CODE_PURPLE     0x08 // (10)
#define NEC_CODE_YELLOW     0x52 // (4A)
#define NEC_CODE_ORANGE     0x0C // (30)
#define NEC_CODE_PINK       0x42 // (42)
#define NEC_CODE_BRIGHTER   0x15 // (A8)
#define NEC_CODE_DIMMER     0x09 // (90)
#define NEC_CODE_FLASH      0x44 // (22)
#define NEC_CODE_SMOOTH     0x07 // (E0)


// ATtiny85 outputs
const int Red = 0;
const int Green = 1;
const int Blue = 4;
const int White = 3;
const int irled = 2;

// These values control the output compare registers in the main loop
volatile uint8_t rval;
volatile uint8_t gval;
volatile uint8_t bval;
volatile uint8_t wval;

volatile int NextBit;
volatile unsigned long RecdData;
int Brightness;

void ReceivedCode(boolean Repeat) {
  // Check for correct remote control
  if ((RecdData & 0xFFFF) != 0xFF00) return;
  
  // Read key pressed
  int key = RecdData >> 16 & 0xFF;
  
  /* Keys 1, 2, and 3 toggle the corresponding LED
  if ((key >= 0x10) && (key <= 0x12) && !Repeat) {
    int pin = LED[key - 0x10];
    digitalWrite(pin, !digitalRead(pin));
  // Keys VOL- and VOL+ adjust the brightness of LED 4
  } else if (key == 0x02) analogWrite(4, Brightness++);
  else if (key == 0x00) analogWrite(4, Brightness--);
  */

  switch (key)
  {
    case NEC_CODE_ON:
      rval = 0;
      gval = 0;
      bval = 0;
      wval = 0;
      break;
     case NEC_CODE_OFF:
      rval = 255;
      gval = 255;
      bval = 255;
      wval = 255;
      break;
    case NEC_CODE_RED:
      rval = 0;
      gval = 255;
      bval = 255;
      wval = 255;
      break;
    case NEC_CODE_BLUE:
      rval = 255;
      gval = 255;
      bval = 0;
      wval = 255;
      break;
    case NEC_CODE_GREEN:
      rval = 255;
      gval = 0;
      bval = 255;
      wval = 255;
      break;
    case NEC_CODE_WHITE:
      rval = 255;
      gval = 255;
      bval = 255;
      wval = 0;
      break;
  }
}

void setup() {
  CONFIG_AS_INPUT(IR_RCVR);
  CONFIG_AS_OUTPUT(RED_LED);
  CONFIG_AS_OUTPUT(GRN_LED);
  CONFIG_AS_OUTPUT(BLU_LED);
  CONFIG_AS_OUTPUT(WHT_LED);
  
  // Configure counter/timer0 for fast PWM on PB0 and PB1
  TCCR0A = (3 << COM0A0) | (3 << COM0B0) | (3 << WGM00);
  TCCR0B = (0 << WGM02)  | (3 << CS00); // Optional; already set
  
  // Configure counter/timer1 for fast PWM on PB4
  TCCR1 = (1 << CTC1)  | (1 << PWM1A) | (3 << COM1A0) | (7 << CS10);
  GTCCR = (1 << PWM1B) | (3 << COM1B0);
  
  // Interrupts on OC1A match and overflow
  TIMSK = TIMSK | (1 << OCIE1A) | (1 << TOIE1);

  // Set up INT0 interrupt on PB2
  MCUCR = MCUCR | 2<<ISC00;   // Interrupt on falling edge
  GIMSK = GIMSK | 1<<INT0;    // Enable INT0
  NextBit = 32;               // Wait for AGC start pulse

  // Setting the output compare values to 255 turns the LEDs off 
  rval = bval = gval = wval = 255;
}

// External interrupt request 0 - called on every falling edge of PB2.
// This is vector number 2 which has the highest priority aside from RESET
ISR(INT0_vect) {
  int Time = TCNT0;
  int Overflow = TIFR & 1<<TOV0;
  // Keep looking for AGC pulse and gap
  if (NextBit == 32) {
    if ((Time >= 194) && (Time <= 228) && (Overflow == 0)) {
      RecdData = 0; NextBit = 0;
    } else if ((Time >= 159) && (Time <= 193) && (Overflow == 0)) ReceivedCode(1);
  // Data bit
  } else {
    if ((Time > 44) || (Overflow != 0)) NextBit = 32; // Invalid - restart
    else {
      if (Time > 26) RecdData = RecdData | ((unsigned long) 1<<NextBit);
      if (NextBit == 31) ReceivedCode(0);
      NextBit++;
    }
  }
  TCNT0 = 0;                  // Clear counter
  TIFR = TIFR | 1<<TOV0;      // Clear overflow
  GIFR = GIFR | 1<<INTF0;     // Clear INT0 flag
}

ISR(TIMER1_COMPA_vect) {
  //if (!bitRead(TIFR,TOV1)) bitSet(PORTB, White);
  if (!(TIFR & (1 << TOV1))) SET_HIGH(WHT_LED); 
}

ISR(TIMER1_OVF_vect) {
  //bitClear(PORTB, White);
  SET_LOW(WHT_LED);
}

void loop() {
  OCR0A = rval;
  OCR0B = bval;
  OCR1A = gval;
  OCR1B = wval;
  delay(1);
}
