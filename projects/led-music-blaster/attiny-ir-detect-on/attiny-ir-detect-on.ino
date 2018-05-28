/* ATtiny85 IR Remote Control Receiver

   David Johnson-Davies - www.technoblogy.com - 3rd April 2015
   ATtiny85 @ 1 MHz (internal oscillator; BOD disabled)
   
   CC BY 4.0
   Licensed under a Creative Commons Attribution 4.0 International license: 
   http://creativecommons.org/licenses/by/4.0/
*/

// LED outputs
int LED[] = {1, 0, 3, 4};

volatile int NextBit;
volatile unsigned long RecdData;
int Brightness;

// Demo routine
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
  
  if (key == 0x45)
  {
    digitalWrite(0, !digitalRead(0));
  }
  
  digitalWrite(1, !digitalRead(1));
}

/*
// Debugging routine - beeps the 32-bit code on PB0
void ReceivedCode() {
  for (unsigned long b=0x80000000; b!=0; b = b>>1) {
    for (unsigned int i=0; i<2500; i++) {
      if (RecdData & b) digitalWrite(0, (i & 8)); else digitalWrite(0, (i & 16));
    }
    for (unsigned int i=0; i<5000; i++) digitalWrite(0, (i & 0));
  }
}
*/

// Interrupt service routine - called on every falling edge of PB2
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

// Setup **********************************************
  
void setup() {
  // Set up LEDs
  for (int x=0; x<4; x++) pinMode(LED[x], OUTPUT);
  // Set up Timer/Counter0 (assumes 1MHz clock)
  TCCR0A = 0;                 // No compare matches
  TCCR0B = 3<<CS00;           // Prescaler /64
  // Set up INT0 interrupt on PB2
  MCUCR = MCUCR | 2<<ISC00;   // Interrupt on falling edge
  GIMSK = GIMSK | 1<<INT0;    // Enable INT0
  NextBit = 32;               // Wait for AGC start pulse
}

void loop() {
}
