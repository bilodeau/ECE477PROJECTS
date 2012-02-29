#include <stdlib.h>
#include <stdio.h>
#include "baud.h"
#include <termios.h>
#include <fcntl.h>

void setup_serial();

int serialport;

int main(){
	setup_serial();
	while(1){
		char c;
		if(read(serialport, &c,1)==1){
			printf("AVR sent: %c %x %d\n",c,c,c);
		}else{
		//	printf("No Data.\n");

		}
	}
}

void setup_serial(){
	printf("opening serial port...\n");
	serialport = open("/dev/tty.usbmodemfd1221",O_NONBLOCK,O_RDONLY|O_NOCTTY);
	if (serialport == -1){
		printf("Error: Unable to open serial port.\n");
		exit(EXIT_FAILURE);
	}
	printf("serial port opened ok...\n");
	struct termios attribs;
	tcgetattr(serialport,&attribs);
	attribs.c_cflag &= ~CSIZE;
	attribs.c_cflag |= CS8;
	attribs.c_cflag |= CLOCAL|CREAD;
	attribs.c_iflag |= IGNPAR;
	attribs.c_oflag |= 0;
	attribs.c_lflag &= ~ICANON;
	cfmakeraw(&attribs);
	cfsetispeed(&attribs,PCBAUDRATE);
	tcsetattr(serialport,TCSANOW,&attribs);
	printf("serial setup ok\n");
}
