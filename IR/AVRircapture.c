// A beginning setup for Timers for IR Part B
// Notice that the interrupts reset TCNT1.
// This might be bad, but the Datasheet used a TCNT1 reset as a coding example.

#define NOT_RDY 0
#define WAIT_ON_SIG 1
#define GET_DIV 2
#define COUNT_L_PULSE 3
#define COUNT_D_PULSE 4

void setup_input_capture(){
	// enable input capture interrupt and compare match a
	TIMSK |= (1<<TICIE1)|(1<<OCIE1A);
	sei();
}
int signal_array[100];
int signal_index;

int light_count, dark_count, start_time, mode, signal_ready, divisor;


// handles the input pulse from the IR
//cycles through 4 modes:
// mode 1 = the PC sent a prompt to get ready to capture a signal
// mode 2 = already received one pulse, now use the second pulse to find the divisor value
// mode 3 = already got divisor, now just counting num cycles in the light burst
// mode 4 = was counting the length of a dark burst

// **** This Interrupt resets TCNT1, which will cause one Compare match to not happen.******
//  This might cause problems since OCR1A is set so close to when the Input Capture occurs.
// I am not sure how to fix this.
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
			OCR1A = time + 1.5*divisor;		// set val to catch missed pulse
			OCR1B = time + divisor;			// set up timer to measure cycles
			signal_array[signal_index++] = divisor;	// record divisor in array  		
			light_count = 2;		// two light periods have occurred so far
			mode++;				// mode = COUNT_L_PULSE
			break;
		case COUNT_L_PULSE :	// was counting length of light burst
			light_count++;	// increment num cycles
			OCR1A = time + 1.5*divisor;	// move dark pulse catch
			OCR1B = time + divisor;		// time of expected pulse
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
	dark_count++;
	if ((dark_count > 225) && (mode == COUNT_D_PULSE)) {
		signal_array[signal_index++] = 0x0FFF;
		signal_array[signal_index] = '\0';
		signal_recieve_flag = 1;	// signal has been received. set flag.
		mode = NOT_RDY;
	}
}
