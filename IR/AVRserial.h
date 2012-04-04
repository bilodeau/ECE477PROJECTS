#include <avr/io.h>
#include <avr/iom8.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include "baud.h"

#define COMMANDLENGTH 200
// These two variables are globals used to tell when and what command has been received from the PC
char receive_buffer[COMMANDLENGTH+1];  // buffer to hold the actual characters received
volatile char serial_command_ready = 0;  // this is a flag that's set when a complete command
					 // has been received.  It is the responsibility of the
					 // command processing code to ensure that this flag
					 // is cleared when the command is processed.

void transmit_byte(char byte){
	while( !(UCSRA & (1<<UDRE))); // wait for the data register to be empty
	UDR = byte;		  // load the given byte into the data register for transmission
}

// sends the specified string to the PC and sends carriage return newline as terminating characters
void transmit(char* str){
	char x;
	for(x=0; x <strlen(str);x++){
		transmit_byte(str[x]); // transmit each byte in the string
	}
	transmit_byte('\r'); // transmit the terminating bytes
	transmit_byte('\n');
}

// simple echo function that transmits back to the PC whatever command was send in.
// used for debugging
int test_echo(void) {
	while(1){
		if (serial_command_ready){
			serial_command_ready = 0;
			transmit(receive_buffer);
		}
	}
	return 0;
}

// Interrupt handler: called when we're just received a byte over serial
ISR(USART_RXC_vect){
	serial_command_ready = 1; // set the flag so that the rest of the program knows we'll have a command ready for processing
	int x;
	for(x=0;x<COMMANDLENGTH;x++){  // wait and receive up to the next 20 bytes
		while (!(UCSRA & (1<<RXC)));
		receive_buffer[x] = UDR;
		
		// if we've just received carriage return newline
		if((receive_buffer[x] == '\n')&&(receive_buffer[x-1] == '\r')){
			receive_buffer[x-1] = '\0'; // terminates the string before carriage return newline
			x = COMMANDLENGTH;
			break; // stop receiving
		}
	}
}

// blanks out the receive buffer so we're ready for new data
void clear_receive_buffer(){
	char x;
	for(x=0;x<COMMANDLENGTH+1;x++)
		receive_buffer[x] = '\0';
}

void setup_serial(){
	// setup baud rate and frame based on defines in baud.h
	UBRRH = MYUBRRH;
	UBRRL = MYUBRRL;
	UCSRA = MYUCSRA;
	UCSRC = MYUCSRC;
	
	// enable receiver and transmitter
	UCSRB = (1<<RXEN)|(1<<TXEN);

	// enable receive complete interrupt
	UCSRB |= (1<<RXCIE);
	sei();	
}
