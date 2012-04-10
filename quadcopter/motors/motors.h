#ifndef MOTORS_H_
#define MOTORS_H_

#define NORTHMOTOR 0
#define SOUTHMOTOR 1
#define EASTMOTOR 2
#define WESTMOTOR 3

#define MINIMUMSIGNAL 78
#define IDLESIGNAL 90
#define MAXIMUMSIGNAL 254

// Motors are setup to use the PWM outputs of 8-bit timers 0 and 2
// NORTH is OC0A(PD6), SOUTH is OC0B(PD5)
// EAST is OC2A(PB3), WEST is OC2B(PD3)
void set_motor_power(char motor, int power);
void set_motors(int power);

void stop_motors(){
	set_motors(MINIMUMSIGNAL);
}

void idle_motors(){
	set_motors(IDLESIGNAL);
}

void full_power_motors(){
	set_motors(MAXIMUMSIGNAL);
}

void set_motors(int power){
	set_motor_power(NORTHMOTOR,power);
	set_motor_power(SOUTHMOTOR,power);
	set_motor_power(EASTMOTOR,power);
	set_motor_power(WESTMOTOR,power);
}

void setup_motors(){
// setup timer0 for north and south
// use WGM mode 3, with TOP = 255,
	TCCR0A = 0xA3; // clear output pins on compare match
	TCCR0B = (0<<WGM02)|3; // use prescaler 1/64
		// clock overflows at 488Hz
	
// setup timer2 for east and west
	TCCR2A = 0xA3; // clear output pins on compare match, counts up and down from zero to OCRA
	TCCR2B = (0<<WGM02)|4; // NOTE: the prescaler values for timer2 use a different table, that's why this is 4 not 3
	
	// TOP value of FF leaves a frequency of ~488hz Hz	
	// pwm frequency needs to be 50hz to 500hz
		
	OCR0A = MINIMUMSIGNAL;
	OCR0B = MINIMUMSIGNAL;
	OCR2A = MINIMUMSIGNAL;
	OCR2B = MINIMUMSIGNAL;

// enable output mode for all four pins	
	DDRD |= (1<<6)|(1<<5)|(1<<3);
	DDRB |= (1<<3);

}

// sets the power of the given motor, power values must be in the range 78-254 inclusive
void set_motor_power(char motor, int power){
	if(power > MAXIMUMSIGNAL){
		power  = MAXIMUMSIGNAL;
	}else if(power < MINIMUMSIGNAL){
		power = MINIMUMSIGNAL;
	}
	switch(motor){
		case NORTHMOTOR:
			OCR0A = power;
			break;
		case SOUTHMOTOR:
			OCR0B = power;
			break;
		case EASTMOTOR:
			OCR2A = power;
			break;
		case WESTMOTOR:
			OCR2B = power;
			break;
	}
}
#endif
