#include <avr/io.h>
#include "delay.h"

int main(){
setup_delay();
DDRB=2;
while(1){
PORTB=0;
int i;
// should be on for a second and a half and likewise off for a second and a half
for(i=0;i<100;i++)
	delay(15);
PORTB=2;
for(i=0;i<100;i++)
	delay(15);
}
return 0;
}
