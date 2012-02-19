#include <stdio.h>
#include <string.h>
//#include <sys/io.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include "rtttl.h"
#include "partb.c"

#define DEBUG 0
#define DDURATION 4
#define DOCTAVE 6
#define DBPM 63
#define SOUNDOFF 1
#define PRINTDEBUG 1

char* parse_note_string(char* ptr,struct defaults* defs, struct note* n);
void parse_defaults(char* defaultsstring, struct defaults* defs);
void parse_s_def(char *ptr, struct defaults *def);
void play_tune(struct note notes[], int tempo, int i);
int calc_divisor(char *pitch_string, char *octave_string);
int calc_cycles(struct defaults* defs, char* len_string, char* dot);
int convert(int key);
void playthesong();
char* get_len();
char* get_pitch();
char* get_octave();
char* get_dot();
char *break_up_string(char *ptr, char *defs);
int count_notes(char song[], char c);
char *clear_space(char *ptr);
int isEmpty(char* ptr);
int transmit_note(int divisor);
void transmit(char* c);


int serialport;

int main() {
  char song[] = "HauntedHouse: d=4,o=5,b=120: 2a4, 2e, 2d#, 2b4, 2a4, 2c, 2d, 2a#4, 2e., e, 1f4, 1a4, 1d#, 2e., d, 2c., b4, 1a4, 1p, 2a4, 2e, 2d#, 2b4, 2a4, 2c, 2d, 2a#4, 2e., e, 1f4, 1a4, 1d#, 2e., d, 2c., b4, 1a4";

  char *defaultsstring, *tune;
  struct defaults defs;
  
  mysetup_serial_port();

  defaultsstring = malloc(strlen(song) + 1);  	// allocate memory for the defaults section of the string
  if (defaultsstring == NULL){error("Out of memory.");}

  tune = break_up_string(song, defaultsstring);		// break up song into its 3 parts.

  parse_defaults(defaultsstring,&defs);			// parse default string into default struct

  int count = count_notes(tune, ',');
  struct note notes[count];			// count notes and allocate array

  int i = 0;
  char *currentNote = tune;				// while there are notes in the note section, parse them into the array
  while(!isEmpty(currentNote)){
    if (i >= count) {error("Bad Note Section.");}
    currentNote = parse_note_string(currentNote, &defs, &notes[i]);
    i++;
  }

  play_tune(notes,defs.bpm, i);				// play the song

  free(defaultsstring);					// free memory
  return 0;
}

// counts the number of commas in the song section and returns the count + 1
// # notes = # commas + 1 ; except when there are no notes
int count_notes(char song[], char c) {
  int count = 0;
  int i;
  for (i = 0; i < strlen(song); i++)
    if (song[i] == c)
      count++;
  return count + 1;
}

// given a ptr to a RTTTL string, parse the defaults section into defs
// and return a pointer to the notes section
// IMPORTANT: defs must have already had memory allocated to it or Seg Fault will occur
char *break_up_string(char *ptr, char *defs) {
  char *defsstr, *tune;

  defsstr = strchr(ptr, ':') + 1;
  tune = strchr(defsstr, ':') + 1;

  sscanf(defsstr, "%[^:]", defs);

  return tune;
}

char *clear_space(char *ptr) {
  char* dummy = malloc(strlen(ptr) + 1);
  dummy[0] = '\0';
  sscanf(ptr,"%[ ]",dummy);
  ptr += strlen(dummy);
  free(dummy);
  return ptr;
}

void parse_defaults(char *ptr, struct defaults *def) {

  // Establish default values

  ptr = clear_space(ptr);		// advance ptr through empty space

  def->len = DDURATION;		// establish default values
  def->bpm = DBPM;
  def->oct = DOCTAVE;

  char *temp = malloc(strlen(ptr) + 1);		// temp var to hold scanned items

  sscanf(ptr, "%[^,]", temp);	// scan first item and parse
  parse_s_def(temp, def);

  while ((ptr = strchr(ptr, ',')) != NULL) {	// while there are more defaults, scan them and parse
    ptr += 1;
    sscanf(ptr, "%[^,]", temp);
    parse_s_def(temp, def);
  }

  free (temp);		// free memory
}

void parse_s_def(char *ptr, struct defaults *def) {

  ptr = clear_space(ptr);

  char *temp = malloc(strlen(ptr) + 1);
  if (temp == NULL){error("Out of memory.");}

  if (sscanf(ptr, "%[^=]", temp) > 1) {error("Bad Defaults String.");
  }

  *temp = toupper(*temp);

  if (*temp == 'D') {
    ptr = strchr(ptr, '=') + 1;
    sscanf(ptr, "%d", &(def->len));
  } else if (*temp == 'B') {
    ptr = strchr(ptr, '=') + 1;
    sscanf(ptr, "%d", &(def->bpm));
  } else if (*temp =='O') {
    ptr = strchr(ptr, '=') + 1;
    sscanf(ptr, "%d", &(def->oct));
  }
  else {
    error("Bad Default Identifier.");
  }

  free(temp);
}

void play_tune(struct note notes[], int tempo, int count){
  //outb(0xb6,0x43);	// config byte out
  int i = 0;
  for(i = 0; i < count; i++) {
    struct note n = notes[i];
    if(SOUNDOFF){
      printf("playing freq: %d\n", n.divisor);
      printf("waiting for: %f\n",n.cycles/1000000.);
      //usleep(n.cycles);
    } else {
      int temp = n.divisor;
      if (n.divisor != 0) {		// this note is not a rest
	transmit_note(n.divisor);
	outb(inb(0x61)|3,0x61);		// speaker on
	outb(temp&255,0x42); 		// low byte out
	outb(temp/256,0x42);		// high byte out
	usleep(n.cycles); 		// sleep for microseconds
	outb(inb(0x61)&~3,0x61);	// speaker off
      } else {
	transmit(0);
	usleep(n.cycles); 		// sleep for microseconds
      }
    }
  }
}

char *parse_note_string(char *ptr, struct defaults *def, struct note *note){
  char len_string[3];
  char pitch_string[3];
  char octave_string[2];
  char dot[2];

  // eat whitespace / commas and advance the pointer
  char* dummy = malloc(strlen(ptr)+1);
  if (dummy == NULL){ error("malloc failed");}
  dummy[0] = '\0';
  sscanf(ptr,"%[ ,]",dummy);
  ptr += strlen(dummy);
  free(dummy);

  DEBUG&&printf("Begin Parsing Note String...\n");
  ptr = get_len(ptr, len_string, def->len);
  DEBUG&&printf("get_len Successful...\n");
  ptr = get_pitch(ptr, pitch_string);
  DEBUG&&printf("get_pitch Successful...\n");
  ptr = get_octave(ptr, octave_string, def->oct);
  DEBUG&&printf("get_octave Successful...\n");
  ptr = get_dot(ptr, dot);
  DEBUG&&printf("get_dot Successful...\n");

  strcpy(note->pitch, pitch_string);
  sscanf(len_string, "%d", &(note->length));
  sscanf(octave_string, "%d", &(note->octave));

  note->divisor = calc_divisor(pitch_string, octave_string);
  DEBUG&&printf("divisor successful\n");

  note->cycles = calc_cycles(def, len_string, dot);
  DEBUG&&printf("cycles successful\n");

  return ptr;
}


int isEmpty(char* ptr){
  return (ptr[-1] == '\0')||!(strlen(ptr)>0);
}
void error(char* errorcode){
  printf("Exiting with error code: %s\n",errorcode);
  exit(1);
}

char *get_len(char *ptr, char *len_string, int dur) {

  if ((dur > 32) || (dur < 1)){
    error("Bad Note Duration.");
  }

  char *temp;
  temp = malloc(strlen(ptr) + 1);

  if (temp == NULL){
    error("Out of memory.");
  }

  sprintf(temp, "%d", dur);

  if (sscanf(ptr, "%[0123456789]", temp)) {
    if (strlen(temp) > 2) {
      error("Bad Note Duration.");
    } else {
      ptr += strlen(temp);
    }
  }
  strcpy(len_string, temp);

  free(temp);
  return ptr;
}


char *get_pitch(char *ptr, char *pitch_string) {

  char *temp;
  temp = malloc(strlen(ptr) + 1);

  if (temp == NULL){error("malloc failed in get_pitch.");}

  if (!sscanf(ptr, "%[ABCDEFGPabcdefgp#]", temp)){
    error("Bad Pitch.");
  }

  if (strlen(temp) > 2) {error("Bad Pitch.");}

  strcpy(pitch_string, temp);
  ptr += strlen(temp);
  free(temp);
  return ptr;
}

char *get_octave(char *ptr, char *octave_string, int oct) {

  char *temp;
  temp = malloc(strlen(ptr) + 1);

  if (temp == NULL){error("malloc failed in get_octave.");}
  if ((oct > 7) || (oct < 4)){error("Specified Octave out of range. Too High.");}

  sprintf(temp, "%d", oct);

  if(sscanf(ptr, "%[4567]", temp)) {
    if (strlen(temp) > 1) {
      error("Bad Octave.");
    } else {
      strcpy(octave_string, temp);
      ptr += strlen(temp);
    }
  }

  free(temp);
  return ptr;
}

char *get_dot(char *ptr, char *dot) {

  char *temp;
  temp = malloc(strlen(ptr) + 1);

  if (temp == NULL){error("malloc failed in get_dot");}

  if(sscanf(ptr, "%[.]", temp)) {
    if (strlen(temp) > 1) {
      error("Illegal character after \".\".");
    } else {
      strcpy(dot, temp);
      ptr += strlen(temp);
      //printf("%s\n",dot);
    }
  }else{ // there was no dot so write something else into *dot
    sprintf(dot,"%d",0);
  }

  free(temp);
  return ptr;
}

int calc_divisor(char *pitch_string, char *octave_string) {

  int oct;
  sscanf(octave_string, "%d", &oct);

  float div = convert(toupper(*pitch_string));
  float mod = 1;
  if (strlen(pitch_string) > 1) {

    if (pitch_string[1] == '#') {
      mod = 0.943874313;
    } else { // otherwise flat
      mod = 1.05946309;
    }
  }

  div *= pow(2, (4 - oct));
  div *= mod;

  return (int)div;
}

int convert(int key){
  switch (key){
  case 'C': return 4560;
  case 'D': return 4063;
  case 'E': return 3619;
  case 'F': return 3416;
  case 'G': return 3043;
  case 'A': return 2711;
  case 'B': return 2415;
  case 'P': return 0;
  default: return -1;
  }
}

// returns the duration of the note in microseconds
int calc_cycles(struct defaults *defs, char *len_string, char *dot){
  int duration;

  if (sscanf(len_string,"%d",&duration) != 1){error("Bad Duration");}

  duration = (defs->len*60*1000000)/defs->bpm / duration;

  if (*dot == '.'){
    duration *= 1.5,DEBUG&&printf("DOT\n");
  }else{
    DEBUG&&printf("NODOT\n");
  }
  return duration;
}

int transmit_note(int divisor) {
	char brightness = (int) ((log(1/divisor) + 9) * 34.6);// calc normalized brightness
	char string[4];
	string[0] = brightness; 
	string[1] = '\r';
	string[2] = '\n';
	string[4] = '\0';
	transmit(string);
	return 0;
}

void transmit(char* c){
	if (strlen(c) > 20){
		move(6,0);
		printw("Command Too Long...");
	} else {
		int test = write(serialport,&c,strlen(c));
		move(10,0);
		printw("bytes sent:  %d",test);
	}
}

void mysetup_serial_port(){	
	PRINTDEBUG&&printf("about to open port...\n");
	serialport = open("/dev/tty.usbmodemfd1211",O_NONBLOCK|O_RDWR|O_NOCTTY);
	PRINTDEBUG&&printf("opened port, now check if null\n");
	if (serialport  == -1){
		printf("Error: Unable to open serial port.\n");	
		exit(1);
	}

	PRINTDEBUG&&printf("opened port successfully...\n");
	struct termios attribs;
	tcgetattr(serialport,&attribs);

	PRINTDEBUG&&printf("got attributes ok...\n");
	attribs.c_cflag &= ~CSIZE;
	attribs.c_cflag |= CS8;
	attribs.c_cflag |= CLOCAL|CREAD;
	attribs.c_iflag |= IGNPAR;
	attribs.c_oflag |= 0;
	attribs.c_lflag &= ~ICANON;
	cfmakeraw(&attribs);
	cfsetispeed(&attribs,BAUDRATE);
	cfsetospeed(&attribs,BAUDRATE);

	tcsetattr(serialport,TCSANOW,&attribs);
	PRINTDEBUG&&printf("done with attributes...\n");
}