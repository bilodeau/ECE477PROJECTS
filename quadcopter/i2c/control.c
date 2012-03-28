#include <avr/io.h>
#include <avr/interrupt.h>
#include "../lib/AVRserial.h"
#include <string.h>
#include "magnetometer.h"
#include "barometer.h"
#include "nunchuck.h"
#include "gyro.h"

void query_slave(char sensor_address, char numbytes);
void forward_command();
void setup_spam();
void send_spam();
char spam_flag = 0;

int main(){
	setup_spam();
	setup_delay();
	setup_serial();
        setup_i2c();
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

void query_slave(char sensor_address, char numbytes){
	
	char b[2];
	b[0] = 12;//sensor_address;
	b[1] = 0;
	process_i2c_bus_write(0xAA,b,1);
	send_stop_condition();
	transmit("done writing to slave");
	delay(10);
	
	char buf[20];
	process_i2c_bus_read(0xAA+1,buf,numbytes);
	send_stop_condition();
	buf[numbytes] = '\0';
	char temp[20];
	sprintf(temp,"hex: %x %x %x %x %x", buf[0],buf[1],buf[2],buf[3],buf[4]);
	transmit(temp);
	sprintf(temp,"string: %s",buf);
	transmit(temp);
}

void forward_command(){
	if(!strcmp(receive_buffer,"GP")){
		power_on_gyro();
	}else if(!strcmp(receive_buffer,"GD")){
		query_gyro();	
	}else if(!strcmp(receive_buffer,"MD")){
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
		query_slave(0x00,5);//sizeof(double));
	}else if(!strcmp(receive_buffer,"NS")){
		get_nunchuck_status();
	}else if(!strcmp(receive_buffer,"NP")){
		power_on_nunchuck();
	}else if(!strcmp(receive_buffer,"ND")){
		query_nunchuck();
	}else if(!strcmp(receive_buffer,"NC")){
		get_forty_nunchuck();
	}
}

// sets up Timer1 for sending data at regular intervals
void setup_spam(){
	TCCR1A = 0; // use WGM mode 4, CTC, OCR1A is TOP
	TCCR1B = 10; // normal port operation, use prescaler 1/8, counts in us
	OCR1A = 0x7FFF;// TOP value, so overflows every 32.768ms
	TIMSK1 = 2; // enable overflow interrupt
	sei();	
}

ISR(TIMER1_COMPA_vect){
	spam_flag = 1;
}

// queries all devices and sends appropriate data packets out to the PC
void send_spam(){
	spam_magnetometer();
	spam_barometer();
	spam_nunchuck();
	spam_gyro();
}
