#include <avr/io.h>

#define NORTHMOTOR 0
#define SOUTHMOTOR 1
#define EASTMOTOR 2
#define WESTMOTOR 3

// Motors are setup to use the PWM outputs of 8-bit timers 0 and 2
// NORTH is OC0A(PD6), SOUTH is OC0B(PD5)
// EAST is OC2A(PB3), WEST is OC2B(PD3)

void setup_motors();

int main(){
	setup_motors();

	char i = 0;
	while(1){
		set_motor_power(NORTHMOTOR,i%100);
		i++;
	}	
	return 0;
}

void setup_motors(){
// setup timer0 for north and south
// use WGM mode 5, counts up and down from zero to OCRA
	TCCR0A = 0xA1; // clear output pins on compare match
	TCCR0B = (1<<WGM02)|(5); // use prescaler 1/1024 so the clock counts in units of 128 micro seconds

// setup timer2 for east and west
	TCCR2A = 0xA1; // clear output pins on compare match, counts up and down from zero to FF
	TCCR2B = (1<<WGM02)|(5);
	
	OCRA = 156; // TOP value: leaves a period of 50.08 Hz
	OC0A = 8; // yields a pulse on of about 1 millisecond
	OC0B = 8;
	OC2A = 8;
	OC2B = 8;

// enable output mode for all four pins	
	DDRD |= (1<<6)|(1<<5)|(1<<3);
	DDRB |= (1<<3);

}

// sets the percent power of the given motor
// this timer setup only lets us use 1/8 resolution, this is BAD.
void set_motor_power(char motor, char power){
	switch(motor){
		case NORTHMOTOR:
			OC0A = 8 + power/100.*8;
			break;
		case SOUTHMOTOR:
			OC0B = 8 + power/100.*8;
			break;
		case EASTMOTOR:
			OC2A = 8 + power/100.*8;
			break;
		case WESTMOTOR:
			OC2B = 8 + power/100.*8;
			break;
	}
}
