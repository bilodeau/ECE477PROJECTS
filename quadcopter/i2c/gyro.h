#include <avr/io.h>
#include "../lib/data.h"

#define HIRESMODE 0

long gyro_x_rotational_velocity;
long gyro_y_rotational_velocity;
int gyroscope_x_bias;
int gyroscope_y_bias;

void get_gyro_data(){
	PRR &= ~(1<<PRADC); // disable power reduction for ADC so that it works
	
	ADMUX &= ~1; // make sure bit 0 is clear, so we're using ADC0
	ADCSRA |= (1<<ADSC);
	while(ADCSRA & (1<<ADSC));
	char low = ADCL;
	char high = ADCH&3; // mask to get only the low two bits
	
	gyro_x_rotational_velocity = 0;
	gyro_x_rotational_velocity |= low;
	gyro_x_rotational_velocity &= 0xFF;
	gyro_x_rotational_velocity |= (high<<8);
	gyro_x_rotational_velocity *= HIRESMODE ? 0.3541:1.611 ;
	gyro_x_rotational_velocity -= gyroscope_x_bias;

	// now switch to ADC1 to get the y value
	ADMUX |= 1;	
	ADCSRA |= (1<<ADSC);
	while(ADCSRA & (1<<ADSC));
	low = ADCL;
	high = ADCH&3; // mask to get only the low two bits
	
	gyro_y_rotational_velocity = 0;
	gyro_y_rotational_velocity |= low;
	gyro_y_rotational_velocity &= 0xFF;
	gyro_y_rotational_velocity |= (high<<8);
	gyro_y_rotational_velocity *= HIRESMODE ? 0.3541:1.611 ;
	gyro_y_rotational_velocity -= gyroscope_y_bias;
}

void power_on_gyro(){
	ADMUX = 0; // setup to use pin ADC0 initially
	ADMUX &= ~((1<<REFS1)|(1<<REFS0));
	ADCSRA &= ~(7); // use prescaler 2 (by clearing low 3 bits)
	ADCSRA |= (1<<ADEN); // enable analog to digital conversion
	
	// zero the gyro with correct bias,  gyro should be stationary when power_on is called
	gyroscope_x_bias = 0;
	gyroscope_y_bias = 0;
	get_gyro_data();	
	gyroscope_x_bias = gyro_x_rotational_velocity;
	gyroscope_y_bias = gyro_y_rotational_velocity;
}

void query_gyro(){
	get_gyro_data();
	char temp[40];
	sprintf(temp,"Gyro's X (deg/s): %ld",gyro_x_rotational_velocity);
	transmit(temp);
	sprintf(temp,"Gyro's Y (deg/s): %ld",gyro_y_rotational_velocity);
	transmit(temp);
}

void spam_gyro(){
	get_gyro_data();
	char temp[40];
	sprintf(temp,"%c%c%ld",SENSORDATAPACKETCHARACTER,GYROSCOPEX,gyro_x_rotational_velocity);
	transmit(temp);
	sprintf(temp,"%c%c%ld",SENSORDATAPACKETCHARACTER,GYROSCOPEY,gyro_y_rotational_velocity);
	transmit(temp);
}
