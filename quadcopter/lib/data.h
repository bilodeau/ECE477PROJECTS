#ifndef DATASTRUCT
#define DATASTRUCT 1

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
	long pitch;
	long roll;
};

struct sensor_data sensor_data_cache;
#endif
