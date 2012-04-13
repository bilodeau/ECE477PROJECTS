#ifndef NUNCHUCK_H_
#define NUNCHUCK_H_
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../../i2c/i2c.h"
#include "../data.h"
#include "../delay.h"

#define NUNCHUCKZEROX 512
#define NUNCHUCKZEROY 495
#define NUNCHUCKZEROZ 521
#define NUNCHUCKMAXX 726
#define NUNCHUCKMAXY 712


char nunchuck_buffer[6];
long nunchuck_x, nunchuck_y, nunchuck_z;

// converts the current component values to yaw pitch and roll values
void get_nunchuck_angles(){
	int nx = nunchuck_x - NUNCHUCKZEROX;
	int ny = nunchuck_y - NUNCHUCKZEROY;
//	int nz = nunchuck_z - NUNCHUCKZEROZ;

	// compute angles using small-angle approximation
	long pitchf = -(float)ny / (float)(NUNCHUCKMAXY-NUNCHUCKZEROY) / M_PI * 180.;
	long rollf = -(float)nx / (float)(NUNCHUCKMAXX-NUNCHUCKZEROX) / M_PI * 180.;
	
	sensor_data_cache.nunchuck_pitch = (long)pitchf;
	sensor_data_cache.nunchuck_roll = (long)rollf;
}

void power_on_nunchuck(){
        /* old way (needs decoding)
	char init[2];
	init[0] = 0x40;
	init[1] = 0x00;
        process_i2c_bus_write(0xA4,init,2);
	send_stop_condition();
	*/
	//new way, doesn't need decoding
	char init[2];
	init[0] = 0xF0;
	init[1] = 0x55;
	process_i2c_bus_write(0xA4,init,2);
	send_stop_condition();
	delay(1);
		
	init[0] = 0xFB;
	init[1] = 0x00;
	process_i2c_bus_write(0xA4,init,2);
	send_stop_condition(); 
}

void get_nunchuck_status(){
	char buf[6];
	buf[0] = 0xFA;
	process_i2c_bus_write(0xA4,buf,1);
	process_i2c_bus_read(0xA5,buf,6);
	send_stop_condition();
	char temp[40];
	sprintf(temp,"NS: %x%x%x%x%x%x",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);
	transmit(temp);
}

void get_forty_nunchuck(){
	char b = 0x40;
	process_i2c_bus_write(0xA4,&b,1);
	process_i2c_bus_read(0xA5,&b,1);
	send_stop_condition();
	char temp[40];
	sprintf(temp,"control byte is: %x", b);
	transmit(temp);
}

void send_zero_nunchuck(){
	char b = 0;
	process_i2c_bus_write(0xA4,&b,1);
	send_stop_condition();
}

void decode_nunchuck_data(){
	nunchuck_x = (nunchuck_x ^ 0x17) + 0x17;
	nunchuck_y = (nunchuck_y ^ 0x17) + 0x17;
	nunchuck_z = (nunchuck_z ^ 0x17) + 0x17;
}

void get_data_nunchuck(){
        process_i2c_bus_read(0xA5,nunchuck_buffer,6); 
	send_stop_condition();
	
	nunchuck_x = (nunchuck_buffer[2]);
	nunchuck_x <<= 2;
	nunchuck_x |= ((nunchuck_buffer[5]>>2)&(3));
	nunchuck_x &= 0x3FF;

	nunchuck_y = (nunchuck_buffer[3]);
	nunchuck_y <<= 2;
	nunchuck_y |= ((nunchuck_buffer[5]>>4)&(3));
	nunchuck_y &= 0x3FF;

	nunchuck_z = (nunchuck_buffer[4]);
	nunchuck_z <<= 2;
	nunchuck_z |= ((nunchuck_buffer[5]>>6)&(3));
	nunchuck_z &= 0x3FF;
	
	send_zero_nunchuck();
	
	//decode_nunchuck_data(); // not needed since we're using the new initialization

	sensor_data_cache.nunchuck_x_value = nunchuck_x;
	sensor_data_cache.nunchuck_y_value = nunchuck_y;
	sensor_data_cache.nunchuck_z_value = nunchuck_z;
}

void query_nunchuck(){
	get_data_nunchuck();
	char temp[41];
	sprintf(temp,"X: %ld Y: %ld Z: %ld",nunchuck_x,nunchuck_y,nunchuck_z);
	transmit(temp);
}
#endif
