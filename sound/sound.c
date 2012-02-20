#include <avr/io.h>
#include <avr/iom8.h>
#include <avr/interrupt.h>
#include <stdio.h>

int step_val = 0;
char increment = 1;
char int_counter = 0;

void init_pwm(void);
int get_top_val_for_frequency(float freq);

int main(void) {
	init_pwm(); // setup timers for PWM and interrupts
	while(1){}
	return 0;
}


// setup timers for PWM
void init_pwm(void){
	DDRB = 2+4;  // enable output on OC1A and OC1B
	ICR1 = get_top_val_for_frequency(261.63);
	OCR1A = ICR1/2;
	//Output compare OC1A 8 bit non inverted PWM
	TCCR1A = 0xe0;  // set OC1A when upcounting, clear when downcounting
		      // OC1B is the inverse
	TCCR1B = 0x13;  // use WGM mode 8
		      // use internal clock with /64 prescalar
}

int get_top_val_for_frequency(float freq){
	float cpu = 1000000;	
	float prescalar = 64.;
	return (1./freq)/(prescalar/cpu);
}
