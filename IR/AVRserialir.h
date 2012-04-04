#include <avr/io.h>
#include <avr/iom8.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include "baud.h"

#define RECEIVEBUFFERSIZE 200

// These two variables are globals used to tell when and what command has been received from the PC
char receive_buffer[RECEIVEBUFFERSIZE];  // buffer to hold the actual characters received
char receive_buffer_index = 0;
volatile char serial_command_ready = 0;  // this is a flag that's set when a complete command
					 // has been received.  It is the responsibility of the
					 // command processing code to ensure that this flag
					 // is cleared when the command is processed.
void check_buffer();
unsigned int get_burst_value(char *ptr);
void setup_serial();
void clear_receive_buffer();

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
	//if(FLASHONSERIAL)DDRB = 2,PORTB = 0;
	if (receive_buffer_index == 0){
		clear_receive_buffer();
		serial_command_ready = 0;
	}
	while (!(UCSRA & (1<<RXC)));
	receive_buffer[receive_buffer_index] = UDR;
	check_buffer();
	//if(FLASHONSERIAL)PORTB = 2;
}

// checks to see if we've got a complete command in the buffer
void check_buffer(){
	if ((receive_buffer_index>0)&&(receive_buffer[receive_buffer_index] == '\n')&&(receive_buffer[receive_buffer_index-1] == '\r')){
	
		if (receive_buffer_index == (get_burst_value(&receive_buffer[8])*8 + get_burst_value(&receive_buffer[12])*8 + 17)) {	// if command is the proper size
			receive_buffer[receive_buffer_index - 1] = '\0'; // terminates the string before carriage return newline
			serial_command_ready = mode = 1;
		}else{						// else the command is too short
			receive_buffer_index = 0;	// reset the buffer
		}
	}else if (receive_buffer_index < RECEIVEBUFFERSIZE){
		if (receive_buffer_index == (get_burst_value(&receive_buffer[8])*8 + get_burst_value(&receive_buffer[12])*8 + 17))	// if the command is too long
			receive_buffer_index = 0;	// reset the buffer
		else
			receive_buffer_index++;		// else increment the index and wait for the next byte
	}else {
		transmit("Bad Command\r\n");
		receive_buffer_index = 0;
	}
}

// blanks out the receive buffer so we're ready for new data
void clear_receive_buffer(){
	char x;
	for(x=0;x<RECEIVEBUFFERSIZE+1;x++)
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

unsigned int get_burst_value(char *ptr) {
	unsigned int result;
	sscanf(ptr,"%ud",&result);
	return result;
}
