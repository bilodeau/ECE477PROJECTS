#include <avr/io.h>
#include <avr/iom8.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "charlie.c"

// define pin modes
#define PINGING 0
#define WAITINGFORECHO 1
char pinMode;
int beginEchoTime;

int main(void){
	TCCR1A = 0;
	TCCR1B = 0x09;
	OCR1A = 60000;
	OCR1B = 5;
	TIMSK = 32+16+8;
	sei();
	while(1){
		flash_leds();
	}
}

ISR(TIMER1_CAPT_vect){
	int time = ICR1;
	if (pinMode == WAITINGFORECHO){
		if ((((TCCR1B & 64))>>6)==1){ // rising edge
			beginEchoTime = time;
			TCCR1B &= ~64; // now listen for falling edges
		}else{ // falling edge
			time -= beginEchoTime;
			time /= 2; // get the one-way time to target
			double  distance = time * 1130./1000000; // distance in feet
			unsigned int index = distance;
			if (distance > 19)
				index = 19;
			clear();
			lightled(index);
			TCCR1B |= 64;  // now listen for rising edges
		}
	}	
}

ISR(TIMER1_COMPA_vect){
	DDRB |= 1; // output mode
	PORTB |= 1;  // send high
	pinMode = PINGING;
}

ISR(TIMER1_COMPB_vect){
	PORTB &= ~1; // send low
	DDRB &= ~1; // setup input mode
	pinMode = WAITINGFORECHO;
}

