#include <avr/io.h>
#include <avr/iom8.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "charlie.c"

#define BAUDRATE 4800
#define FOSC 1843200
#define MYUBRR FOSC/16/BAUDRATE-1

void setup_serial();
int error();
void transmit(char byte);

unsigned char i = 0;

int main(void) {
	//set port B to input mode so we ignore those pins
	DDRB = 0;
	PORTB = 0;
	setup_serial();
	UCSRB |= (0<<RXCIE)|(0<<TXCIE)|(0<<UDRIE);	
	sei();// enable global interrupts

	while(1){
		transmit(i);
		i++;
		if (i > 127)
			i = 0;
	}
	return 0;
}

void transmit(char byte){
	while( !(UCSRA & (1<<UDRE)));
	UDR = byte;
}

ISR(USART_TXC_vect){
}

ISR(USART_RXC_vect){

}

ISR(USART_UDRE_vect){

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
}

// turns on the white LED for an error code
int error(){
	DDRB |= 2;
	PORTB &= ~2;
	return 0;
}
