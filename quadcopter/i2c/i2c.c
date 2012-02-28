#include <avr/io.h>
#include "../AVRserial.h"
#include "../baud.h"
#include <string.h>

#define FLASHONI2C 1
#define START 0x08
#define MT_SLA_ACK 0x18
#define MT_DATA_ACK 0x28
#define REPEATED_START 0x10
#define MR_SLA_ACK 0x40
#define MR_DATA_NACK 0x58

void error(); // turns on the LED solid if an error occurs
void forward_command();
void query_magnetometer();
void setup_i2c();
void process_i2c_bus_read(char read_address, char* buffer, char numbytes);
void process_i2c_bus_write(char write_address, char* data, char numbytes);
void send_stop_condition();
void power_on_magnetometer();
void get_status_register();

int main(){
	setup_serial();
	setup_i2c();
	serial_command_ready = 0;
	while(1){
		if (serial_command_ready){
			forward_command();
			serial_command_ready = 0;
		}
	}
	return 0;
}

// issues the command in serial_command_buffer to the i2c bus
void forward_command(){
	if (!strcmp(receive_buffer,"M")){
		query_magnetometer();
	}else if (!strcmp(receive_buffer,"P")){
		power_on_magnetometer();
	}else if(!strcmp(receive_buffer,"S")){
		get_status_register();
	}
}

void get_status_register(){
	char buffer = 0x09;
	process_i2c_bus_write(0x3C,&buffer,1);
	process_i2c_bus_read(0x3D,&buffer,1);
	send_stop_condition();
	char temp [41];
	sprintf(temp,"Status register is: %d",buffer);
	transmit(temp);
}

void power_on_magnetometer(){
	char init[2] = {0x00,0x70};
	process_i2c_bus_write(0x3C,init,2);
	init[0] = 0x01;
	init[1] = 0xA0;
	process_i2c_bus_write(0x3C,init,2);
	init[0] = 0x02;
	init[1] = 0x00;
	process_i2c_bus_write(0x3C,init,2);
	send_stop_condition();
}

void query_magnetometer(){
	char buffer[7];
	buffer[0] = 0x03;
	buffer[1] = '\0';
	buffer[6] = '\0';
	// set the read pointer to the first data register
	process_i2c_bus_write(0x3C,buffer,1);
	// read in all 6 data bytes byte from 
	FLASHONI2C&&(DDRB = 2,PORTB = 0);
	process_i2c_bus_read(0x3D,buffer,6);
	FLASHONI2C&&(PORTB = 2);
	send_stop_condition();
	int x = (buffer[0]<<8)|buffer[1];
	int z = (buffer[2]<<8)|buffer[3];
	int y = (buffer[4]<<8)|buffer[5];

	char temp[41];
	sprintf(temp,"Mag: %d %d %d",x,z,y);
	transmit(temp);
}

void setup_i2c(){
	TWBR = 2; // use 400kHz
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

void error(){
	DDRB = 2;
	PORTB = 0;
	while(1);
}
