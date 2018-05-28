/* Four PWM Outputs */

// ATtiny85 outputs
const int Red = 0;
const int Green = 1;
const int Blue = 4;
const int White = 3;
volatile uint8_t* Port[] = {&OCR0A, &OCR0B, &OCR1A, &OCR1B};

const int NEC_ON[] = {
// ON, OFF (in 10's of microseconds)

// Leading pulse with space
920, 468,

// 0x00
52, 62,
52, 62,
52, 62,
52, 62,
52, 62,
52, 62,
52, 62,
52, 62,

// 0xFF
52, 176,
52, 176,
52, 176,
52, 176,
52, 176,
52, 176,
52, 176,
52, 176,

// A2
52, 176,
52, 62,
52, 176,
52, 62,
52, 62,
52, 62,
52, 176,
52, 62,

//5D
52, 62,
52, 176,
52, 62,
52, 176,
52, 176,
52, 176,
52, 62,
52, 176
};

#define IRpin_PIN      PINB
#define IRpin          2

// the maximum pulse we'll listen for - 65 milliseconds is a long time
#define MAXPULSE 65000
#define NUMPULSES 50

// what our timing resolution should be, larger is better
// as its more 'precise' - but too large and you wont get
// accurate timing
#define RESOLUTION 20 

// What percent we will allow in variation to match the same code
#define FUZZINESS 20

// we will store up to 100 pulse pairs (this is -a lot-)
uint16_t pulses[NUMPULSES][2];  // pair is high and low pulse 
uint8_t currentpulse = 0; // index for pulses we're storing

//KGO: added size of compare sample. Only compare the minimum of the two
boolean IRcompare(int numpulses, int Signal[], int refsize) {
  int count = min(numpulses,refsize);
  for (int i=0; i< count-1; i++) {
    int oncode = pulses[i][1] * RESOLUTION / 10;
    int offcode = pulses[i+1][0] * RESOLUTION / 10;
    
    // check to make sure the error is less than FUZZINESS percent
    if ( abs(oncode - Signal[i*2 + 0]) <= (Signal[i*2 + 0] * FUZZINESS / 100)) {

    } else {

      // we didn't match perfectly, return a false match
      return false;
    }
        
    if ( abs(offcode - Signal[i*2 + 1]) <= (Signal[i*2 + 1] * FUZZINESS / 100)) {
    } else {
      // we didn't match perfectly, return a false match
      return false;
    }
  }
  
  // Everything matched!
  return true;
}

int listenForIR(void) {
  currentpulse = 0;
  
  while (1) {
    uint16_t highpulse, lowpulse;  // temporary storage timing
    highpulse = lowpulse = 0; // start out with no pulse length
  
//  while (digitalRead(IRpin)) { // this is too slow!
    while (IRpin_PIN & (1 << IRpin)) {
       // pin is still HIGH

       // count off another few microseconds
       highpulse++;
       delayMicroseconds(RESOLUTION);

       // If the pulse is too long, we 'timed out' - either nothing
       // was received or the code is finished, so print what
       // we've grabbed so far, and then reset
       
       // KGO: Added check for end of receive buffer
       if (((highpulse >= MAXPULSE) && (currentpulse != 0))|| currentpulse == NUMPULSES) {
         return currentpulse;
       }
    }
    // we didn't time out so lets stash the reading
    pulses[currentpulse][0] = highpulse;
  
    // same as above
    while (! (IRpin_PIN & _BV(IRpin))) {
       // pin is still LOW
       lowpulse++;
       delayMicroseconds(RESOLUTION);
        // KGO: Added check for end of receive buffer
        if (((lowpulse >= MAXPULSE)  && (currentpulse != 0))|| currentpulse == NUMPULSES) {
         return currentpulse;
       }
    }
    pulses[currentpulse][1] = lowpulse;

    // we read one high-low pulse successfully, continue!
    currentpulse++;
  }
}

void setup() {
  pinMode(Red, OUTPUT);
  pinMode(Green, OUTPUT);
  pinMode(Blue, OUTPUT);
  pinMode(White, OUTPUT);
  
  // Configure counter/timer0 for fast PWM on PB0 and PB1
  TCCR0A = (3 << COM0A0) | (3 << COM0B0) | ( 3 << WGM00);
  TCCR0B = (3 << CS00); // clk_io/64 (From prescaler)

  
  // Configure counter/timer1 for fast PWM on PB4
  TCCR1 = (1 << CTC1) | (1 << PWM1A) | ( 3 << COM1A0) | (7 << CS10);
  GTCCR = (1 << PWM1B) | (3 << COM1B0);
  
  // Interrupts on OC1A match and overflow
  TIMSK = TIMSK | (1 << OCIE1A) | (1 << TOIE1);
}

ISR(TIMER1_COMPA_vect) {
  if (!bitRead(TIFR,TOV1)) bitSet(PORTB, White);
}

ISR(TIMER1_OVF_vect) {
  bitClear(PORTB, White);
}

void loop() {
  //OCR0A = OCR0B = OCR1A = OCR1B = 0;
  int numberpulses;
  numberpulses = listenForIR();
  
  if (IRcompare(numberpulses, NEC_ON, sizeof(NEC_ON)/4)) {
    OCR1B = 255;
    delay(500);
  } else {
    OCR1B = 0;  
  }
  
  OCR0A = OCR0B = OCR1A = 0;
  delay(1000);
  OCR0A = OCR0B = OCR1A = 255;
  delay(1000);  
}
