#include <avr/io.h>
#include <avr/iom8.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "charlie.c"

#define BAUDRATE 4800
#define FOSC 1843200
#define MYUBRR FOSC/16/BAUDRATE-1
#define UPDATEPERIOD  4000 // 40k is about 3 sec

void setup_serial();
void setup_timer();
int error();
void transmit(char byte);

unsigned char i = 0;

int main(void) {
	setup_serial();
	setup_timer();
	sei();// enable global interrupts

	while(1){
	}
	return 0;
}

void transmit(char byte){
	while( !(UCSRA & (1<<UDRE)));
	UDR = byte;
}

void setup_timer(){
	TCCR1A = 0;
	TCCR1B = 0x0b;
	OCR1A = UPDATEPERIOD;
	TIMSK = 16;
}
ISR(TIMER1_COMPA_vect){
	transmit(i);
	i++;
	if(i > 127)
		i = 0;
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
	
	// configure interrupts
	UCSRB |= (0<<RXCIE)|(0<<TXCIE)|(0<<UDRIE);	
}

// turns on the white LED for an error code
int error(){
	DDRB |= 2;
	PORTB &= ~2;
	return 0;
}
