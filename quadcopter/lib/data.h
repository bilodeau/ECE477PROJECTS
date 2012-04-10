#ifndef DATA_H_
#define DATA_H_

// leading character for all sensor data packets
// recall that the GPS module uses $
#define SENSORDATAPACKETCHARACTER 64//'@'

// data packet codes:
#define COMPASSHEADING 1
#define SONARDISTANCE 2
#define	BAROMETERTEMPERATURE 3
#define BAROMETERPRESSURE 4
#define BAROMETERALTITUDE 5
#define GYROSCOPEX 6
#define GYROSCOPEY 7
#define GYROSCOPETEMPERATURE 8
#define NUNCHUCKX 9
#define NUNCHUCKY 10
#define NUNCHUCKZ 11
#define YAW 12
#define PITCH 13
#define ROLL 14

#define RP_FILTER_C 0.75
#define DELTA_T 8.192	// in ms
#define ALT_FILTER_C 0.75

struct sensor_data{
	long compass_heading; // degrees * 100
	long sonar_distance;	// millimeters
	long barometer_temperature;	// invalid data
	long barometer_pressure;	// invalid data
	long barometer_altitude;	// invalid data
	long gyroscope_x_rotational_velocity;	// degrees/sec (+- 100)
	long gyroscope_y_rotational_velocity;	// degrees/sec (+- 100)
	long gyroscope_temperature;		// unused
	long nunchuck_x_value;	// raw data values from the nunchuck
	long nunchuck_y_value;	//
	long nunchuck_z_value;	//
	long yaw;		// converted angular position values
	long nunchuck_pitch;
	long nunchuck_roll;
	long filt_roll_angle;
	long filt_pitch_angle;
	long filt_altitude;
};

struct sensor_data sensor_data_cache;

void update_adj_rp() {
	sensor_data_cache.filt_roll_angle = RP_FILTER_C*(sensor_data_cache.filt_roll_angle + sensor_data_cache.gyroscope_x_rotational_velocity*DELTA_T) + (1 - RP_FILTER_C)*(sensor_data_cache.nunchuck_roll);
	sensor_data_cache.filt_pitch_angle = RP_FILTER_C*(sensor_data_cache.filt_pitch_angle + sensor_data_cache.gyroscope_y_rotational_velocity*DELTA_T) + (1 - RP_FILTER_C)*(sensor_data_cache.nunchuck_pitch);
}

void update_adj_alt() {
	sensor_data_cache.filt_altitude = ALT_FILTER_C*(sensor_data_cache.filt_altitude) + (1 - ALT_FILTER_C)*(sensor_data_cache.sonar_distance);
}

void update_adj_yaw() {
	
}

// proposed new sensor data struct
/*struct sensor_data_new{
	struct l_data_q compass_heading;	// degress * 100
	struct l_data_q nunchuck_x_value, nunchuck_y_value, nunchuck_z_value;	// raw data vals from nunchuck
	struct l_data_q gyroscope_y_rotational_velocity, gyroscope_x_rotational_velocity; // degrees/sec (+- 100)
	struct l_data_q gyroscope_temperature; 	// unused
	struct l_data_q yaw, pitch, roll;	// converted angular positions
	struct s_data_q barometer_temperature, barometer_pressure, barometer_altitude;	// invalid data
	struct s_data_q sonar_distance;	// in hundredths of feet
};


// a fifo data queue holding up 4 data points
struct s_data_q{
	long data[4] = {0,0,0,0}
	int index = 0;
	sum = 0;
};

// a fifo data queue holding up to 8 data points
struct l_data_q{
	long data[8] = {0,0,0,0,0,0,0,0}
	int index = 0;
	sum = 0;
};

// adds a value to a s_data_q
void update_s_q(struct s_data_q *q, long i) {
	q->sum -= q->data[index];
	q->data[index] = i;
	q->sum += q->data[index++];
	index &= 3;	// mod index by 4
}

// adds a value to a l_data_q
void update_l_q(struct l_data_q *q, long i) {
	q->sum -= q->data[index];
	q->data[index] = i;
	q->sum += q->data[index++];
	index &= 7;	// mod index by 8
}

// returns the average value of a s_data_q
// this won't give good data until queue fills up
// otherwise only 1,2, or 3 values are summed and divided by 4
int get_s_q_ave(struct s_data_q *q) {
	return ((q->sum) >> 2);	// divide sum by 4
}

// returns the average value of a l_data_q
// this won't give good data until queue fills up
// otherwise only 1,2,3,4,5,6, or 7 values are summed and divided by 8
int get_l_q_ave(struct l_data_q *q) {
	return ((q->sum) >> 3);	// divide sum by 8
}*/
#endif
