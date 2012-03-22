#ifndef I2CSLAVE
#define I2CSLAVE 1
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

// if the LSB of the slave address is set, slave will respond to general call 0x00 also
#define SLAVEADDRESS 0xAA

void setup_i2c_slave();
void process_slave_receive();
void process_slave_transmit();

char slave_receive_buffer_index;
char slave_receive_buffer[20];

char slave_transmit_buffer_index;
char slave_transmit_buffer[20];

void setup_i2c_slave(){
	TWAR = SLAVEADDRESS; // set this chip's own slave address
	TWCR = (1<<TWEA)|(1<<TWEN)|(1<<TWIE);  // enable acknowledgement of slave address, enable two-wire interface, enable TWINT flag interrupt
	sei(); // enable global interrupts
}

// interrupt handler for watching the TWINT flag
ISR(TWI_vect){
	TWSR |= (1<<TWINT); // have to set the bit to clear the flag
	// read out from the status register
	char status = TWSR & 0xF8; // mask off the prescaler bits
	if (status == 0x60){
		slave_receive_buffer_index = 0;
	}else if(){
		process_slave_receive();
	}else if (status == 0xA8){
		process_slave_transmit();
	}else{
		// BAD STATUS CODE READ
	}
}

void process_slave_receive(){
	// switch to slave receiver mode
	slave_receive_buffer_index = 0;
	while((TWSR & 0xF8) != 0xA0){
		while();
		slave_receive_buffer[slave_receive_buffer_index] = TWDR;
		slave_receive_buffer_index++;
	}
	
	// first byte is the sensor code
	// second byte is set or get
	// third (optional) byte is the high byte of the value to set
	// fourth (optional) byte is the low byte of the value to set
}

void process_slave_transmit(){
	// switch to slave transmitter mode
}

#endif
