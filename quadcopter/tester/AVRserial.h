#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include "baud.h"

// THIS VERSION OF CODE IS FOR ATMEGA328P

void setup_serial();
void transmit(char* str);
void transmit_byte(char byte);
void receive();

char receive_buffer[21];
volatile char serial_command_ready = 0;

int test_echo(void) {
	while(1){
		if (serial_command_ready){
			serial_command_ready = 0;
			transmit(receive_buffer);
		}
	}
	return 0;
}

void transmit(char* str){
	DDRB = 2;
	PORTB = 0;
	char x;
	for(x=0; x <strlen(str);x++){
		transmit_byte(str[x]);
	}
	transmit_byte('\r');
	transmit_byte('\n');
	PORTB = 2;
}

void transmit_byte(char byte){
	while( !(UCSR0A & (1<<UDRE0)));
	UDR0 = byte;

}

ISR(USART_RX_vect){
	DDRB = 2;
	PORTB = 0;
	serial_command_ready = 1;
	int x;
	for(x=0;x<20;x++){
		while (!(UCSR0A & (1<<RXC0)));
		receive_buffer[x] = UDR0;
		if((receive_buffer[x] == '\n')&&(receive_buffer[x-1] == '\r')){
			receive_buffer[x-1] = '\0'; // terminates the string before carriage return newline
			x = 20;
			break;
		}
	}
	PORTB = 2;
}

void clear_receive_buffer(){
	char x;
	for(x=0;x<21;x++)
		receive_buffer[x] = '\0';
}

void setup_serial(){
	// setup baud rate and frame based on defines in baud.h
	UBRR0H = MYUBRRH;
	UBRR0L = MYUBRRL;
	UCSR0A = MYUCSRA;
	UCSR0C = MYUCSRC;
	
	// enable receiver and transmitter
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);

	// enable receive complete interrupt
	UCSR0B |= (1<<RXCIE0);
	sei(); // enable global interrupts	
}
