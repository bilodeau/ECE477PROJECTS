#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "AVRserial.h"
#include "baud.h"

volatile int timer;	// the number of times the interrupt has been called

char cmd[] = "0000 0069 0009 0000 0100 0060 0020 0060 0020 0020 0020 0020 0020 0020 0020 0020 0020 0060 0020 0020 0020 0FFF";

void transmit_burst();
void setup_pwm();
int receive_buffer_index = 0;

void test(){
sprintf(receive_buffer,"%s",cmd);
serial_command_ready = 1;
}

unsigned int get_burst_value(char *ptr) {
        unsigned int result = 0;
        unsigned char temp;
        char i;
        for (i = 0; i <4; i++) {
                temp = *ptr - '0';
                result |= (temp<<(4*(3-i)));
                ptr++;
        }
	transmit("getting burst pair");   
        return result;
}

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

int main() {
	setup_serial();
	setup_pwm();
    	for ( ; ; ) {
		if (serial_command_ready) {
			if (check_buffer()){
				OCR1A = get_burst_value(receive_buffer + 5) - 1;// set carrier frequency
				receive_buffer_index = 20;
				while (receive_buffer[receive_buffer_index] != '\0') {	// while there are still bursts to be sent
										// flip the mode from whatever it was previously
					transmit_burst();				//	trransmit the burst
					receive_buffer_index += 5;		// increment index to the next burst
				}
			}else{
				// bad stuff in receive_buffer, so blank it out
				receive_buffer[0] ='\0';
			}
			serial_command_ready = 0;
			receive_buffer_index = 0;
		}
	}
	
}

ISR (TIMER1_COMPA_vect) {
	timer++;
}	
	
void transmit_burst() {
	int num_cycles = 2*get_burst_value(receive_buffer + receive_buffer_index);	// set first duration
	timer = 0;
	TCCR1A ^= (1<<COM1A0);// switch the pin mode so that it switches between all off and toggling
	while (timer <= num_cycles);			// wait for the signal to be broadcast for the num of cycles
}	


void setup_pwm() {
    DDRB = 2;
	TCCR1A = (0<<COM1A0);
	TCCR1B = (1<<WGM12)|1; // use WGM mode 4 CTC with OCR1A as TOP, prescaler of 1
	OCR1A = 0xFFFF;
	TIMSK = (1<<OCIE1A);					// enable the output compare match interrupt
	sei();
}
