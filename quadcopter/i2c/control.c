#include <avr/io.h>
#include <avr/interrupt.h>
#include "../lib/AVRserial.h"
#include <string.h>
#include "magnetometer.h"
#include "barometer.h"
#include "../lib/sonar.h"
#include "nunchuck.h"

void forward_command();
void setup_spam();
void send_spam();
char spam_flag = 0;

int main(){
	setup_spam();
	setup_delay();
	setup_serial();
        setup_i2c();
	setup_sonar();
        serial_command_ready = 0;
        while(1){
		if (serial_command_ready){
			forward_command();
                        serial_command_ready = 0;
                }
		if (spam_flag){
			send_spam();
			spam_flag = 0;
		}   
        }   
        return 0;
}

// issues the command in serial_command_buffer to the i2c bus
void forward_command(){
        if (!strcmp(receive_buffer,"MD")){
                query_magnetometer();
        }else if (!strcmp(receive_buffer,"MP")){
               power_on_magnetometer();
        }else if(!strcmp(receive_buffer,"MS")){
                get_magnetometer_status();
        }else if(!strcmp(receive_buffer,"BHC")){
	       	hard_barometer_calibration();
	}else if(!strcmp(receive_buffer,"BC")){
		get_barometer_calibration();
	}else if(!strcmp(receive_buffer,"BD")){
		query_barometer();
	}else if(!strcmp(receive_buffer,"BT")){
		query_barometer_true();
	}else if(!strcmp(receive_buffer,"SONAR")){
		query_sonar();
	}else if(!strcmp(receive_buffer,"NS")){
		get_nunchuck_status();
	}else if(!strcmp(receive_buffer,"NP")){
		power_on_nunchuck();
	}else if(!strcmp(receive_buffer,"ND")){
		query_nunchuck();
	}
}

// sets up the timer for sending data at regular intervals
void setup_spam(){
	TCCR0A = 0;
	TCCR0B = 5; // normal port mode, use prescaler 1/1024, TOP is FF, so overflows every 32.768ms
	TIMSK0 = 1; // enable overflow interrupt
	sei();	
}

ISR(TIMER0_OVF_vect){
	spam_flag = 1;
}

// queries all devices and sends appropriate data packets out to the PC
void send_spam(){
	//spam_magnetometer();
	//spam_barometer();
	spam_sonar();
	//spam_nunchuck();
}
