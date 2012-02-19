#include <avr/io.h>
#include <avr/iom8.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "charlie.c"

#define BAUDRATE 9600
#define FOSC 1843200
#define MYUBRR FOSC/16/BAUDRATE-1

void setup_serial();
int error();
void transmit(char byte);

char i = 0;

int main(void) {
	//set port B to input mode so we ignore those pins
	DDRB = 0;
	PORTB = 0;
	setup_serial();
	UCSRB |= (0<<RXCIE)|(0<<TXCIE)|(0<<UDRIE);	
	// enable just the Data REgister Empty interrupt
	sei();// enable global interrupts

//	transmit(i);
	while(1){
	while((UCSRA&(1<<UDRE)) == 0);
	UDR = 0x0a;
	i++;
		flash_leds();
	}
	return 0;
}

void transmit(char byte){
	error();
	while( !(UCSRA & (1<<UDRE)))
	UCSRB &= ~(1<<TXB8);
	if (byte & 0x0110)
		UCSRB |= (1<<TXB8);
	UDR = byte;
	if (i == 0){
		DDRB |= 2;
		PORTB |= 2;
	}
}

ISR(USART_TXC_vect){
	//clear();
	lightled(2);
	i++;
	transmit(i);
}

ISR(USART_RXC_vect){
	//clear();
	lightled(0);
}

ISR(USART_UDRE_vect){
	//clear();
	lightled(1);
	UDR = i;
	i++;
}
void setup_serial(){
	// setup baud rate
	UBRRH = 0;//(unsigned char) ((MYUBRR)>>8);
	UBRRL = 12;//(unsigned char) MYUBRR;
	
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
