#include <avr/io.h>
#include "../lib/AVRserial.h"
#include <stdio.h>
#include "motors.h"

void forward_command();

int main(){
        setup_serial();
        setup_motors();
	set_motors(0);
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
		idle_motors();
	}else if(!strcmp(receive_buffer,"FULL")){
		full_power_motors();
	}else if(!strncmp(receive_buffer,"SET ",4)){
		int power;
		sscanf(receive_buffer+4,"%d",&power);
		set_motors(power);
	}else if(!strcmp(receive_buffer,"STOP")){
		stop_motors();
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
