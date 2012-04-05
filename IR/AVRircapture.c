// A beginning setup for Timers for IR Part B
// Notice that the interrupts reset TCNT1.
// This might be bad, but the Datasheet used a TCNT1 reset as a coding example.


int light_count, dark_count, start_time, mode;

// handles the input pulse from the IR
//cycles through 4 modes:
// mode 1 = the PC sent a prompt to get ready to capture a signal
// mode 2 = already received one pulse, now use the second pulse to find the divisor value
// mode 3 = already got divisor, now just counting num cycles in the light burst
// mode 4 = was counting the length of a dark burst

// **** This Interrupt resets TCNT1, which will cause one Compare match to not happen.******
//  This might cause problems since OCR1A is set so close to when the Input Capture occurs.
// I am not sure how to fix this.
ISR(TIM1_CAPT_vect) {
	int time = ICR1;
	TCNT1 = 0;
	switch (mode) {
		case 1:	// mode 1 = was waiting to capture signal
			signal_index = 0;	// start at beginning of signal_array
			mode++;			// start getting the divisor
			break;
		case 2:	// mode 2 = was getting divisor
			OCR1A = time + 1;	// set val to determine when pulse shouldve occurred
			OCR0A = time;		// create clock to track num periods of dark burst
			signal_array[signal_index++] = time;	// record divisor in array  		
			light_count = 2;		// two light periods have occurred so far
			mode++;				// start counting light burst length
			TCNT1 = 0;
			break;
		case 3:	// mode 3 = was counting length of light burst
			light_count++;
			break;
		case 4:	// mode 4 = was counting length of dark burst
			signal_array[signal_index++] = dark_count;
			light_count = 1;
			mode = 3;	// switch back to counting light burst length
			break;
		default:// else mode 0 = not waiting for signal, so do nothing
			break;
	}
}

// uses a compare value slightly above the carrier frequency.
// if an input capture hasn't happened by the time this fires, then 
// the light burst is done and the dark burst has started
ISR(TIM1_COMPA_vect) {
	if (light_count != 0) {	// just finished counting a light pair length
		signal_array[signal_index++] = light_count;	// put val into array
		light_count = 0;	// reset light count
		dark_count =0;		// reset dark_count to get accurate dark burst length
		mode = 4;		// set mode to counting dark burst length 
	}
}

// this interrupt fires in snyc with the carrier frequency
// it is used to keep track of the length of dark bursts
ISR(TIM0_COMPA_vect) {
	dark_count++;
}
