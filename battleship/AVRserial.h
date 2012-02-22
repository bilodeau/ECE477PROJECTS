#include <avr/io.h>
#include <avr/iom8.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include "baud.h"

void setup_serial();
void transmit(char* str);
void transmit_byte(char byte);
void receive();

char receive_buffer[21];

int test_echo(void) {
	while(1){
		receive();
		transmit(receive_buffer);
	}
	return 0;
}

void transmit(char* str){
	char x;
	for(x=0; x <strlen(str);x++){
		transmit_byte(str[x]);
	}
	transmit_byte('\r');
	transmit_byte('\n');
}

void transmit_byte(char byte){
	while( !(UCSRA & (1<<UDRE)));
	UDR = byte;

}

void receive(){
	receive_buffer[20] = '\0';
	int x;
	for(x=0;x<20;x++){
		while (!(UCSRA & (1<<RXC)));
		receive_buffer[x] = UDR;
		if((receive_buffer[x] == '\n')&&(receive_buffer[x-1] == '\r')){
			receive_buffer[x-1] = '\0'; // terminates the string before carriage return newline
			x = 20;
			break;
		}
	}
}
void setup_serial(){
	// setup baud rate and frame based on defines in baud.h
	UBRRH = MYUBRRH;
	UBRRL = MYUBRRL;
	UCSRA = MYUCSRA;
	UCSRC = MYUCSRC;
	
	// enable receiver and transmitter
	UCSRB = (1<<RXEN)|(1<<TXEN);
	
	// configure interrupts
	UCSRB |= (0<<RXCIE)|(0<<TXCIE)|(0<<UDRIE);	
}
