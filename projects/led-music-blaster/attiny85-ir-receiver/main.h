#ifndef _MAIN_H_
#define _MAIN_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>


// #define F_CPU  1000000UL

#define NEC_CODE_ON		0x45
#define NEC_CODE_OFF	0x47


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

// Proabably should make new macro to make _port, _pin, etc more automatic
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

#endif /* _MAIN_H_ */