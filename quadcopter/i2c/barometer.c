#include <avr/io.h>
#include "../lib/AVRserial.h"
#include "../lib/baud.h"
#include <string.h>
#include "../lib/delay.h"
#include <stdio.h>

#define FLASHONI2C 1
#define START 0x08
#define MT_SLA_ACK 0x18
#define MT_DATA_ACK 0x28
#define REPEATED_START 0x10
#define MR_SLA_ACK 0x40
#define MR_DATA_NACK 0x58

void error(); // turns on the LED solid if an error occurs
void forward_command();
void query_barometer();
void setup_i2c();
void process_i2c_bus_read(char read_address, char* buffer, char numbytes);
void process_i2c_bus_write(char write_address, char* data, char numbytes);
void send_stop_condition();
void get_status_register();

short ac1, ac2, ac3, ac4, ac5, ac6, b1, b2, mb, mc, md; // the 11 calibration values
short temperature, pressure;     // the temp and pressure vars from the device

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
// only two commands for the barometer, query and get status.
void forward_command(){
	if (!strcmp(receive_buffer,"M")){
		query_barometer();
	}else if(!strcmp(receive_buffer,"S")){
		get_status_register();
	}
}

void get_status_register(){
	char starting_pos = 0xAA;       // 0xAA = start of calibration data
	process_i2c_bus_write(0xEE,&starting_pos,1); // 0xEE = barometer write address
	char calib_buff[22];            // eleven 16-bit words = 22 bytes
	process_i2c_bus_read(0xEF,calib_buff,22); // 0xEF = barometer read address
	send_stop_condition();

	// The data read above assumes that the pointer gets incremented after each byte is read.  The spec sheet
	// is very vague about this but, based on their examples, it should work.

    // each calib var is 16 bits. put the bytes together here.
    ac1 = (calib_buff[0]<<8)|calib_buff[1];     // (MSB<<8)|LSB
    ac2 = (calib_buff[2]<<8)|calib_buff[3];
    ac3 = (calib_buff[4]<<8)|calib_buff[5];
    ac4 = (calib_buff[6]<<8)|calib_buff[7];
    ac5 = (calib_buff[8]<<8)|calib_buff[9];
    ac6 = (calib_buff[10]<<8)|calib_buff[11];
    b1 = (calib_buff[12]<<8)|calib_buff[13];
    b2 = (calib_buff[14]<<8)|calib_buff[15];
    mb = (calib_buff[16]<<8)|calib_buff[17];
    mc = (calib_buff[18]<<8)|calib_buff[19];
    md = (calib_buff[20]<<8)|calib_buff[21];

    // now transmit back the values.
    char temp[75];
    sprintf(temp, "ac1 = %d, ac2= %d, ac3= %d", ac1, ac2, ac3);
    transmit(temp);
    sprintf(temp, "ac4= %d, ac5= %d, ac6= %d", ac4, ac5, ac6);
    transmit(temp);
    sprintf(temp, "b1= %d, b2= %d", b1, b2);
    transmit(temp);
    sprintf(temp, "mb= %d, mc= %d, md= %d", mb, mc, md);
    transmit(temp);
}

// gets the temperature and pressure data
void query_barometer(){
	char buffer[2];
	buffer[0] = 0xFA;   // address of the control register. must be set to either temp or pressure mode
	buffer[1] = 0x2E;   // control byte specifying temperature mode

	// sets the control register to temperature mode
	process_i2c_bus_write(0xEE,buffer,2);
	send_stop_condition();

	// need to wait 4.5 ms here
	delay(5);

	buffer[0] = 0xF6;       // the data register
	buffer[1] = '\0';       // clear second byte just in case
	process_i2c_bus_write(0xEE, buffer, 1);     // sets pointer to the data register now holding temperature data

	FLASHONI2C&&(DDRB = 2,PORTB = 0);
	process_i2c_bus_read(0xEF,buffer,2);        // read in the 2 data bytes byte from the data register
	FLASHONI2C&&(PORTB = 2);
	send_stop_condition();
	temperature = (buffer[0]<<8)|buffer[1];   // reassemble the 16-bit value

	// ** This only reads the temperature data once.  The spec sheet mentioned doing it twice.


    buffer[0] = 0xFA;   // address of the control register. must be set to either temp or pressure mode
	buffer[1] = 0x34;   // control byte specifying pressure mode

	process_i2c_bus_write(0xEE,buffer,2);   // sets the control register to pressure mode
	send_stop_condition();

	// need to wait 4.5 ms here
	delay(5);

	buffer[0] = 0xF6;       // the data register
	buffer[1] = '\0';       // clear second byte just in case
	process_i2c_bus_write(0xEE, buffer, 1);     // sets pointer to the data register now holding temperature data

	FLASHONI2C&&(DDRB = 2,PORTB = 0);
	process_i2c_bus_read(0xEF,buffer,2);    // read in the 2 bytes from the data register in MSB, LSB order
	FLASHONI2C&&(PORTB = 2);
	send_stop_condition();
	pressure = (buffer[0]<<8)|buffer[1];   // reassemble the 16-bit value

	// ** This only reads the pressure data once.  The spec sheet mentioned doing it twice.


	char temp[50];      // this should be a big enough buffer
	sprintf(temp,"p: %d, t: %d",pressure,temperature);
	transmit(temp);
}






// ***************** Everything below this line is identical to the functions in the magnetometer file *******************8*






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

void setup_i2c(){
	TWBR = 2; // use 400kHz
	TWSR &= ~3; // use prescaler 1
}
