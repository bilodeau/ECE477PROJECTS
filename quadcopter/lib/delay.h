#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
// DELAYS using timer0 based on the defined clock speed

#define CLOCKSPEED 8000000

void setup_delay(){
	TCCR0A = 0;
	TCCR0B = 2; // use prescalar 8 so the clock counts in milliseconds
}

void delay(unsigned char millis){
	TCNT0 = 0;
	while(TCNT0 <= millis);
}
