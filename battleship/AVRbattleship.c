#include <string.h>
#include <stdlib.h>
#include "../charlie/charlie.h"
#include "AVRserial.h"

// Most of the above section is probably best placed into an AVR header
int init_game();
void add_three_ship();
void add_two_ship();
int game_status();
int process_request();
void check_hits(int request);
void setup_interrupt();

unsigned int mode = 0;
char ship_pos[20], shots_fired[20], shots_hit[20];  // create arrays for game

int main() {
	setup_serial();                    	// initialize serial port
	setup_interrupt();			// setup interrupt
	sei(); // enable interrupts globally

	while(!serial_command_ready) {	// discard pseudorandom nums until first fire command
		rand();
	}
	
	init_game();        // zero out arrays and add ships to game
    	while (1) {         // until game is over
		if (serial_command_ready){
			serial_command_ready = 0;
			int request = process_request();    // process request
        		if (request > -1) {                 // if request >= 0, then it is a shot
            			shots_fired[request] = 1;       // update shots fired array
            			check_hits(request);  // check if shot is successful
            			if (game_status()){       // check if game is over
        				// animate yay!
					mode = 2;
				}
			} else if (request == -1) {         // if request == -1, then RESET the game
            			//show ships
				mode = 2;
        		}
		}
		if(mode == 0){
			flash_leds_from_array(shots_fired);
		} else if (mode == 1) {
			flash_leds_from_array(shots_hit);
		} else {
			flash_leds_from_array(ship_pos);
    		}
    	}
    	return 0;
}

ISR(TIMER1_COMPA_vect){	
	if (mode < 2){
		mode ^= 1; // toggle between all shots mode to flashing hits mode
	}else{
		mode++;
	}
	if (mode > 100){
		mode = 0;
		init_game();
	}
}

// Takes the three game arrays and zeroes them out.
// Then adds a 3-ship and a 2-ship to the ship_pos array
int init_game() {
    int i;
    for (i = 0; i < 20; i++) {  // zero out char arrays
        shots_fired[i] = 0;
        shots_hit[i] = 0;
        ship_pos[i] = 0;
    }
    add_three_ship();       // add three_ship
    add_two_ship();         // add two ship
    return 0;                // return game
}

// Adds a ship of length three to the ship_pos array
void add_three_ship() {
    int pos = 1;
    int i, j;
    while (pos) {           // while the current position is full
        i = (rand() % 4);      // get a random i index
        j = (rand() % 5);      // get a random j index
        pos = ship_pos[i + j*4];    // check if the random i,j position is empty
    }
    ship_pos[i + j*4] = 1;      // an empty spot is found, so fill in the grid spot

    char orientation = (rand() % 4);    // get a random orientatin (0,1,2,3 = W,E,N,S, respectively)
    while (orientation != 5) {
        switch (orientation) {
            case 0:             // west orientation
                if (j > 1) {    // check if too close to wall to draw rest of ship
                    ship_pos[i + (j - 1)*4] = 1;
                    ship_pos[i + (j - 2)*4] = 1;
                    orientation = 5;
                    break;
                } else {
                    orientation = (rand() % 3) + 1;
                    break;
                }

            case 1:             // east orientation
                if (j < 2) {    // check if too close to wall to draw rest of ship
                    ship_pos[i + (j + 1)*4] = 1;
                    ship_pos[i + (j + 2)*4] = 1;
                    orientation = 5;
                    break;
                } else {
                    int choices[3] = {0,2,3};
                    orientation = choices[(rand() % 3)];
                    break;
                }
            case 2:             // north orientatino
                if (i > 1) {    // check if too close to wall to draw rest of ship
                    ship_pos[(i - 1) + j*4] = 1;
                    ship_pos[(i - 2) + j*4] = 1;
                    orientation = 5;
                    break;
                } else {
                    int choices[3] = {0,1,3};
                    orientation = choices[(rand() % 3)];
                    break;
                }
            case 3:             // south orientation
                if (i < 3) {    // check if too close to wall to draw rest of ship
                    ship_pos[(i + 1) + j*4] = 1;
                    ship_pos[(i + 2) + j*4] = 1;
                    orientation = 5;
                    break;
                } else {
                    int choices[3] = {0,1,2};
                    orientation = choices[(rand() % 3)];
                    break;
                }
        }
    }
}

// Adds a ship of length two to the ship_pos array.
void add_two_ship() {
    int pos = 1;
    int i, j;
    while (pos) {           // find open spot on grid
        i = (rand() % 4);      // random 0-4
        j = (rand() % 5);      // random 0-3
        pos = ship_pos[i + j*4];
    }
    ship_pos[i + j*4] = 1;      // fill in grid spot

    char orientation = (rand() % 4);   // get random orientation 0-3
    while (orientation != 5) {
        switch (orientation) {
            case 0:             // west orientation
                if ((j > 0) && !ship_pos[i + (j - 1)*4]) {// check if other ship is there or too close to wall
                    ship_pos[i + (j - 1)*4] = 1;
                    orientation = 5;
                    break;
                } else {        // roll another orientation
                    orientation = (rand() % 3) + 1;
                    break;
                }

            case 1:             // east orientation
                if ((j < 3) && !ship_pos[i + (j + 1)*4]) {// check if other ship is there or too close to wall
                    ship_pos[i + (j + 1)*4] = 1;
                    orientation = 5;
                    break;
                } else {        // roll another orientation
                    int choices[3] = {0,2,3};
                    orientation = choices[rand() % 3];
                    break;
                }
            case 2:             // north orientatino
                if ((i > 0) && !ship_pos[(i - 1) + j*4]) {// check if other ship is there or too close to wall
                    ship_pos[(i - 1) + j*4] = 1;
                    orientation = 5;
                    break;
                } else {        // roll another orientation
                    int choices[3] = {0,1,3};
                    orientation = choices[rand() % 3];
                    break;
                }
            case 3:             // south orientation
                if ((i < 4) && !ship_pos[(i + 1) + j*4]) {// check if other ship is there or too close to wall
                    ship_pos[(i + 1) + j*4] = 1;
                    orientation = 5;
                    break;
                } else {        // roll another orientation
                    int choices[3] = {0,1,2};
                    orientation = choices[rand() % 3];
                    break;
                }
        }
    }
}

// Queries game status. Returns 1 if game is won, 0 otherwise.
int game_status() {
    int count = 0;              // count variable
    int i;
    for (i = 0; i< 20; i++) {   // count up number of hits
        if (shots_hit[i])
            ++count;
    }

    if (count == 5) return 1;
    else return 0;
}

// Process request from PC. Returns shot pos if command is FIRE and pos is valid.
// Returns -1 is command is RESET and -2 if command is wrong or pos is out of range
int process_request(){
	int shot;
	if (!strncmp(receive_buffer,"RESET",5)){    // if RESET, then return -1
		return -1;
	}else if(!strncmp(receive_buffer,"FIRE",4)){    // if FIRE, then get position
        if ((sscanf(receive_buffer+4,"%d",&shot)==1)&&((shot>=0)&&(shot<=19))){  // check for a good value
            char echo[20];
            sprintf(echo,"Position = %d",shot);
            transmit(echo);                     // echo good value
            return shot;                        // return good value
        }else{
            transmit("ILLEGAL VALUE");      // if bad value, tell PC and return -2
            return -2;
        }
	}else{
		transmit("UNIMPLEMENTED");          // if bad command, tell PC and return -2
		return -2;
	}
}

// Checks if a shot is a HIT and updates the shots_hit array accordingly
//  If it is a hit, transmits "HIT!\r\n" to PC, "MISS :(\r\n" otherwise.
void check_hits(int request) {
    if (ship_pos[request]) {    // check if request is a hit
        shots_hit[request] = 1; // update shots_hit array
        transmit("HIT");   // tell PC about success
    } else {
        transmit("MISS");// else tell PC about failure
    }
}

// Setup interrupt
void setup_interrupt() {
	TCCR1A = 0;
	TCCR1B = 0x0b;
	OCR1A = 20000;
	TIMSK = 16;
}
