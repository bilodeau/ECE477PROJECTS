#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "AVRserial.h"
#include "baud.h"

volatile int timer;	// the number of times the interrupt has been called
void transmit_burst();
void setup_pwm();
int receive_buffer_index = 0;
unsigned int get_burst_value(char *ptr) {
        unsigned int result = 0;
        unsigned char temp;
        char i;
        for (i = 0; i <4; i++) {
                temp = *ptr - '0';
                result |= (temp<<(4*(3-i)));
                ptr++;
        }
        return result;
}

// check to see if the receive_buffer contains a valid IR Code
char check_buffer(){	
	if (strlen(receive_buffer) < 20)
		return 0; // code was too short to hold its own length
	int numPairs1 = get_burst_value(receive_buffer + 10);
	int numPairs2 = get_burst_value(receive_buffer + 15);

	char temp[100];
	sprintf(temp,"1: %d 2: %d len: %d",numPairs1,numPairs2,strlen(receive_buffer));
	transmit(temp);

	if (strlen(receive_buffer) != 19 + numPairs1*10 + numPairs2*10)
		return 0;
	return 1;
}

// ISR to keep track of carrier frequency periods
ISR (TIMER1_COMPA_vect) {
	timer++;
}	

// transmit the next burst in the receive_buffer	
void transmit_burst() {
	int num_cycles = 2*get_burst_value(receive_buffer + receive_buffer_index);	// set first duration
	timer = 0;
	TCCR1A ^= (1<<COM1A0);// switch the pin mode so that it switches between all off and toggling
	while (timer <= num_cycles);			// wait for the signal to be broadcast for the num of cycles
}	

// setup PWM to transmit IR Signal
void setup_pwm() {
    	DDRB = 2;
	TCCR1A = (0<<COM1A0);
	TCCR1B = (1<<WGM12)|1; // use WGM mode 4 CTC with OCR1A as TOP, prescaler of 1
	OCR1A = 0xFFFF;
	TIMSK = (1<<OCIE1A);					// enable the output compare match interrupt
	sei();
}
