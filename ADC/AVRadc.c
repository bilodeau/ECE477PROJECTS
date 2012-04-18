#include <avr/io.h>
#include <avr/interrupt.h>
#include "AVRserial.h"

#define INTERRUPTCOUNTERLIMIT 128  // counts up to this limit every second

void setup_timer();
void setup_adc();
void do_adc_and_transmit();

char timer_flag = 0;
int interrupt_counter = 0;

int main(){
	setup_timer();
	setup_serial();
	setup_adc();
	for(;;){
		if (timer_flag){
			do_adc_and_transmit();
			timer_flag = 0;
		}
	}
	return 0;
}

void setup_timer(){
	// normal port operation, TOP = 0xFFFF, use prescaler 1/1024
	TCCR1A = 0;
	TCCR1B = 5;
	TIMSK = (1<<TOIE1); // enable overflow interrupt
}

ISR(TIMER1_OVF_vect){
	interrupt_counter++;
	if (interrupt_counter >= INTERRUPTCOUNTERLIMIT){
		timer_flag = 1;
		interrupt_counter -= INTERRUPTCOUNTERLIMIT;
	}
}

void setup_adc(){
	// use internal 2.56V reference
	// measure from ADC channel zero (pin 23 / PC0)
	ADMUX = (1<<REFS1)|(1<<REFS0);
	
	// enable ADC, set ADC prescalar to
	ADCSRA = (1<<ADEN)|(0<<ADPS2)|(0<<ADPS1)|(0<<ADPS0);
}

void do_adc_and_transmit(){
	// start a conversion
	ADCSRA |= (1<<ADSC);
	
	// wait for the conversion to complete
	while(!(ADCSRA & ADIF));
	unsigned char low = ADCL;
	unsigned char high = ADCH&3;
	int temp = (high<<8)|(low);

	// send the value out over serial
	char buf[40];
	sprintf(buf,"%d,",temp);
	transmit(buf);
}
