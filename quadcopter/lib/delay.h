#ifndef DELAY_H_
#define DELAY_H_

#include <avr/io.h>
// DELAYS using timer1 based on the defined clock speed

volatile char delay_flag = 0;

// can't delay more than 32 ms
void delay(unsigned char millis){
	TIMSK1 |= (1<<OCIE1B);	
	delay_flag = 1;
	OCR1B = (TCNT1 + 1000*millis) % ICR1;
	while(delay_flag);
	TIMSK1 &= ~(1<<OCIE1B);	
}

ISR(TIMER1_COMPB_vect){
	delay_flag = 0;
}
#endif
