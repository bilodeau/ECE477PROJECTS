#ifndef SPAM_H_
#define SPAM_H_
#include "../lib/data.h"
#include <string.h>
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

void spam_sonar(){
	query_slave(0x00,4);
        char temp[40];
        sprintf(temp,"%c%c%ld",SENSORDATAPACKETCHARACTER,SONARDISTANCE,sensor_data_cache.sonar_distance);
        transmit(temp);
}
#endif
