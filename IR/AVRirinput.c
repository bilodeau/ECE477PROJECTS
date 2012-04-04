#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "AVRserial.h"

// define pin modes
#define PINGING 0
#define WAITINGFORECHO 1
char sonar_pinMode;
int sonar_beginEchoTime;

int main(){
	setup_serial();
	setup();
	transmit("setup done");
	while(1){
//		transmit("SPAM");	
	}	
	return 0;
}

void setup(void){
	//enable input on pin B0
	DDRB = 0;
	PORTB = 0;
	// use normal mode WGM = 0
	TCCR1A = 0;
	TCCR1B = 0x02; // use 1/8 prescaler so the clock counts in microseconds
	TCCR1B |= (1<<ICES1);  // input capture on rising edge
	TIMSK = (1<<TICIE1);//|(1<<OCIE1B)|(1<<OCIE1A); // enable the three interrupts we need
	sei();
}

ISR(TIMER1_CAPT_vect){
	
	transmit("input capture!!");
	
	/*int time = ICR1;
	if (sonar_pinMode == WAITINGFORECHO){
			// mask and shift to find the current edge selection
		if ((((TCCR1B & (1<<ICES1)))>>ICES1)==1){ // rising edge
			sonar_beginEchoTime = time;
			TCCR1B &= ~(1<<ICES1); // now listen for falling edges
		}else{ // falling edge
			time -= sonar_beginEchoTime;
			time /= 2; // get the one-way time to target
			sonar_distance = time * 1130./1000000; // distance in feet
			TCCR1B |= (1<<ICES1);  // now listen for rising edges
		}
	}*/	
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

