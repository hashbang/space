#ifndef _MAIN_H_
#define _MAIN_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

//#include "adc.h"
//#include "ms_timer.h"
//#include "usart.h"

#define F_CPU  8000000UL

/***** Configure IO *****/
#define CONFIG_AS_INPUT(bit)    {bit ## _DDR &= ~(1 << bit);}
#define CONFIG_AS_OUTPUT(bit)   {bit ## _DDR |= (1 << bit);}

// This globally disables pullups and tristates the pin in the argument. Saves power but use carefully. This also makes the macros header less generic
#define CONFIG_AS_DISABLED(bit)	{MCUCR |= (1 << PUD); bit ## _DDR &= ~(1 << pin); bit ## _PORT |= (1 << pin);}

#define PULLUP_ON(bit)          {bit ## _PORT |= (1 << bit);}
#define PULLUP_OFF(bit)         {bit ## _PORT &= ~(1 << bit);}

/***** Manipulate Outputs *****/
#define SET_HIGH(bit)           {bit ## _PORT |= (1 << bit);}
#define SET_LOW(bit)            {bit ## _PORT &= ~(1 << bit);}
#define SET_TOGGLE(bit)         {bit ## _PORT ^= (1 << bit);}

/***** Test Inputs *****/
#define IS_HIGH(bit)            (bit ## _PIN & (1 << bit))
#define IS_LOW(bit)             (! (bit ## _PIN & (1 << bit)))

#endif /* _MAIN_H_ */