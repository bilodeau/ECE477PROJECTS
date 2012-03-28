#include <avr/io.h>
#include <stdio.h>
#include "i2c_slave.h"
//#include "../lib/sonar.h"

void process_command_received();
void load_sonar_packet();
void load_gps_packet();

int main(){
//	setup_sonar();
	setup_i2c_slave();
	for(;;){ // loop forever (most efficient declaration of an infinite loop)
		if(TWCR&(1<<TWINT))
			handletwi();
		if(slave_command_ready)
			process_command_received();
	}
}

// Commands to the slave chip are formatted as follows:
// slave is addressed using 0xAA
// first byte of each command indicates which sensor the next read will come from

void process_command_received(){
	if (slave_receive_buffer[0] == 0x00){ // sonar
		load_sonar_packet();
	}else if(slave_receive_buffer[0] == 0x01){ // GPS
		load_gps_packet();
	}else{
		// not a valid command
	}
}

void load_sonar_packet(){ // X bytes
	/*
	char i;
	char *ptr =(char *) &sonar_distance;
	for(i=0; i<sizeof(double);i++)
		slave_transmit_buffer[i] = ptr[i];
*/
	sprintf(slave_transmit_buffer,"TEST.");
}

void load_gps_packet(){
	char i;
	char ptr[] = "GPS Packet";
	for(i=0; i<10;i++)
		slave_transmit_buffer[i] = ptr[i];
}
