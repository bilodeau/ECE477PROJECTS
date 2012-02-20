#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include "rtttl.h"
#include "rtttl_modular.h"

#define SOUNDOFF 1
#define PRINTDEBUG 1
#define BAUDRATE 9600

void play_tune(struct note notes[], int tempo, int count, int serialport);
void mysetup_serial_port(int *serialport);
int query_note(int serialport);

int main() {
	int bpm, num_notes;
	int serialport;
	struct note *notes;
	mysetup_serial_port(&serialport);
	get_note_array(notes, &bpm, &num_notes);
	play_tune(notes, bpm, num_notes, serialport);
	free(notes);
}

void play_tune(struct note notes[], int tempo, int count, int serialport){
  //outb(0xb6,0x43);	// config byte out
  	int i = 0;
  	for(i = 0; i < count; i++) {
    		struct note n = notes[i];
    		if(SOUNDOFF){
      			printf("playing freq: %d\n", n.divisor);
      			printf("waiting for: %f\n",n.cycles/1000000.);      
			transmit_note(serialport, n.divisor);
			query_note(serialport);
			usleep(n.cycles);
    		} else {
      			int temp = n.divisor;
      			if (n.divisor != 0) {		// this note is not a rest
				transmit_note(serialport, n.divisor);
				query_note(serialport);
				outb(inb(0x61)|3,0x61);		// speaker on
				outb(temp&255,0x42); 		// low byte out
				outb(temp/256,0x42);		// high byte out
				usleep(n.cycles); 		// sleep for microseconds
				outb(inb(0x61)&~3,0x61);	// speaker off
      			} else {
				transmit_note(serialport, 0);
				usleep(n.cycles); 		// sleep for microseconds
      			}
    		}
  	}
}

int transmit_note(int serialport, int divisor) {
	int brightness = ((log(1./divisor) + 9) * 34.6);// calc normalized brightness
	PRINTDEBUG&&printf("brightness is: %d\n",brightness);
	char *string = malloc(20*sizeof(char));
	sprintf(string, "SET B1 %d\r\n", brightness);	
	
	// send brightness value to AVR
	if (strlen(string) > 20){
		PRINTDEBUG&&printf("Command Too Long...");
	} else {
		int test = write(serialport,string,strlen(string));
		PRINTDEBUG&&printf("string sent: %s\nbytes sent:  %d\n",string,test);
	}

	// read AVR response and print data to stdout
	char buffer[20];
	int test = read(serialport, buffer, sizeof(buffer));
	if (test = -1) {
		printf("AVR did not a send response to the query.\n");
	}
	else {
		buffer[test] = '\0';
		PRINTDEBUG&&printf("string received: %s\nbytes sent:  %d\n",buffer,test);	
	}
	return 0;
}

int query_note(int serialport) {

	// send brightness query
	char string[] = "GET B1\r\n";
	int test = write(serialport,string,strlen(string));
	PRINTDEBUG&&printf("string sent: %s\nbytes sent:  %d\n",string,test);

	// read AVR response and print data to stdout
	char buffer[20];
	test = read(serialport, buffer, 20);
	if (test = -1) {
		printf("AVR did not send response to query.\n");
	}
	else {
		buffer[test] = '\0';
		PRINTDEBUG&&printf("string received: %s\nbytes sent:  %d\n",buffer,test);	
	}		 
}

void mysetup_serial_port(int *serialport) {	
	PRINTDEBUG&&printf("about to open port...\n");
	*serialport = open("/dev/tty.usbmodemfd1211",O_NONBLOCK|O_RDWR|O_NOCTTY);
	PRINTDEBUG&&printf("opened port, now check if null\n");
	if (*serialport  == -1){
		printf("Error: Unable to open serial port.\n");	
		exit(1);
	}

	PRINTDEBUG&&printf("opened port successfully...\n");
	struct termios attribs;
	tcgetattr(*serialport,&attribs);

	PRINTDEBUG&&printf("got attributes ok...\n");
	attribs.c_cflag &= ~CSIZE;
	attribs.c_cflag |= CS8;
	attribs.c_cflag |= CLOCAL|CREAD;
	attribs.c_iflag |= IGNPAR;
	attribs.c_oflag |= 0;
	attribs.c_lflag &= ~ICANON;
	cfmakeraw(&attribs);
	cfsetispeed(&attribs,BAUDRATE);
	cfsetospeed(&attribs,BAUDRATE);

	tcsetattr(*serialport,TCSANOW,&attribs);
	PRINTDEBUG&&printf("done with attributes...\n");
}
