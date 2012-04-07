#ifndef MAGNETOMETER_H_
#define MAGNETOMETER_H_

#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include "i2c.h"
#include <math.h>
#include "../lib/data.h"

void get_magnetometer_status(){
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

double convert_raw_heading_to_degrees(int x, int y, int z){
	double norm_factor = sqrt(x*x+y*y+z*z);
	x *= norm_factor;
	y *= norm_factor;
	z *= norm_factor;
	double heading = atan2(y,x); // calculate the heading for when the compass is level
	if (heading < 0)
		heading += 2*M_PI;
	return heading*180/M_PI; // convert to degrees
}

void query_magnetometer(){
        char buffer[7];
        buffer[0] = 0x03;
        buffer[1] = '\0';
        buffer[6] = '\0';
	 // set the read pointer to the first data register
        process_i2c_bus_write(0x3C,buffer,1);
        // read in all 6 data bytes byte from 
        process_i2c_bus_read(0x3D,buffer,6);
        send_stop_condition();
        int x = (buffer[0]<<8)|buffer[1];
        int z = (buffer[2]<<8)|buffer[3];
        int y = (buffer[4]<<8)|buffer[5];

        char temp[41];
        sprintf(temp,"MagRaw: %d %d %d",x,z,y);
        transmit(temp);
	double heading = convert_raw_heading_to_degrees(x,y,z);
	int h = heading;
	int h2 = 100*(heading-h);	
	sprintf(temp,"Heading: %d.%d",h,h2);
	transmit(temp);
}

void spam_magnetometer(){
        char buffer[7];
        buffer[0] = 0x03;
        buffer[1] = '\0';
        buffer[6] = '\0';
	 // set the read pointer to the first data register
        process_i2c_bus_write(0x3C,buffer,1);
        // read in all 6 data bytes byte from 
        process_i2c_bus_read(0x3D,buffer,6);
        send_stop_condition();
        int x = (buffer[0]<<8)|buffer[1];
        int z = (buffer[2]<<8)|buffer[3];
        int y = (buffer[4]<<8)|buffer[5];

        char temp[41];
	double heading = convert_raw_heading_to_degrees(x,y,z);
	long h = 100*heading;
	sprintf(temp,"%c%c%ld",SENSORDATAPACKETCHARACTER,COMPASSHEADING,h);
	transmit(temp);	
}
#endif
