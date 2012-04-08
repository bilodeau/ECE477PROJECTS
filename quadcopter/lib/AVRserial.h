#ifndef AVRSERIAL_H_
#define AVRSERIAL_H_
// THIS VERSION OF CODE IS FOR ATMEGA328P
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>
#include "baud.h"


#define FLASHONSERIAL 0 // flashes the led on PB1 every time serial communication happens
			// note that the actual flash lines are commented out below
#define RECEIVEBUFFERSIZE 81
#define TRANSMITQUEUESIZE 20

void setup_serial();
void transmit(char* str);
void begin_transmit();
void receive();
void check_buffer();
void clear_receive_buffer();

char receive_buffer_index;
char receive_buffer[RECEIVEBUFFERSIZE+1];

char transmit_buffer_index = 0;
char transmit_buffer_empty_row_index = 0;
char transmit_buffer_current_row_index = 0;
char transmit_buffer[TRANSMITQUEUESIZE][RECEIVEBUFFERSIZE+3];
volatile char serial_command_ready = 0;

void transmit(char* str){
	if(strlen(str) > RECEIVEBUFFERSIZE){
		sprintf(transmit_buffer[transmit_buffer_empty_row_index],"Command Longer Than Transmit Buffer.");
	}else{
		sprintf(transmit_buffer[transmit_buffer_empty_row_index],str);
	}
	strcat(transmit_buffer[transmit_buffer_empty_row_index],"\r\n");
//	if(transmit_buffer_current_row_index == transmit_buffer_empty_row_index)
//		begin_transmit();
	transmit_buffer_empty_row_index++;
	if(transmit_buffer_empty_row_index >= TRANSMITQUEUESIZE)
		transmit_buffer_empty_row_index = 0;
}

void clear_transmit_buffer(char row_index){
	char x;
	for(x=0;x<RECEIVEBUFFERSIZE+3;x++)
		transmit_buffer[row_index][x] = '\0';
}

void begin_transmit(){
	UDR0 = transmit_buffer[transmit_buffer_current_row_index][transmit_buffer_index];
	transmit_buffer_index++;
}

ISR(USART_UDRE_vect){
	char byte = transmit_buffer[transmit_buffer_current_row_index][transmit_buffer_index];
	if(byte != '\0'){
		UDR0 = byte;
		transmit_buffer_index++;
		if(transmit_buffer_index >=  strlen(transmit_buffer[transmit_buffer_current_row_index])){
			transmit_buffer_index = 0;
			clear_transmit_buffer(transmit_buffer_current_row_index);
			transmit_buffer_current_row_index++;
			if(transmit_buffer_current_row_index >= TRANSMITQUEUESIZE)
				transmit_buffer_current_row_index = 0;
		}
	}
}

ISR(USART_RX_vect){
	if (receive_buffer_index == 0){
		clear_receive_buffer();
		serial_command_ready = 0;
	}
//	while (!(UCSR0A & (1<<RXC0))); // don't need to wait since the interrupt only fires when ready
	receive_buffer[receive_buffer_index] = UDR0;
	check_buffer();
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
		transmit("Bad Command");
		receive_buffer_index = 0;
	}
}

void clear_receive_buffer(){
	char x;
	for(x=0;x<RECEIVEBUFFERSIZE+1;x++)
		receive_buffer[x] = '\0';
}

void setup_serial(){
	// blank out the transmit buffer queue
	char i;
	for (i=0;i<TRANSMITQUEUESIZE;i++)
		clear_transmit_buffer(i);

	// setup baud rate and frame based on defines in baud.h
	UBRR0H = MYUBRRH;
	UBRR0L = MYUBRRL;
	UCSR0A = MYUCSRA;
	UCSR0C = MYUCSRC;
	
	// enable receiver and transmitter
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);

	// enable receive complete interrupt
	UCSR0B |= (1<<RXCIE0)|(1<<UDRIE0);
	sei(); // enable global interrupts	
}
#endif
