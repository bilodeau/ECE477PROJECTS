#include <avr/io.h>
#include "../lib/AVRserial.h"
#include <stdio.h>

#define NORTHMOTOR 0
#define SOUTHMOTOR 1
#define EASTMOTOR 2
#define WESTMOTOR 3

#define MINIMUMSIGNAL 19
#define IDLESIGNAL 22
#define MAXIMUMSIGNAL 77

// Motors are setup to use the PWM outputs of 8-bit timers 0 and 2
// NORTH is OC0A(PD6), SOUTH is OC0B(PD5)
// EAST is OC2A(PB3), WEST is OC2B(PD3)

void setup_motors();
void set_motor_power(char motor, int power);
void forward_command();

float ledmin = 1;
float ledmax = 1;

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
        if(!strcmp(receive_buffer,"LEDS")){
		ledmin = 0;
		ledmax = 255./(MAXIMUMSIGNAL - MINIMUMSIGNAL);
	}else if(!strcmp(receive_buffer,"LEDS OFF")){
		ledmin = 1;
		ledmax = 1;	
	}else if(!strcmp(receive_buffer,"IDLE")){
		set_motor_power(NORTHMOTOR,6);
		set_motor_power(SOUTHMOTOR,6);
		set_motor_power(EASTMOTOR,6);
		set_motor_power(WESTMOTOR,6);
	}else if(!strcmp(receive_buffer,"FULL")){
		set_motor_power(NORTHMOTOR,100);
		set_motor_power(SOUTHMOTOR,100);
		set_motor_power(EASTMOTOR,100);
		set_motor_power(WESTMOTOR,100);
	}else if(!strncmp(receive_buffer,"SET ",4)){
		int power;
		sscanf(receive_buffer+4,"%d",&power);
		if ((power >= 0 )&&(power<=100)){
			set_motor_power(NORTHMOTOR,(int)power);
			set_motor_power(SOUTHMOTOR,(int)power);
			set_motor_power(EASTMOTOR,(int)power);
			set_motor_power(WESTMOTOR,(int)power);
		}
	}else if(!strcmp(receive_buffer,"OFF")){
		set_motor_power(NORTHMOTOR,0);
		set_motor_power(SOUTHMOTOR,0);
		set_motor_power(EASTMOTOR,0);
		set_motor_power(WESTMOTOR,0);
	}else if(!strncmp(receive_buffer,"SET",3)){
		int power;
		sscanf(receive_buffer+4,"%d",&power);
		if(!strncmp(receive_buffer+3,"N",1)){
			set_motor_power(NORTHMOTOR,(int)power);
		}else if(!strncmp(receive_buffer+3,"S",1)){
			set_motor_power(SOUTHMOTOR,(int)power);
		}else if(!strncmp(receive_buffer+3,"E",1)){
			set_motor_power(EASTMOTOR,(int)power);
		}else if(!strncmp(receive_buffer+3,"W",1)){
			set_motor_power(WESTMOTOR,(int)power);
		}
	}else {
		transmit("Bad Command ID.");
	}
	char temp[40];
	sprintf(temp,"ocr0x= %d %d %d %d",OCR0A,OCR0B,OCR2A,OCR2B);
	transmit(temp);
	transmit("got command");
}

void setup_motors(){
// setup timer0 for north and south
// use WGM mode 3, CTC with TOP = 255,
	TCCR0A = 0xA3; // clear output pins on compare match
	TCCR0B = (0<<WGM02)|4; // use prescaler 1/256
		// clock overflows at like 122Hz
	
// setup timer2 for east and west
	TCCR2A = 0xA3; // clear output pins on compare match, counts up and down from zero to OCRA
	TCCR2B = (0<<WGM02)|6;
	
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
void set_motor_power(char motor, int p){
	float power = p;
	switch(motor){
		case NORTHMOTOR:
			OCR0A = MINIMUMSIGNAL*ledmin + ledmax*power/100.*(MAXIMUMSIGNAL-MINIMUMSIGNAL);
			break;
		case SOUTHMOTOR:
			OCR0B = MINIMUMSIGNAL*ledmin + ledmax*power/100.*(MAXIMUMSIGNAL-MINIMUMSIGNAL);
			break;
		case EASTMOTOR:
			OCR2A = MINIMUMSIGNAL*ledmin + ledmax*power/100.*(MAXIMUMSIGNAL-MINIMUMSIGNAL);
			break;
		case WESTMOTOR:
			OCR2B = MINIMUMSIGNAL*ledmin + ledmax*power/100.*(MAXIMUMSIGNAL-MINIMUMSIGNAL);
			break;
		default:
			transmit("Bad Motor ID.");
	}
}
