#include <avr/io.h>
#include "delay.h"

int main(){
setup_delay();
DDRB=2;
while(1){
PORTB=0;
int i;
for(i=0;i<1000;i++)
	delay(1);
PORTB=2;
for(i=0;i<1000;i++)
	delay(1);
}
return 0;
}
