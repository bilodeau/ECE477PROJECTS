#include <stdlib.h>
#include "AVRir.h"

#define NOT_RDY 0
#define WAIT_ON_SIG 1
#define GET_DIV 2
#define COUNT_L_PULSE 3
#define COUNT_D_PULSE 4


char* get_ascii_hex(int i);
void setup_input_capture();
void build_ir_code(char *word);

int signal_array[100];
int signal_index;
int light_count, dark_count, start_time, mode, signal_ready, divisor, num_signals;

int main(void) {
	setup_serial();
	signal_ready = 0;
	mode = NOT_RDY;
	for ( ; ; ) {
		if (serial_command_ready) {			// if command is ready to be read
			if (serial_command_buffer[0] == 'l') {	// if command is to capture IR Signal
				setup_input_capture();		// setup interrupts for capture
				mode = WAIT_ON_SIG;		// start waiting for a signal
				if(signal_ready) {		// if a code has been captured
					char *word = malloc((signal_index + 2)*5 + 1);
					build_ir_code(word);
					transmit(word);
					signal_ready = 0;
				}
			} else if (check_buffer()) {	// if IR Code is good
				setup_pwm();		// setup pwm to send IR Signal
				transmit_signal();	// transmit signal
			} else {
				receive_buffer = '\0';	// else bad command. zero out buffer
			}
			serial_command_ready = 0;	// reset ready flag
			receive_buffer_index = 0;	// reset buffer index	
		}				
	}	
}


// takes a char ptr and puts a Pronto format IR Code into it
void build_ir_code(char *word) {
	strcat(word, "0000 ");
	strcat(word, get_ascii_hex(signal_array[0]));	// concatentate the divisor
	strcat(word, get_ascii_hex((signal_index - 2)/2));// concat the num 1st burst pair
	strcat(word, "0000 ");			// concat 0 for 2nd burst pair
	int i;
	for (i = 1; i < signal_index - 1; i++) {	// for each signal, concat to the string
		strcat(word, get_ascii_hex(signal_array[i]));
	}
	strcat(word, "0FFF");		// concat final dark burst
}

// get 5 char ascii hex representation of a decimal number
char *get_ascii_hex(int i) {
	char *word = malloc(6);
	sprintf(word, "00%x ", i);
	return word; 
}

// setup input capture interrupts
void setup_input_capture(){
	OCR1A = 0xFFFF;
	OCR1B = 0xFFFF;
	DDRB = 0;
	PORTB = 0;
	TCCR1A = 0;
	TCCR1B = 0x01; // use no prescaler
	TCCR1B = (0<<ICES1); // capture on falling edge

	// enable input capture interrupt and compare match a
	TIMSK = (1<<TICIE1)|(1<<OCIE1A)|(1<<OCIE1B);
	sei();
}

// handles the input pulse from the IR
//cycles through 4 modes:
// mode WAIT_ON_SIG = the PC sent a prompt to get ready to capture a signal
// mode GET_DIV = already received one pulse, now use the second pulse to find the divisor value
// mode COUNT_L_PULSE = already got divisor, now just counting num cycles in the light burst
// mode COUNT_D_PULSE = was counting the length of a dark burst
// mode NOT_RDY = stop waiting for IR Signal

ISR(TIMER1_CAPT_vect) {
	int time = ICR1;
	switch (mode) {
		case WAIT_ON_SIG :	// was waiting to capture signal
			start_time = time;	// get start time
			signal_index = 0;	// start at beginning of signal_array
			mode++;			// mode = GET_DIV
			break;
		case GET_DIV :		// was getting divisor
			divisor = time - start_time;	// calc divisor
			if (divisor < 0) {	// if divisor is neg, ovflow occurred
				divisor = time + (0xFFFF - start_time) + 1;
			}
			OCR1B = time + divisor;		// time of expected pulse
			signal_array[signal_index++] = divisor;	// record divisor in array  		
			light_count = 2;		// two light periods have occurred so far
			mode++;				// mode = COUNT_L_PULSE
			break;
		case COUNT_L_PULSE :	// was counting length of light burst
			light_count++;	// increment num cycles
			OCR1A = time + 1.5*divisor;	// move dark pulse catch
			break;
		case COUNT_D_PULSE :	// was counting length of dark burst
			signal_array[signal_index++] = dark_count;	// update sig array
			light_count = 1;		// one light pulse has occurred
			mode = COUNT_L_PULSE;	//switch baqck to counting light pulses
			break;
		default:// else not waiting for signal, so do nothing
			break;
	}
}

// uses a compare value slightly above the carrier frequency.
// if an input capture hasn't happened by the time this fires, then 
// the light burst is done and the dark burst has started
ISR(TIMER1_COMPA_vect) {
	if (mode == COUNT_L_PULSE) {	// just finished counting a light pair length
		signal_array[signal_index++] = light_count;	// put val into array
		light_count = 0;	// reset light count
		dark_count = 1;		// one dark burst has occurred so far
		mode = COUNT_D_PULSE;	// set mode to counting dark burst length 
	}
}

// this interrupt fires in snyc with the carrier frequency
// it is used to keep track of the length of dark bursts
ISR(TIMER1_COMPB_vect) {
	OCR1B += divisor;
	if (mode == COUNT_D_PULSE) {
		dark_count++;
		if (dark_count > 225) {
			signal_array[signal_index++] = 0x0FFF;
			signal_array[signal_index] = '\0';
			signal_ready = 1;	// signal has been received. set flag.
			mode = NOT_RDY;
		}
	}
}
