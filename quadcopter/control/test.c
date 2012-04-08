#include "../lib/AVRserial.h"
int main(){
setup_serial();
while(1){
	transmit("Hello World!");
}
return 0;
}
