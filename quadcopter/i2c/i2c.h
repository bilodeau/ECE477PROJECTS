#ifndef I2C_H_
#define I2C_H_
#include <avr/io.h>
#include <string.h>
#include "../lib/data.h"

#define FLASHONI2C  0
#define START 0x08
#define MT_SLA_ACK 0x18
#define MT_DATA_ACK 0x28
#define REPEATED_START 0x10
#define MR_SLA_ACK 0x40
#define MR_DATA_NACK 0x58

void setup_i2c();
void process_i2c_bus_read(char read_address, char* buffer, char numbytes);
void process_i2c_bus_write(char write_address, char* data, char numbytes);
void send_stop_condition();

void query_slave(char sensor_address, char numbytes){
 
        char b[2];
        b[0] = sensor_address;
        b[1] = 0;
        process_i2c_bus_write(0xAA,b,1);
        //delay is not necessary since the i2c clock is much much slower than cpu clock
        char buf[20];
        process_i2c_bus_read(0xAA+1,buf,numbytes);
        send_stop_condition();
        sensor_data_cache.sonar_distance = (*((double *)buf)*100);
}

void setup_i2c(){
	TWBR = 32; // use 100kHz
	TWSR &= ~3; // use prescaler 1
}

void send_stop_condition(){
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
}

// sends the given command over the bus the specified data, reading the result into the buffer
void process_i2c_bus_write(char write_address, char* data,char numbytes){
	// send start bit
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	
	// wait for TWINT Flag set
	while(!(TWCR & (1<<TWINT)));

	// check value of TWI status register.  The 0xF8 masks the prescalar bits
	if (((TWSR & 0xF8) != START)&&((TWSR & 0xF8) != REPEATED_START)){
		char temp[21];
		sprintf(temp,"bad start condition");
		transmit(temp);
	}

	// load the write address into the data register
	TWDR = write_address;
	TWCR = (1<<TWINT)|(1<<TWEN); // start transmission by..

	// wait for TWINT Flag set (means the address was transmitted OK and we got an ACK/NACK
	while (!(TWCR & (1<<TWINT)));
	
	// check TWI status register
	if ((TWSR & 0xF8) != MT_SLA_ACK){
		char temp[21];
		sprintf(temp,"address transmit failed");
		transmit(temp);
	}
	
	char i;
	for (i=0;i<numbytes;i++){
		TWDR = data[i];
		TWCR = (1<<TWINT)|(1<<TWEN); // start transmission
		
		while(!(TWCR & (1<<TWINT))); // wait for send complete
		if ((TWSR & 0xF8) != MT_DATA_ACK){
			char temp[21];
			sprintf(temp,"data transmit failed");
			transmit(temp);
		}
	}	
}

// sends reads from the bus the given number of bytes into the specified buffer
void process_i2c_bus_read(char read_address,char* buffer, char numbytes){
	// send repeated start condition
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	
	// wait for TWINT Flag set
	while(!(TWCR & (1<<TWINT)));

	// load the read address into the data register
	TWDR = read_address;
	TWCR = (1<<TWINT)|(1<<TWEN); // start transmission by..
	
	// wait for TWINT Flag set (means the address was transmitted OK and we got an ACK/NACK
	while (!(TWCR & (1<<TWINT)));
		
	// check TWI status register
	if ((TWSR & 0xF8) != MR_SLA_ACK){
		char temp[21];
		sprintf(temp,"read address send failed");
		transmit(temp);
	}
	char i;
	for (i = 0; i < numbytes-1; i++){	
		TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN);// start reception
		while (!(TWCR & (1<<TWINT)));
		buffer[i] = TWDR;
	}
	TWCR = (1<<TWINT)|(1<<TWEN); // start reception, but end with a NACK
	while(!(TWCR & (1<<TWINT)));
	buffer[i] = TWDR;
}
#endif
