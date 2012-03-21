#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <sys/io.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include "rtttl.h"
#include "rtttl_modular.h"
#include "baud.h"

#define SOUNDOFF 0
#define PRINTDEBUG 0

void play_tune(struct note notes[], int tempo, int count, int serialport);
void mysetup_serial_port(int *serialport);
int query_note(int serialport);

int main() {
	int bpm, num_notes;	// vars to hold bpm and number of notes
	int serialport;		// var to hold file index of the serial port
	struct note *notes;	// array of struct notes making up the song
	mysetup_serial_port(&serialport);	// open serial port and place file indentifier into serialport	
	notes = get_note_array(&bpm, &num_notes); // parses the notes into an array of note structs
	PRINTDEBUG&&printf("notes parsed OK\n");
	play_tune(notes, bpm, num_notes, serialport);	// play the tune
	free(notes);
	transmit_note(serialport,0); // make sure the led is off at the end
}

// plays the array of note structs and transmits a brightness level to the AVR when each note is played
void play_tune(struct note notes[], int tempo, int count, int serialport){
	(!SOUNDOFF)&&outb(0xb6,0x43);	// config byte out
  	int i = 0;
  	for(i = 0; i < count; i++) {
    		struct note n = notes[i];
		if(SOUNDOFF){ // simulate playing the tune without trying to actually use the speaker
      			printf("playing freq: %d\n", n.divisor);
      			printf("waiting for: %f\n",n.cycles/1000000.);      
			transmit_note(serialport, n.divisor); // send a command to AVR with a given brightness
			usleep(n.cycles);
			query_note(serialport);
    		} else {
      			int temp = n.divisor;
      			if (n.divisor != 0) {		// this note is not a rest
				transmit_note(serialport, n.divisor); // send a command to the AVR with the given brightness
				query_note(serialport);		// ask AVR what brightness it is playing
				outb(inb(0x61)|3,0x61);		// speaker on
				outb(temp&255,0x42); 		// low byte out
				outb(temp/256,0x42);		// high byte out
				usleep(n.cycles); 		// sleep for microseconds
				outb(inb(0x61)&~3,0x61);	// speaker off
      			} else {
				transmit_note(serialport, 0);	// transmit a zero to turn off LED during a P note
				usleep(n.cycles); 		// sleep for microseconds
      			}
    		}
  	}
}

// sends the AVR a command to set a brightness which we calculate from the given divisor
int transmit_note(int serialport, int divisor) {
	char brightness = ((log(1./divisor) + 9) * 34.6);// calculate normalized brightness
	PRINTDEBUG&&printf("calculated brightness is: %d\n",brightness);
	char *string = malloc(20*sizeof(char));		// allocate memory for string		
	sprintf(string, "SET B1 %d\r\n", brightness);	// place formatted command into the string
	
	// send brightness value to AVR
	if (strlen(string) > 20){ // don't send it if the command string is too long / misformatted
		PRINTDEBUG&&printf("Command Too Long...");
	} else {
		int test = write(serialport,string,strlen(string));	// else write to the serial port
		PRINTDEBUG&&printf("string sent to AVR: %sbytes sent to AVR:  %d\n",string,test);
	}
	return 0;
}

// waits for, reads in, and prints out the AVR's response
int query_note(int serialport) {
	// read AVR response and print data to stdout
	char buffer[21];	// buffer to hold received data
	int test = read(serialport, buffer, 20);	// read from serial port
	if (test == -1) {	// catch a bad serial port read
		printf("AVR did not send response to query.\n\n");
	}else {
		buffer[test] = '\0';	// places delimiter at end of string
		PRINTDEBUG&&printf("string received from AVR: %sbytes received from AVR:  %d\n\n",buffer,test);
	}		 
}

// configure the serial port for communication
void mysetup_serial_port(int *serialport) {	
	PRINTDEBUG&&printf("about to open port...\n");
	// open the port non-blocking, read/write and place the file index into the var serialport
	*serialport = open("/dev/tty.usbmodemfd1221",O_NONBLOCK|O_RDWR|O_NOCTTY);
	PRINTDEBUG&&printf("opened port, now check if null\n");
	if (*serialport  == -1){	// catch a bad serial port open call
		printf("Error: Unable to open serial port.\n");	
		exit(1);
	}

	PRINTDEBUG&&printf("opened port successfully...\n");
	struct termios attribs;
	tcgetattr(*serialport,&attribs);	// get termios attributes from the file with index serialport

	PRINTDEBUG&&printf("got attributes ok...\n");
	attribs.c_cflag &= ~CSIZE; // this mask is required when setting the frame size
	attribs.c_cflag |= CS8;	// use the frame size of 8 bits
	attribs.c_cflag |= CLOCAL|CREAD; // enable receiver and don't transfer control of the port
	attribs.c_iflag |= IGNPAR; // ignore parity
	attribs.c_oflag |= 0;
	attribs.c_lflag &= ~ICANON;  // don't use canonical mode, so that bytes are sent instantly
	cfmakeraw(&attribs);
	cfsetispeed(&attribs,PCBAUDRATE); // use the baud rate define from baud.h
	cfsetospeed(&attribs,PCBAUDRATE);

	tcsetattr(*serialport,TCSANOW,&attribs); // actually set the attributes we've setup
	PRINTDEBUG&&printf("done with attributes...\n");
}
