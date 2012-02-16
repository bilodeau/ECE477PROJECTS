#include <termios.h>
#include <unistd.h>
#include <stdio.h>

#define BAUDRATE B9600

int main(){
	FILE* serialport;
	setup_serial_port();
	while(1){
		char byte;
//		while (byte[1] == EOF)
			fread(&byte,sizeof(byte),1,serialport);
		if (byte != '\n')
			printf("DATA: %x\n",byte);
	}
	fclose(serialport);
	return 0;
}

void setup_serial_port(){
	if ((serialport = fopen("/dev/ttys000","rb")) == NULL){
		printf("Error: Unable to open serial port.\n");	
		exit(1);
	}
	struct termios attribs;
	tcgetattr(serialport,&attribs);
	
	attribs.c_cflag |= CS8|CLOCAL|CREAD;
	attribs.c_iflag |= IGNPAR;
	attribs.c_oflag |= 0;
	attribs.c_lflag |= 0;
	cfsetispeed(&attribs,BAUDRATE);
	cfsetospeed(&attribs,BAUDRATE);
	
	tcsetattr(serialport,TCSANOW,&attribs);
}
