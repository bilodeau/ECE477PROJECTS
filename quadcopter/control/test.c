#include "../lib/AVRserial.h"
int main(){
setup_serial();
	char i = 0;
while(1){
	char temp[40];
	sprintf(temp,"Hello: %c",i);
	transmit(temp);
	i++;
}
return 0;
}
