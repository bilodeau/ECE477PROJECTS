#include "../lib/AVRserial.h"
#include "../i2c/spam.h"

int main(){
setup_serial();
while(1){
	transmit("Hello World!");
}
return 0;
}
