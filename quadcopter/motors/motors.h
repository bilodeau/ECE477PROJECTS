#ifndef MOTORS_H_
#define MOTORS_H_

#include "../control/controller.h"

#define NORTHMOTOR 0
#define SOUTHMOTOR 1
#define EASTMOTOR 2
#define WESTMOTOR 3

#define MINIMUMSIGNAL 19
#define IDLESIGNAL 22
#define MAXIMUMSIGNAL 77

#define PRINTTHRUST 1

// Motors are setup to use the PWM outputs of 8-bit timers 0 and 2
// NORTH is OC0A(PD6), SOUTH is OC0B(PD5)
// EAST is OC2A(PB3), WEST is OC2B(PD3)
int north_thrust = 0, south_thrust = 0,east_thrust = 0, west_thrust = 0;
void set_motor_power(char motor, int power);
void stop_motors(){
	set_motor_power(NORTHMOTOR,0);
	set_motor_power(SOUTHMOTOR,0);
	set_motor_power(EASTMOTOR,0);
	set_motor_power(WESTMOTOR,0);
}

void set_motors(char power){
	if ((power >= 0) && (power <= 100)){
		set_motor_power(NORTHMOTOR,power);
		set_motor_power(SOUTHMOTOR,power);
		set_motor_power(EASTMOTOR,power);
		set_motor_power(WESTMOTOR,power);
	}
}

void setup_motors(){
// setup timer0 for north and south
// use WGM mode 3, CTC with TOP = 255,
	TCCR0A = 0xA3; // clear output pins on compare match
	TCCR0B = (0<<WGM02)|4; // use prescaler 1/256
		// clock overflows at like 122Hz
	
// setup timer2 for east and west
	TCCR2A = 0xA3; // clear output pins on compare match, counts up and down from zero to OCRA
	TCCR2B = (0<<WGM02)|6; // NOTE: the prescaler values for timer2 use a different table, that's why this is 6 not 4
	
	
	// TOP value of FF leaves a frequency of ~122hz Hz	
	// pwm frequency needs to be 50hz to 500hz
		
	OCR0A = 0; // yields a pulse on of about 1 ms
	OCR0B = 0; // 64 yields a pulse of just over 2 ms
	OCR2A = 0;
	OCR2B = 0;

// enable output mode for all four pins	
	DDRD |= (1<<6)|(1<<5)|(1<<3);
	DDRB |= (1<<3);

}

// sets the percent power of the given motor
void set_motor_power(char motor, int power){
	switch(motor){
		case NORTHMOTOR:
			OCR0A = MINIMUMSIGNAL + power/100.*(MAXIMUMSIGNAL-MINIMUMSIGNAL);
			break;
		case SOUTHMOTOR:
			OCR0B = MINIMUMSIGNAL + power/100.*(MAXIMUMSIGNAL-MINIMUMSIGNAL);
			break;
		case EASTMOTOR:
			OCR2A = MINIMUMSIGNAL + power/100.*(MAXIMUMSIGNAL-MINIMUMSIGNAL);
			break;
		case WESTMOTOR:
			OCR2B = MINIMUMSIGNAL + power/100.*(MAXIMUMSIGNAL-MINIMUMSIGNAL);
			break;
	}
}

void update_motors(){
/*
	int base_thrust = 10;
	int max_thrust = 50;

	int mpitch = pitch;
	int mroll = roll;
	
	if (mpitch>45) mpitch = 45;
	if (mpitch<-45) mpitch = -45;
	if (mroll>45) mroll = 45;
	if (mroll<-45) mroll = -45;

	float roll_factor = (90.-mroll)/180.;
	west_thrust = base_thrust + (max_thrust-base_thrust)*(1-roll_factor);
	east_thrust = base_thrust + (max_thrust-base_thrust)*(roll_factor);
	char temp[40];
	if(PRINTTHRUST){
		sprintf(temp,"east: %d west: %d",east_thrust,west_thrust);
		transmit(temp);
	}
	
	float pitch_factor = (90.-mpitch)/180.;
	south_thrust = base_thrust + (max_thrust-base_thrust)*(1-pitch_factor);
	north_thrust = base_thrust + (max_thrust-base_thrust)*(pitch_factor);
	if(PRINTTHRUST){
		sprintf(temp,"north: %d south: %d",north_thrust,south_thrust);
		transmit(temp);
	}

	if(north_thrust > max_thrust)
		north_thrust = max_thrust;
	if(south_thrust > max_thrust)
		south_thrust = max_thrust;
	if(east_thrust > max_thrust)
		east_thrust = max_thrust;
	if(west_thrust > max_thrust)
		west_thrust = max_thrust;
	
	if(north_thrust < base_thrust)
		north_thrust = base_thrust;
	if(south_thrust < base_thrust)
		south_thrust = base_thrust;
	if(east_thrust < base_thrust)
		east_thrust = base_thrust;
	if(west_thrust < base_thrust)
		west_thrust = base_thrust;
*/
	compute_controller();
	set_motor_power(NORTHMOTOR,get_north_thrust());
	set_motor_power(SOUTHMOTOR,get_south_thrust());
	set_motor_power(EASTMOTOR,get_east_thrust());
	set_motor_power(WESTMOTOR,get_west_thrust());
}
#endif
