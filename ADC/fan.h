#ifndef FAN_H_
#define FAN_H_

// sets up PWM motor control on TIMER2
// uses OC2 (PB3/MOSI)
void setup_fan(){
	// fast pwm mode and 1/256 prescaler for 31,250Hz, clear output on compare match
	TCCR2 = (1<<COM21)|(0<<COM20)|(1<<WGM20)|(1<<WGM21)|6; 
	OCR2 = 0;
	PORTB |= (1<<3); // enable output on PB3
}

// adjusts the fan speed based on the current temperature
// the target temp is 20 deg C -> 0% power to fan
// 40 deg C -> 100% power to fan
void update_fan(int temp){
	if(temp <= 2000){
		OCR2 = 0;
	}else if(temp >= 4000){
		OCR2 = 255;
	}else{
		OCR2 = (temp - 2000.)/2000.*255;
	}
}

#endif
