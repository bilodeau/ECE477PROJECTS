#ifndef I2CSLAVE
#define I2CSLAVE 1
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "../lib/charlie.h"

// if the LSB of the slave address is set, slave will respond to general call 0x00 also
#define SLAVEADDRESS 0xAA
#define SLAVEBUFFERSIZE 20
#define LIGHTPERCODE 1

void setup_i2c_slave();
void process_slave_receive();
void process_slave_transmit();
void process_code_received();

char slave_command_ready = 0;
char slave_receive_buffer_index = 0;
char slave_receive_buffer[SLAVEBUFFERSIZE];

char slave_transmit_buffer_index = 0;
char slave_transmit_buffer[SLAVEBUFFERSIZE];

void setup_i2c_slave(){
	TWAR = SLAVEADDRESS; // set this chip's own slave address
	TWCR = (1<<TWEA)|(1<<TWEN);  // enable acknowledgement of slave address, enable two-wire interface, enable TWINT flag interrupt
	TWDR = 0xFF;
	//sei(); // enable global interrupts
	slave_transmit_buffer[0] = 4;
}

// interrupt handler for watching the TWINT flag
//ISR(TWI_vect){
void handletwi(){
	// read out from the status register
	unsigned char status = TWSR & 0xF8; // mask off the prescaler bits
	led_off();
	
	// SLAVE RECEIVER MODE CODES
	if (status == 0x60){
		if(LIGHTPERCODE)led_on_i(0);
		slave_receive_buffer_index = 0;
	}else if(status == 0x68){
		if(LIGHTPERCODE)led_on_i(1);
	}else if(status == 0x70){
		if(LIGHTPERCODE)led_on_i(2);
	}else if(status == 0x78){
		if(LIGHTPERCODE)led_on_i(3);
	}else if(status == 0x80){
		if(LIGHTPERCODE)led_on_i(4);
		process_slave_receive();
	}else if(status == 0x88){
		if(LIGHTPERCODE)led_on_i(5);
		process_slave_receive();
	}else if(status == 0x90){
		if(LIGHTPERCODE)led_on_i(6);
	}else if(status == 0x98){
		if(LIGHTPERCODE)led_on_i(7);
	}else if(status == 0xA0){ // STOP after receive  or repeated START
		if(LIGHTPERCODE)led_on_i(8);
		slave_command_ready = 1;

	// SLAVE TRANSMITTER MODE CODES
	}else if(status == 0xA8){
		if(LIGHTPERCODE)led_on_i(9);
		slave_transmit_buffer_index = 0;
		process_slave_transmit();
	}else if(status == 0xB0){
		if(LIGHTPERCODE)led_on_i(10);
		process_slave_transmit();
	}else if(status == 0xB8){
		if(LIGHTPERCODE)led_on_i(11);
		process_slave_transmit();
	}else if (status == 0xC0){ //Transmit finished
		if(LIGHTPERCODE)led_on_i(12);
	}else if(status == 0xC8){ // Transmit finished (shouldn't happen)
		if(LIGHTPERCODE)led_on_i(13);
		//do nothing
	}else if(status == 0x00){ // bus error, reset the bus
		if(LIGHTPERCODE)led_on_i(14);
	}else if(status == 0xF8){ // miscellaneous state
		if(LIGHTPERCODE) led_on_i(16);
		// we read TWSR without twint flag being set
	}else{
		if(LIGHTPERCODE)led_on_i(18);
		// BAD STATUS CODE READ
	}
	
	//led_on_i((status>>4)&0xF);
	if (TWCR&(1<<TWINT))
		TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN); // ack
}

void process_slave_receive(){
	if(slave_receive_buffer_index < SLAVEBUFFERSIZE){
		slave_receive_buffer[slave_receive_buffer_index] = TWDR;
		slave_receive_buffer_index++;
	}else{
		//ERROR
	}
}

void process_slave_transmit(){
	if (slave_transmit_buffer_index < SLAVEBUFFERSIZE){
		TWDR = slave_transmit_buffer[slave_transmit_buffer_index];
		slave_transmit_buffer_index++;
	}else{
		//ERROR
	}
}

#endif
