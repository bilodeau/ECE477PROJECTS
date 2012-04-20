#include <avr/io.h>
#include <avr/interrupt.h>
#include "AVRserial.h"
#include "fan.h"

#define INTERRUPTCOUNTERLIMIT 1000  // counts up to this limit every second

void setup_timer();
void setup_adc();
void do_adc_and_transmit();

volatile char timer_flag = 0;
volatile int interrupt_counter = 0;
long last_temp_reading;

int main(){
	setup_serial();
	setup_timer();
	setup_adc();
	setup_fan();
	DDRB |= 2;
	for(;;){
		if (timer_flag){
			do_adc_and_transmit();
			update_fan(last_temp_reading);
			timer_flag = 0;
			PORTB ^= 2;
		}
	}
	return 0;
}

void setup_timer(){
	// WGM mode 4 CTC, TOP = OCR1A, use prescaler 1/8, counts in us
	TCCR1A = 0;
	TCCR1B = (1<<WGM12)|2;
	OCR1A = 999; // once a millisecond
	TIMSK = (1<<OCIE1A); // enable overflow interrupt
	sei(); // enable interrupts globally
}

ISR(TIMER1_COMPA_vect){
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

	int i;
	long sum = 0;
	for (i = 0; i <16; i++){
		// start a conversion
		ADCSRA |= (1<<ADSC);
	
		// wait for the conversion to complete
	//	while(!(ADCSRA & ADIF));
		unsigned char low = ADCL;
		unsigned char high = ADCH&3;
		last_temp_reading = (high<<8)|(low);
	
		// adjust the reading to be in degrees C
		last_temp_reading = (last_temp_reading/1024.0 * 2.56 - .5) * 10000;
			sum += last_temp_reading;
	}
	sum >>= 4;
	// send the value out over serial
	char buf[40];
	sprintf(buf,"%ld,",sum);
	transmit(buf);
}
