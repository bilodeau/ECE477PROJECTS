#ifndef I2CSLAVE
#define I2CSLAVE 1
#include <avr/io.h>

// if the LSB of the slave address is set, slave will respond to general call 0x00 also
#define SLAVEADDRESS 0xAA
#define SLAVEBUFFERSIZE 20

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
	TWCR = (1<<TWEA)|(1<<TWEN);  // enable acknowledgement of slave address, enable two-wire interface
	TWDR = 0xFF;
	slave_transmit_buffer[0] = 'X';
}

// handler to be called when the TWINT flag is set
void handletwi(){
	// read out from the status register
	unsigned char status = TWSR & 0xF8; // mask off the prescaler bits
	
	// SLAVE RECEIVER MODE CODES
	if (status == 0x60){
		slave_receive_buffer_index = 0;
	}else if(status == 0x68){
	}else if(status == 0x70){
	}else if(status == 0x78){
	}else if(status == 0x80){
		process_slave_receive();
	}else if(status == 0x88){
		process_slave_receive();
	}else if(status == 0x90){
	}else if(status == 0x98){
	}else if(status == 0xA0){ // STOP after receive  or repeated START
		slave_command_ready = 1;

	// SLAVE TRANSMITTER MODE CODES
	}else if(status == 0xA8){
		slave_transmit_buffer_index = 0;
		process_slave_transmit();
	}else if(status == 0xB0){
		process_slave_transmit();
	}else if(status == 0xB8){
		process_slave_transmit();
	}else if (status == 0xC0){ //Transmit finished
	}else if(status == 0xC8){ // Transmit finished (shouldn't happen)
		//do nothing
	}else if(status == 0x00){ // bus error, reset the bus
	}else if(status == 0xF8){ // miscellaneous state
		// we read TWSR without twint flag being set
	}else{
		// BAD STATUS CODE READ
	}
	
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
