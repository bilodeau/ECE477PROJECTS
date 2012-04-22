#include <avr/io.h>
#include <avr/interrupt.h>
#include "AVRserial.h"
#include "fan.h"

#define INTERRUPTCOUNTERLIMIT 1000  // counts up to this limit every second

void setup_timer();
void setup_adc();
void transmit_temp();

volatile char timer_flag = 0;
volatile int interrupt_counter = 0;

volatile int reading_index = 0;
volatile int readings[16];
volatile int running_sum;

long last_temp_reading;

int main(){
	setup_serial();
	setup_timer();
	setup_adc();
	setup_fan();
	DDRB |= 2;
	for(;;){
		if (timer_flag){
			transmit_temp();
			update_fan(running_sum>>4);
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

ISR(ADC_vect){
	running_sum -= readings[reading_index]; // subtract the oldest reading
	unsigned char low = ADCL;
	unsigned char high = ADCH&3;
	readings[reading_index] = (high<<8)|(low);
	running_sum += readings[reading_index];

	reading_index++;
	if (reading_index >= 16)
		reading_index = 0;
}

void setup_adc(){
	// use internal 2.56V reference
	// measure from ADC channel zero (pin 23 / PC0)
	ADMUX = (1<<REFS1)|(1<<REFS0);
	
	// enable ADC, use free running mode, set ADC prescalar to 1/64
	// enable conversion complete interrupt
	ADCSRA = (1<<ADEN)|(1<<ADFR)|(1<<ADPS2)|(1<<ADPS1)|(0<<ADPS0)|(1<<ADIE)|(1<<ADSC);
}
	
// send the value out over serial, formatted for CSV
void transmit_temp(){
	int reading = (running_sum>>4); // get the average reading
	long temp = (reading/1024.*2.56-.5)*10000.; // convert to celsius
	char buf[40];
	sprintf(buf,"%ld,",temp); 
	transmit(buf);
}
