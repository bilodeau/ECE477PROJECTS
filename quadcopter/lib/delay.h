#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
// DELAYS using timer0 based on the defined clock speed

#define CLOCKSPEED 8000000

void setup_delay(){
	TCCR0A = 0;
	TCCR0B = 5; // use prescalar 1024 so the clock counts in units of 128 microseconds
}

void delay(unsigned char millis){
	// delay for 8*millis as each tick is 128 us
	TCNT0 = 0;
	while(TCNT0 <= 8*millis);
}
