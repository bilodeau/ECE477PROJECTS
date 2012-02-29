#include <stdio.h>
#include "baud.h"
#include <fcntl.h>
#include <termios.h>
#include <ctype.h>

void setup_serial();

int* serialport;

int main(){
	setup_serial();
	while(1){
		
		printf("message\n");
	}

}

void setup_serial(){
	*serialport = open("/dev/tty.usbmodem.fd1221",O_RDONLY|O_NOCTTY);
	if (*serialport == -1){
		printf("Error: Unable toe open serial port.");
	}
	struct termios attribs;
	tcgetattr(*serialport,&attribs);
	attribs.c_cflag &= ~CSIZE;
	attribs.c_cflag |= CS8;
	attribs.c_cflag |= CLOCAL|CREAD;
	attribs.c_iflag |= IGNPAR;
	attribs.c_oflag |= 0;
	attribs.c_lflag &= ~ICANON;
	cfmakeraw(&attribs);
	cfsetispeed(&attribs,PCBAUDRATE);
	tcsetattr(*serialport,TCSANOW,&attribs);
}
