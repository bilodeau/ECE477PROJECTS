#ifndef CONTROLLER_H_
#define CONTROLLER_H_
char controller_north_thrust;
char controller_south_thrust;
char controller_east_thrust;
char controller_west_thrust;

void setup_controller(){

}

void compute_controller(){

}

char get_north_thrust(){
	return controller_north_thrust;
}

char get_south_thrust(){
	return controller_south_thrust;
}

char get_east_thrust(){
	return controller_east_thrust;
}

char get_west_thrust(){
	return controller_west_thrust;
}
#endif
