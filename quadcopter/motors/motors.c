#include <avr/io.h>
#include "../lib/AVRserial.h"
#include <stdio.h>

#define NORTHMOTOR 0
#define SOUTHMOTOR 1
#define EASTMOTOR 2
#define WESTMOTOR 3

// Motors are setup to use the PWM outputs of 8-bit timers 0 and 2
// NORTH is OC0A(PD6), SOUTH is OC0B(PD5)
// EAST is OC2A(PB3), WEST is OC2B(PD3)

void setup_motors();
void set_motor_power(char motor, char power);
void forward_command();

int main(){
        setup_serial();
        setup_motors();
	serial_command_ready = 0;
        while(1){
                if (serial_command_ready){
                        forward_command();
                        serial_command_ready = 0;
                }   
        }   
        return 0;
}

void forward_command(){
        if(!strcmp(receive_buffer,"IDLE")){
		set_motor_power(SOUTHMOTOR,0);
	}else if(!strcmp(receive_buffer,"FULL")){
		set_motor_power(SOUTHMOTOR,100);
	}else if(!strncmp(receive_buffer,"SET ",4)){
		int power;
		sscanf(receive_buffer+4,"%d",&power);
		if ((power >= 0 )&&(power<=100))
			set_motor_power(SOUTHMOTOR,(int)power);
	}else if(!strcmp(receive_buffer,"MAX")){
		OCR0B = 255;
	}else if(!strcmp(receive_buffer,"OFF")){
		OCR0B = 0;
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
	TCCR2B = (0<<WGM02)|4;
	
	// TOP value of FF leaves a frequency of ~122hz Hz	
	// pwm frequency needs to be 50hz to 500hz
		
	OCR0A = 32; // yields a pulse on of about 1 ms
	OCR0B = 32; // 64 yields a pulse of just over 2 ms
	OCR2A = 32;
	OCR2B = 32;

// enable output mode for all four pins	
	DDRD |= (1<<6)|(1<<5)|(1<<3);
	DDRB |= (1<<3);

}

// sets the percent power of the given motor
void set_motor_power(char motor, char power){
	switch(motor){
		case NORTHMOTOR:
			OCR0A = 32 + power/100.*32;
			break;
		case SOUTHMOTOR:
			OCR0B = 32 + power/100.*32;
			break;
		case EASTMOTOR:
			OCR2A = 32 + power/100.*32;
			break;
		case WESTMOTOR:
			OCR2B = 32 + power/100.*32;
			break;
	}
}
