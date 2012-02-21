#include <avr/io.h>
#include <avr/iom8.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "charlie.h"

#define FRAMEPERIOD 1500
#define MODEPERIOD 40000

int int_counter = 0; // counter to track 'frame' updates
int mode = 0; // counter to track the current display mode

int main(void) {
	TCCR1A = 0;  // setup Timer1 control registers
		     // we don't need any output pins
		     // use WGM mode 4
		     // and clock prescalar 1/64
	TCCR1B = 0x0b;
	OCR1A = MODEPERIOD;  // this is the TOP value for the timer
	OCR1B = FRAMEPERIOD; 	// this is the shorter interrupt period
	TIMSK = 24;	// enable compare match interrupts
			// on OCR1A and OCR1B
	sei();		// enable interrupts globally
	clear();	// clear all the LED bit flags
	while(1){
		flash_leds(); // cycle over all 20 LEDS, turning each on if the appropriate flag is set
	}
	return 0;
}

// This interrupt routine is called when the timer matches OCR1A
ISR(TIMER1_COMPA_vect){
	mode++;	// increment the mode counter
	int_counter = 0; // reset the step counter to start at frame 0
}

// This interrupt routine is called when the timer matches OCR1B
ISR(TIMER1_COMPB_vect){
	OCR1B += FRAMEPERIOD;  // schedule this interrupt to occur again after the next frame period
	if (OCR1B > OCR1A) // if compare B is scheduled after the timer TOP value
		OCR1B -= OCR1A;  // reset to start over just past zero
	int_counter++;
	clear(); // clear all the LED bit flags
	mode = mode % 4;  // mod 4 since there's only 4 modes
	if (mode == 0){ // this mode flashes row by row
		if (int_counter > 4) // only 4 columns
			int_counter = 0;
		lightcolumn(int_counter);
	}else if (mode == 1){ // this mode flashes column by column
		 if (int_counter > 5) // 5 columns
			int_counter = 0;
		lightrow(int_counter);
	}else if (mode == 2){ // this mode flashes by row AND column
		if (int_counter > 4) // 4 rows
			int_counter = 0;
		rowcolpattern(int_counter);
	}else if (mode ==3){ // this mode flashes one LED at a time
		if (int_counter > 19) // 20 LEDs
			int_counter = 0;
		byindex(int_counter);
	}
}
