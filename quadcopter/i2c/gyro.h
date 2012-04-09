#ifndef GYRO_H_
#define GYRO_H_

#include <avr/io.h>
#include "../lib/data.h"

#define HIRESMODE 0

ISR(ADC_vect){
	unsigned char low = ADCL;
	unsigned char high = ADCH&3; // mask to get only the low two bits
	if(ADMUX&1 == 0){
		sensor_data_cache.gyroscope_x_rotational_velocity = ((high)<<8)&low * (HIRESMODE ? 0.3541:1.611) - gyroscope_x_bias;
	}else{
	sensor_data_cache.gyroscope_y_rotational_velocity = ((high)<<8)&low * (HIRESMODE ? 0.3541:1.611) - gyroscope_y_bias;
	}
	// now toggle the ADC channel to get the other value
	ADMUX ^= 1;	
}

void trigger_gyro_conversion(){
	ADCSRA |= (1<<ADSC); // start a conversion
}

void power_on_gyro(){
	PRR &= ~(1<<PRADC); // disable power reduction for ADC so that it works
	ADMUX = 0; // setup to use pin ADC0 initially
	ADMUX &= ~((1<<REFS1)|(1<<REFS0));
	ADCSRA = (1<<ADEN)|(1<<ADIE); // enable analog to digital conversion, using prescaler 2 (low 3 bits are clear), enable ADC interrupt
	sei(); // make sure interrupts are enabled globally
	trigger_gyro_conversion();
}

void query_gyro(){
	char temp[40];
	sprintf(temp,"Gyro's X (deg/s): %ld",sensor_data_cache.gyroscope_x_rotational_velocity);
	transmit(temp);
	sprintf(temp,"Gyro's Y (deg/s): %ld",sensor_data_cache.gyroscope_y_rotational_velocity);
	transmit(temp);
}
#endif
