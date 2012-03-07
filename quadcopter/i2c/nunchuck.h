#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include "i2c.h"
#include "../lib/data.h"

long nunchuck_x, nunchuck_y, nunchuck_z;
long button1, button2;

void power_on_nunchuck(){
        char init[2] = {0x40,0x00};
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

void get_data_nunchuck(){
        char buffer[6];
	buffer[0] = 0x00;
	process_i2c_bus_write(0xA4,buffer,1);
        process_i2c_bus_read(0xA5,buffer,6); 
	send_stop_condition();
	
	nunchuck_x = (buffer[2]<<2)|((buffer[5]>>2)&(3));
	nunchuck_y = (buffer[3]<<2)|((buffer[5]>>4)&(3));
	nunchuck_z = (buffer[4]<<2)|((buffer[5]>>6)&(3));
	button1 = buffer[5]&1;
	button2 = (buffer[5]>>1)&1;
}

void query_nunchuck(){
	get_data_nunchuck();
	char temp[41];
	sprintf(temp,"X: %ld Y: %ld Z: %ld",nunchuck_x,nunchuck_y,nunchuck_z);
	transmit(temp);
	sprintf(temp,"Button1: %ld Button2: %ld",button1,button2);
	transmit(temp);
}

void spam_nunchuck(){
        get_data_nunchuck();	
	char temp[41];
	sprintf(temp,"%c%c%ld",SENSORDATAPACKETCHARACTER,NUNCHUCKX,nunchuck_x);
	transmit(temp);	
	sprintf(temp,"%c%c%ld",SENSORDATAPACKETCHARACTER,NUNCHUCKY,nunchuck_y);
	transmit(temp);	
	sprintf(temp,"%c%c%ld",SENSORDATAPACKETCHARACTER,NUNCHUCKZ,nunchuck_z);
	transmit(temp);	
}
