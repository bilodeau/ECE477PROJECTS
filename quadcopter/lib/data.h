#ifndef DATASTRUCT
#define DATASTRUCT 1

// leading character for all sensor data packets
// recall that the GPS module uses $
#define SENSORDATAPACKETCHARACTER '@'

// data packet codes:
#define COMPASSHEADING 0
#define SONARDISTANCE 1
#define	BAROMETERTEMPERATURE 2
#define BAROMETERPRESSURE 3
#define BAROMETERALTITUDE 4
#define GYROSCOPEX 5
#define GYROSCOPEY 6
#define GYROSCOPETEMPERATURE 7
#define NUNCHUCKX 8
#define NUNCHUCKY 9
#define NUNCHUCKZ 10

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
