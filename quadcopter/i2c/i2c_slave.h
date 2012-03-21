#ifndef I2CSLAVE
#define I2CSLAVE 1
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

// if the LSB of the slave address is set, slave will respond to general call 0x00 also
#define SLAVEADDRESS 0xAA

void setup_i2c_slave();
void process_slave_write();
void process_slave_read();

void setup_i2c_slave(){
	TWAR = SLAVEADDRESS; // set this chip's own slave address
	TWCR = (1<<TWEA)|(1<<TWEN)|(1<<TWIE);  // enable acknowledgement of slave address, enable two-wire interface, enable TWINT flag interrupt
	sei(); // enable global interrupts
}

// interrupt handler for watching the TWINT flag
ISR(TWI_vect){
	TWSR &= ~(1<<TWINT); 
	// read out from the status register
	char status = TWSR&(~3); // mask off the prescaler bits
	if (status = ){
		process_slave_write();
	}else if (status = ){
		process_slave_read();
	}	
}

void process_slave_write(){
	// switch to slave receiver mode
}

void process_slave_read(){
	// switch to slave transmitter mode
}

#endif
