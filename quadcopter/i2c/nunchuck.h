#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include "i2c.h"
#include "../lib/data.h"
#include <math.h>

long nunchuck_x, nunchuck_y, nunchuck_z;
long yaw,pitch,roll;  // all values in degrees
int nunchuck_zero_x = 510;
int nunchuck_zero_y = 490;
int nunchuck_zero_z = 460;

// converts the current component values to yaw pitch and roll values
void nunchuck_to_degrees(){
	float inverse_length = 1./sqrtf(nunchuck_x*nunchuck_x + nunchuck_y*nunchuck_y + nunchuck_z*nunchuck_z); // compute the normalization factor

	// normalize all of the components (so that the vector is normalized)	
	float x = nunchuck_x * inverse_length;
	float y = nunchuck_y * inverse_length;
	float z = nunchuck_z * inverse_length;

	float pitchf = acos(y)/M_PI*180 - 90; // minus 90 to rotate
	float rollf = -atan2(x,z)/M_PI*180;
	float yawf = z;//0;

//	char temp[100];
//	sprintf(temp,"x is now: %d",(int)(pitchf*100));
//	transmit(temp);
		
	// adjust to within the range (-180 , 180]
	// yaw is [0,360)

	yaw = (long)(yawf);  // multiply by 100 since we're storing them as longs
	pitch = (long)(pitchf);
	roll = (long)(rollf);
}

void zero_nunchuck(){
	nunchuck_zero_x = nunchuck_x;
	nunchuck_zero_y = nunchuck_y;
	nunchuck_zero_z = nunchuck_z;
}

void power_on_nunchuck(){
        char init[2];
	init[0] = 0x40;
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
	nunchuck_x ^= 0x17;
	nunchuck_x += 0x17;
	nunchuck_y ^= 0x17;
	nunchuck_y += 0x17;
	nunchuck_z ^= 0x17;
	nunchuck_z += 0x17;
}

void get_data_nunchuck(){
	char buffer[6];
        process_i2c_bus_read(0xA5,buffer,6); 
	send_stop_condition();
	
	decode_nunchuck_data();
	
	nunchuck_x = (buffer[2]);
	nunchuck_x <<= 2;
	nunchuck_x |= ((buffer[5]>>2)&(3));
	nunchuck_x &= 0x3FF;

	nunchuck_y = (buffer[3]);
	nunchuck_y <<= 2;
	nunchuck_y |= ((buffer[5]>>4)&(3));
	nunchuck_y &= 0x3FF;

	nunchuck_z = (buffer[4]);
	nunchuck_z <<= 2;
	nunchuck_z |= ((buffer[5]>>6)&(3));
	nunchuck_z &= 0x3FF;
	
	nunchuck_x -= nunchuck_zero_x;
	nunchuck_y -= nunchuck_zero_y;
	nunchuck_z -= nunchuck_zero_z;
	send_zero_nunchuck();
}

void query_nunchuck(){
	get_data_nunchuck();
	char temp[41];
	sprintf(temp,"X: %ld Y: %ld Z: %ld",nunchuck_x,nunchuck_y,nunchuck_z);
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

void spam_nunchuck_angles(){
        get_data_nunchuck();	
	nunchuck_to_degrees();
	char temp[41];
	sprintf(temp,"%c%c%ld",SENSORDATAPACKETCHARACTER,YAW,yaw);
	transmit(temp);	
	sprintf(temp,"%c%c%ld",SENSORDATAPACKETCHARACTER,PITCH,pitch);
	transmit(temp);	
	sprintf(temp,"%c%c%ld",SENSORDATAPACKETCHARACTER,ROLL,roll);
	transmit(temp);	
}
