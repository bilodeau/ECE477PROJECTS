// THIS VERSION OF CODE IS FOR ATMEGA328P
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "baud.h"


#define FLASHONSERIAL 0 // flashes the led on PB1 every time serial communication happens
#define RECEIVEBUFFERSIZE 20

void setup_serial();
void transmit(char* str);
void transmit_byte(char byte);
void receive();
void check_buffer();
void clear_receive_buffer();

char receive_buffer_index;
char receive_buffer[RECEIVEBUFFERSIZE+1];
volatile char serial_command_ready = 0;

void transmit(char* str){
//	FLASHONSERIAL&&(DDRB = 2,PORTB = 0);
	char x;
	for(x=0; x <strlen(str);x++){
		transmit_byte(str[x]);
	}
	transmit_byte('\r');
	transmit_byte('\n');
//	FLASHONSERIAL&&(PORTB = 2);
}

void transmit_byte(char byte){
	while( !(UCSR0A & (1<<UDRE0)));
	UDR0 = byte;

}

ISR(USART_RX_vect){
	FLASHONSERIAL&&(DDRB = 2,PORTB = 0);
	if (receive_buffer_index == 0){
		clear_receive_buffer();
		serial_command_ready = 0;
	}
	while (!(UCSR0A & (1<<RXC0)));
	receive_buffer[receive_buffer_index] = UDR0;
	check_buffer();
	FLASHONSERIAL&&(PORTB = 2);
}

// checks to see if we've got a complete command in the buffer
void check_buffer(){
	if((receive_buffer_index>0)&&(receive_buffer[receive_buffer_index] == '\n')&&(receive_buffer[receive_buffer_index-1] == '\r')){
		receive_buffer[receive_buffer_index-1] = '\0'; // terminates the string before carriage return newline
		serial_command_ready = 1;
		receive_buffer_index = 0;
	}else if (receive_buffer_index < RECEIVEBUFFERSIZE){
		receive_buffer_index++;
	}else{
		transmit("Bad Command\r\n");
		receive_buffer_index = 0;
	}
}

void clear_receive_buffer(){
	char x;
	for(x=0;x<RECEIVEBUFFERSIZE+1;x++)
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
