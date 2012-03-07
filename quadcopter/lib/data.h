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

struct sensor_data{
	long compass_heading;
	long sonar_distance;
	long barometer_temperature;
	long barometer_pressure;
	long barometer_altitude;
	long gyroscope_x_rotational_velocity;
	long gyroscope_y_rotational_velocity;
	long gyroscope_temperature;
	long nunchuck_x_angular_position;
	long nunchuck_y_angular_position;
	long nunchuck_z_angular_position;
};

struct sensor_data sensor_data_cache;
#endif
