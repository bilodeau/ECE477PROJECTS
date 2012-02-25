#include <avr/io.h>
#include <avr/interrupt.h>

int main(void) 
{ 
// disable global interrupts
cli();

// disable ADC
ADCSRA = 0;

MCUCR &= ~(1<<PUD); // disable the global pull up disable, enabling local pull ups

DDRC = 0x00; // all C pins to input
PORTC = 0xFF; // enable pullup resistors
DDRC = 3; // all C pins to output
PORTC = 2;

DDRB = 2;
PORTB = 0;

while(1){
//PORTC = 2;
}

}

