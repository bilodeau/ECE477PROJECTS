#include <avr/io.h>

#define NORTHMOTOR 0
#define SOUTHMOTOR 1
#define EASTMOTOR 2
#define WESTMOTOR 3

// Motors are setup to use the PWM outputs of 8-bit timers 0 and 2
// NORTH is OC0A(PD6), SOUTH is OC0B(PD5)
// EAST is OC2A(PB3), WEST is OC2B(PD3)

void setup_motors();
void set_motor_power(char motor, char power);

int main(){
	setup_motors();

	while(1){
			set_motor_power(SOUTHMOTOR,10);
	}	
	return 0;
}

void setup_motors(){
// setup timer0 for north and south
// use WGM mode 5, counts up and down from zero to OCRA
	TCCR0A = 0xA1; // clear output pins on compare match
	TCCR0B = (1<<WGM02)|(5); // use prescaler 1/1024 so the clock counts in units of 128 micro seconds

// setup timer2 for east and west
	TCCR2A = 0xA1; // clear output pins on compare match, counts up and down from zero to OCRA
	TCCR2B = (1<<WGM02)|5;
	
	// TOP value of FF leaves a frequency of ~30hz Hz	
	// pwm frequency needs to be 50hz to 500hz
		
	OCR0A = 156; // yields a pulse on of about 1 millisecond
	OCR0B = 8;
	OCR2A = 156;
	OCR2B = 8;

// enable output mode for all four pins	
	DDRD |= (1<<6)|(1<<5)|(1<<3);
	DDRB |= (1<<3);

}

// sets the percent power of the given motor
// this timer setup only lets us use 1/8 resolution, this is BAD.
void set_motor_power(char motor, char power){
	switch(motor){
		case NORTHMOTOR:
			OCR0A = 8 + power/100.*8;
			break;
		case SOUTHMOTOR:
			OCR0B = 8 + power/100.*8;
			break;
		case EASTMOTOR:
			OCR2A = 8 + power/100.*8;
			break;
		case WESTMOTOR:
			OCR2B = 8 + power/100.*8;
			break;
	}
}
