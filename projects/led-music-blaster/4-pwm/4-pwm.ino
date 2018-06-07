/* Four PWM Outputs */

// ATtiny85 outputs
const int Red = 0;
const int Green = 1;
const int Blue = 4;
const int White = 3;
volatile uint8_t* Port[] = {&OCR0A, &OCR0B, &OCR1A, &OCR1B};

void setup() {
  pinMode(Red, OUTPUT);
  pinMode(Green, OUTPUT);
  pinMode(Blue, OUTPUT);
  pinMode(White, OUTPUT);
  // Configure counter/timer0 for fast PWM on PB0 and PB1
  TCCR0A = 3<<COM0A0 | 3<<COM0B0 | 3<<WGM00;
  TCCR0B = 0<<WGM02 | 3<<CS00; // Optional; already set
  // Configure counter/timer1 for fast PWM on PB4
  TCCR1 = 1<<CTC1 | 1<<PWM1A | 3<<COM1A0 | 7<<CS10;
  GTCCR = 1<<PWM1B | 3<<COM1B0;
  // Interrupts on OC1A match and overflow
  TIMSK = TIMSK | 1<<OCIE1A | 1<<TOIE1;
}

ISR(TIMER1_COMPA_vect) {
  if (!bitRead(TIFR,TOV1)) bitSet(PORTB, White);
}

ISR(TIMER1_OVF_vect) {
  bitClear(PORTB, White);
}

// Sets colour Red=0 Green=1 Blue=2 White=3
// to specified intensity 0 (off) to 255 (max)
void SetColour (int colour, int intensity) {
  *Port[colour] = 255 - intensity;
}  

void loop() {
  for (int i=-255; i <= 254; i++) {
    OCR0A = abs(i);
    OCR0B = 255-abs(i);
    OCR1A = abs(i);
    OCR1B = 255-abs(i);
    delay(10);
  }
}
