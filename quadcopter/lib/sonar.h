#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

// define pin modes
#define PINGING 0
#define WAITINGFORECHO 1
char sonar_pinMode;
int sonar_beginEchoTime;
volatile double sonar_distance = 0;

void query_sonar(){
	int temp = sonar_distance;
	int temp2 = 100*(sonar_distance - temp);
	char str[40];
	sprintf(str,"Range to target is (ft) : %d.%d",temp,temp2);
	transmit(str); 
}

void setup_sonar(void){
	// use WGM mode 4, which is Clear Timer on Compare Match with OCR1A as the TOP
	TCCR1A = 0;
	TCCR1B = 0x0A; // use 1/8 prescaler so the clock counts in microseconds
	OCR1A = 60000; // TOP value
	OCR1B = 5; // this value is the duration of the signal trigger pulse
	TIMSK1 = (1<<ICIE1)|(1<<OCIE1B)|(1<<OCIE1A); // enable the three interrupts we need
	sei();
}

ISR(TIMER1_CAPT_vect){
	int time = ICR1;
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

