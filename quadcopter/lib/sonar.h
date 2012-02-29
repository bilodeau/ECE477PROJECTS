#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

// define pin modes
#define PINGING 0
#define WAITINGFORECHO 1
char sonar_pinMode;
int sonar_beginEchoTime;

void setup_sonar(void){
	TCCR1A = 0;
	TCCR1B = 0x09;
	OCR1A = 60000;
	OCR1B = 5;
	TIMSK = 32+16+8;
	sei();
}

ISR(TIMER1_CAPT_vect){
	int time = ICR1;
	if (sonar_pinMode == WAITINGFORECHO){
		if ((((TCCR1B & 64))>>6)==1){ // rising edge
			sonar_beginEchoTime = time;
			TCCR1B &= ~64; // now listen for falling edges
		}else{ // falling edge
			time -= sonar_beginEchoTime;
			time /= 2; // get the one-way time to target
			double  distance = time * 1130./1000000; // distance in feet
			char str[20];
			sprintf(str,"Range to target (ft) : %f",distance)
			transmit(str);
			TCCR1B |= 64;  // now listen for rising edges
		}
	}	
}

ISR(TIMER1_COMPA_vect){
	DDRB |= 1; // output mode
	PORTB |= 1;  // send high
	sonar_pinMode = PINGING;
}

ISR(TIMER1_COMPB_vect){
	PORTB &= ~1; // send low
	DDRB &= ~1; // setup input mode
	sonar_pinMode = WAITINGFORECHO;
}

