#include "../lib/AVRserial.h"
#include "../lib/devices/poll_devices.h"
#include "../i2c/i2c.h"
//#include "../lib/data.h"

int main(){
setup_serial();
setup_i2c();
while(1){
//	poll_magnetometer();
//	spam_magnetometer();
	poll_sonar();
	update_adj_alt();
	spam_sonar();
	transmit("Hello World!");
}
return 0;
}
