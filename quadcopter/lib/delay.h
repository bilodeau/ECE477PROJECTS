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
	DDRB = 2;
	PORTB = 0;
	setup_delay();
	TCNT0 = 0;
	while(TCNT0 <= millis){
		char str[20];
		sprintf(str,"TCNT0 = %d",TCNT0);
		transmit(str);
	}
	PORTB = 2;
}
