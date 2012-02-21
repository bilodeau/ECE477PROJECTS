#include <avr/io.h>
#include <avr/iom8.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include"../charlie/charlie.h"

#define BAUDRATE 9600
#define FOSC 1843200
#define MYUBRR FOSC/16/BAUDRATE-1

void setup_serial();
void transmit(char* byte);
void receive();

char receive_buffer[21];

int main(void) {
	setup_serial();
	
	while(1){
		receive();
		transmit(receive_buffer);
	}
	return 0;
}

// the PC will be expecting "\r\n" as the last two bytes of this command
void transmit(char* str){
	led_on_i(1);
	char x;
	for(x=0; x <strlen(str);x++){
		while( !(UCSRA & (1<<UDRE)));
		UDR = str[x];
	}
	led_off();
}

void receive(){
	receive_buffer[20] = '\0';
	int x;
	for(x=0;x<20;x++){
		while (!(UCSRA & (1<<RXC)));
		receive_buffer[x] = UDR;
		if((receive_buffer[x] == '\n')&&(receive_buffer[x-1] == '\r')){
			led_on_i(6);
			receive_buffer[x+1] = '\0';
			x = 20;
			break;
		}
	}
}
void setup_serial(){
	// setup baud rate
	UBRRH = 0;//(unsigned char) ((MYUBRR)>>8);
	UBRRL = 12;//(unsigned char) MYUBRR;
	
	UCSRA |= (1<<UDRE)|(1<<U2X); // turn off double speed mode!!
	// enable receiver and transmitter
	UCSRB = (1<<RXEN)|(1<<TXEN);
	// set frame format: 8 data, 2 stop bit
	UCSRC = (1<<URSEL)|(1<<USBS)|(3<<UCSZ0);
	
	// configure interrupts
	UCSRB |= (0<<RXCIE)|(0<<TXCIE)|(0<<UDRIE);	
}
