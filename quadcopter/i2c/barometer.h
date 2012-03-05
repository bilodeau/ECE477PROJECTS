#include <avr/io.h>
#include <string.h>
#include "../lib/delay.h"
#include <stdio.h>
#include "i2c.h"

void query_barometer();
void get_barometer_calibration();
void query_barometer_true();

short ac1, ac2, ac3, b1, b2, mb, mc, md; // the 11 calibration values
unsigned short ac4,ac5,ac6;
long temperature, pressure;     // the temp and pressure vars from the device

void get_barometer_calibration(){
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
    sprintf(temp, "ac4= %u, ac5= %u, ac6= %u", ac4, ac5, ac6);
    transmit(temp);
    sprintf(temp, "b1= %d, b2= %d", b1, b2);
    transmit(temp);
    sprintf(temp, "mb= %d, mc= %d, md= %d", mb, mc, md);
    transmit(temp);
}

// gets the temperature and pressure data
void query_barometer(){
	char buffer[2];
	char count;
	for (count=0;count<1;count++){
	buffer[0] = 0xF4;   // address of the control register. must be set to either temp or pressure mode
	buffer[1] = 0x2E;   // control byte specifying temperature mode

	// sets the control register to temperature mode
	process_i2c_bus_write(0xEE,buffer,2);
	send_stop_condition();

	// need to wait 4.5 ms here
	//char time;
	//for (time =0; time <10;time++)
		delay(15);

	buffer[0] = 0xF6;       // the data register
	buffer[1] = '\0';       // clear second byte just in case
	process_i2c_bus_write(0xEE, buffer, 1);     // sets pointer to the data register now holding temperature data

	process_i2c_bus_read(0xEF,buffer,2);        // read in the 2 data bytes byte from the data register
	send_stop_condition();
	temperature = (buffer[0]<<8)|buffer[1];   // reassemble the 16-bit value
	} // READS TWICE
	// ** This only reads the temperature data once.  The spec sheet mentioned doing it twice.


	for (count=0;count<1;count++){
    buffer[0] = 0xF4;   // address of the control register. must be set to either temp or pressure mode
	buffer[1] = 0x34;   // control byte specifying pressure mode

	process_i2c_bus_write(0xEE,buffer,2);   // sets the control register to pressure mode
	send_stop_condition();

	// need to wait 4.5 ms here
	//char time;
	//for (time =0; time <10;time++)
		delay(15);

	buffer[0] = 0xF6;       // the data register
	buffer[1] = '\0';       // clear second byte just in case
	process_i2c_bus_write(0xEE, buffer, 1);     // sets pointer to the data register now holding temperature data

	process_i2c_bus_read(0xEF,buffer,2);    // read in the 2 bytes from the data register in MSB, LSB order
	send_stop_condition();
	pressure = (buffer[0]<<8)|buffer[1];   // reassemble the 16-bit value
}// READS TWICE
	// ** This only reads the pressure data once.  The spec sheet mentioned doing it twice.


	char temp[50];      // this should be a big enough buffer
	sprintf(temp,"p: %ld, t: %ld",pressure,temperature);
	transmit(temp);
}

void query_barometer_true() {
	long ut;
	long up;
	long x1, x2, b5, b6, x3, b3, p;
	unsigned long b4, b7;
	
	ut = temperature;
	up = pressure;
	
	x1 = ((long)ut - ac6) * ac5 >> 15;
	x2 = ((long) mc << 11) / (x1 + md);
	b5 = x1 + x2;
	temperature = (b5 + 8) >> 4;
	
	b6 = b5 - 4000;
	x1 = (b2 * (b6 * b6 >> 12)) >> 11;
	x2 = ac2 * b6 >> 11;
	x3 = x1 + x2;
	b3 = (((long) ac1 * 4 + x3) + 2)/4;
	x1 = ac3 * b6 >> 13;
	x2 = (b1 * (b6 * b6 >> 12)) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	b4 = (ac4 * (unsigned long) (x3 + 32768)) >> 15;
	b7 = ((unsigned long) up - b3) * (50000 >> 0);
	p = b7 < 0x80000000 ? (b7 * 2) / b4 : (b7 / b4) * 2;
	x1 = (p >> 8) * (p >> 8);
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p) >> 16;
	pressure = p + ((x1 + x2 + 3791) >> 4);
	
	char temp[75];      // this should be a big enough buffer
	sprintf(temp,"true_p: %ld, true_t: %ld",pressure,temperature);
	transmit(temp);
	
}
