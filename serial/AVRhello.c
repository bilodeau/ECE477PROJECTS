#include <avr/io.h>
#include <avr/iom8.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../charlie/charlie.h"

void init_serial(void);
void my_send_string (char * buf);

/* this is the equivalent of Hello World for AVR serial communication */
/* it sets up the serial communication for 1200 baud and repeatedly sends */
/* strings containing a long and an int value */
/* it forms the strings using sprintf         */

int main(void)
{
  char buf[50];
  unsigned int x;
  unsigned long y;
  x=y=0;
  init_serial();
  while(1) 
  {
    sprintf(buf,"Hello World!");//The values are:%lu\t%u\n",y++,x++);
	my_send_string(buf);
  }   
  return 0;
}

/* Initializes AVR USART for 4800 baud (assuming 1MHz clock) */
/* 1MHz/(16*(12+1)) = 4808                                   */

void init_serial(void)
{
   UBRRH=0;
   UBRRL=12; // 4800 BAUD FOR 1MHZ SYSTEM CLOCK
   UCSRA |= 2; // turn off double speed mode!!!
	UCSRC= (1<<URSEL)|(1<<USBS)|(3<<UCSZ0) ;  // 8 BIT NO PARITY 2 STOP
   UCSRB=(1<<RXEN)|(1<<TXEN)  ; //ENABLE TX AND RX ALSO 8 BIT
}   

/* simple routine to use software polling to send a string serially */
/* waits for UDRE (USART Data Register Empty) before sending byte   */
/* uses strlen to decide how many bytes to send (must have null     */
/* terminator on the string)                                        */

void my_send_string(char * buf)
{  int x;  //uses software polling, assumes serial is set up  
   for(x=0;x<strlen(buf);x++)
   {
     while((UCSRA&(1<<UDRE)) == 0); //wait until empty 
      UDR = buf[x];
	led_on_i(buf[x] % 20);
   }
}
