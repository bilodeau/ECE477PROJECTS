#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define BAUDRATE B2400

int serialport;
void mysetup_serial_port();

		char byte;
int main(){
	printf("main called..\n");
	mysetup_serial_port();
	printf("done setup...\n");
	while(1){
//	printf("waiting for good byte..\n");
		char temp = byte;
		if ((read(serialport,&byte,1) > 0))//&&(byte != temp))
			printf("DATA: %c\n",byte);
	}
	close(serialport);
	return 0;
}

void mysetup_serial_port(){	
	printf("about to open port...\n");
	serialport = open("/dev/tty.usbmodemfd1211",O_NONBLOCK|O_RDONLY|O_NOCTTY);
	printf("opened port, now check if null\n");
	if (serialport  == -1){
		printf("Error: Unable to open serial port.\n");	
		exit(1);
	}
	
	printf("opened port successfully...\n");
	struct termios attribs;
	tcgetattr(serialport,&attribs);
	
	printf("got attributes ok...\n");
	attribs.c_cflag &= ~CSIZE;
	attribs.c_cflag |= CS8;
	attribs.c_cflag |= CLOCAL|CREAD;
	attribs.c_iflag |= IGNPAR;
	attribs.c_oflag |= 0;
	attribs.c_lflag &= ~ICANON;
	cfmakeraw(&attribs);
	cfsetispeed(&attribs,BAUDRATE);
	cfsetospeed(&attribs,BAUDRATE);
	
	tcsetattr(serialport,TCSANOW,&attribs);
	printf("done with attributes...\n");
}
