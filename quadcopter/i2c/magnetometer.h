#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include "i2c.h"

void get_status_register(){
        char buffer = 0x09;
        DDRB=2,PORTB=0;
	process_i2c_bus_write(0x3C,&buffer,1);
        process_i2c_bus_read(0x3D,&buffer,1);
        send_stop_condition();
        char temp [41];
        sprintf(temp,"Status register is: %d",buffer);
        transmit(temp);
	PORTB=2;
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
        process_i2c_bus_read(0x3D,buffer,6);
        send_stop_condition();
        int x = (buffer[0]<<8)|buffer[1];
        int z = (buffer[2]<<8)|buffer[3];
        int y = (buffer[4]<<8)|buffer[5];

        char temp[41];
        sprintf(temp,"Mag: %d %d %d",x,z,y);
        transmit(temp);
}
