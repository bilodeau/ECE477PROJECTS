#include <avr/io.h>
#include <string.h>
char mode;	// 0 = transmitting low,  1 = transmitting high
#include "AVRserialir.h"
#include "baud.h"

int num_cycles;	// the number of cycles at the carrier frequency for a burst
volatile int timer;	// the number of times the interrupt has been called

char cmd[] = "0000 0069 0009 0000 0100 0060 0020 0060 0020 0020 0020 0020 0020 0020 0020 0020 0020 0060 0020 0020 0020 0FFF";

void transmit_burst();
void setup_pwm();
void test(){
sprintf(receive_buffer,"%s",cmd);
mode = 1;
serial_command_ready = 1;
}

int main() {

    	mode = 0;
	setup_serial();
	setup_pwm();
	
	transmit("started test");

    	for ( ; ; ) {
		//test();
		if (serial_command_ready) {
			OCR1A = 100*get_burst_value(receive_buffer + 5);				// set carrier frequency
			receive_buffer_index = 20;
			while (receive_buffer[receive_buffer_index] != '\0') {	// while there are still bursts to be sent
										// flip the mode from whatever it was previously
				transmit_burst();				//	trransmit the burst
				receive_buffer_index += 5;		// increment index to the next burst
			}
			serial_command_ready = 0;
		}
	}
	
}

ISR (TIMER1_COMPA_vect) {
	if (mode){	
		PORTB ^= 2;       // mode = 1 means sending high burst, so flip the bit
	}else{		
		PORTB = 2;        // else sending low burst, so keep bit low
	}
	timer++;
}	
	
void transmit_burst() {
	timer = 0;
	num_cycles = get_burst_value(receive_buffer + receive_buffer_index);	// set first duration
	TIMSK |= (1<<OCIE1A);					// enable the output compare match interrupt
	while (timer != num_cycles);			// wait for the signal to be broadcast for the num of cycles
	TIMSK &= ~(1<<OCIE1A);
	char temp[40];
	sprintf(temp,"num_cycles is: %d",num_cycles);
	transmit(temp);					// disable the interrupt;
}	


void setup_pwm() {
    DDRB = 2;
    PORTB = 2;
	TCCR1A = (0<<COM1A1);
	TCCR1B = (1<<WGM12)|5; // use WGM mode 4 CTC with OCR1A as TOP, prescaler of 1/1024
	sei();
}


