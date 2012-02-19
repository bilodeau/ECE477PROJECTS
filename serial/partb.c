#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "rtttl.h"

#define HEIGHT 28 // the number of horizontal lines of text it takes to print a single staff
#define WIDTH 80 // width of the terminal window in characters

/* struct declared in rtttl.h
struct note {
  int divisor;
  int cycles;
  int octave;
  int dot;
  char pitch[3];
  int length;
};

// error handling function declared in rtttl.
void error(char* errorcode){
  printf("Exiting with error: \n%s\n",errorcode);
  exit(1);
}
*/

char* setup_staff(int num_notes);
void insert_measure_bar(char* ptr);
void insert_note(char* ptr, struct note* n);
char* draw_note(char pitchpos,char duration);
void add_lines_outside_staff(char* ptr,char pitchpos);
char* pad_note(char* note, char columns);
void add_dot(char* ptr, char pitchpos,char dot);
void add_pitch_mod(char* ptr, char pitchpos, char mod);
char get_padding(char duration);
char pitchmod(char* pitch);
char pitchindex(char* pitch, char octave);
char note_symbol(char duration);
char* measure_bar();
char* blank_staff();
char* rest(char dur, char dot);
char* g_clef();
char* note(char duration, char* pitch, char octave, char dot);
char* reformat_music(char* ptr);
void print_test();
/*
  int main(char args[]){
  print_test();
  }*/

void print_test(){
  // manually assembles some sheet music for testing most of the other methods
  int numColumns = 500; // allocate plenty of memory
  char* temp = malloc(HEIGHT*numColumns+1);
  if (temp == NULL){error("malloc failed.");}

  strcat(temp, g_clef());
  strcat(temp, blank_staff());
  strcat(temp, blank_staff());
  strcat(temp, blank_staff());
  strcat(temp, blank_staff());
  strcat(temp, blank_staff());
  strcat(temp, blank_staff());
  strcat(temp,note(1,"C",4,0));
  strcat(temp,note(4,"D",4,0));
  strcat(temp,note(2,"P",4,0));
  strcat(temp,note(4,"C",4,0));
  strcat(temp,note(16,"E",4,0));
  strcat(temp,note(4,"F#",4,0));
  strcat(temp, blank_staff());
  strcat(temp, measure_bar());
  strcat(temp, blank_staff());
  strcat(temp, rest(2,0));
  strcat(temp,note(4,"G",4,0));
  strcat(temp,note(4,"C",5,1));
  strcat(temp, blank_staff());
  strcat(temp, measure_bar());
  strcat(temp, blank_staff());
  strcat(temp,note(8,"B",7,0));
  strcat(temp,note(2,"C",7,1));

  printf("%s",reformat_music(temp));
  free(temp);
}

// formats the given array of notes into a string of ASCII sheet music
char* write_music(struct note notes[], int num_notes){
  char* ptr = setup_staff(num_notes); // inserts the G-clef and allocates memory for the whole string
  int i;
  float time = 0; // keep a a running time count so we can insert measure bars
  for(i = 0; i < num_notes; i++){
    struct note n = notes[i];

    if (time >= 1){
      insert_measure_bar(ptr);
      time = time - 1;
    }
    time += 1./n.length;
    insert_note(ptr, &n);
  }
  ptr = reformat_music(ptr);
  return ptr;
}

// allocates memory for the whole string, draws the treble clef and some padding
char* setup_staff(int num_notes){
  int widthofnote = (get_padding(1)*2+4);
  int widthofclef = 8 + 5;
  int nummeasurebars = num_notes; //we know bars <= notes
  char* s = malloc(num_notes*widthofnote + nummeasurebars*3 + widthofclef);
  s[0] = '\0';
  strcat(s,g_clef()); // add the treble clef
  int i;
  for(i = 0; i < 5; i++)
    strcat(s,blank_staff()); // add some blank space
  return s;
}

// inserts a note from the info in the given struct at the end of the given string
void insert_note(char* ptr,struct note* n){
  strcat(ptr,note(n->length, n->pitch, n->octave, n->dot));
}

// inserts a measure bar (and some padding) at the end of the given string
void insert_measure_bar(char* s){
  strcat(s,blank_staff()); // add a blank column
  strcat(s,measure_bar()); // add the measure bar
  strcat(s,blank_staff()); // add a blank column
}

// g_clef is 8 columns wide
char* g_clef(){
  return "                 - - - - -                   - - - ||- \\                 - - -/- \\ _                 - - / /\\_ /                /||||||||||                 \\||/ - - _                   - - - \\|/                   - - - - -  ";
}

char* blank_staff(){
  return "                 - - - - -  ";
}

char* measure_bar(){
  return "                 |||||||||  ";
}
char note_symbol(char duration){
  switch(duration){
  case 1: return 'O';
  case 2: return 'O';
  case 4: return '*';
  case 8: return 'e';
  case 16: return 's';
  }
  return '0';
  error("No symbol for duration, or bad duration...");
}

// returns 0 if this note is a natural, returns -1 if its a flat, +1 if sharp
char pitchmod(char* pitch){
  char retval = 0;
  if (strlen(pitch) > 1){
    if (pitch[1] == '#'){
      retval = 1;
    }else if(pitch[1] == 'b'){
      retval = -1;
    }
  }
  return retval;
}

// returns the pitch position index starting at the top and counting down in half steps
char pitchindex(char* pitch, char octave){
  char pos;
 
  switch(toupper(pitch[0])){
  case 'B': pos = 0;break;
  case 'A': pos = 1;break;
  case 'G': pos = 2;break;
  case 'F': pos = 3;break;
  case 'E': pos = 4;break;
  case 'D': pos = 5;break;
  case 'C': pos = 6;break;
  default: pos =-1;
  }

  if ((pos == -1)||((octave > 7)||(octave < 4))) error("Bad pitch or octave value.");
  return pos + (7-octave)*7;
}

// rests are specially located notes with the word 'REST' written in.
char* rest(char duration, char dot){
  char* s = note(duration,"G",5,dot);
  char offset = ((duration == 1)? 1 : 2)+get_padding(duration);
  s[HEIGHT*offset+18] = 'R';
  s[HEIGHT*offset+20] = 'E';
  s[HEIGHT*offset+22] = 'S';
  s[HEIGHT*offset+24] = 'T';
  return s;  
}

char* note(char duration, char* pitch, char octave, char dot){
  // draw a special character if this note is a rest
  if((pitch[0] == 'P')||(pitch[0] == 'p')) { return rest(duration,dot);}
  // get the note's position in half steps counting from the top down
  char pitchpos = pitchindex(pitch,octave);

  // draw a note head and possibly a stem
  char* ptr = draw_note(pitchpos,duration);

  // insert some blank columns for possible dot or sharp/flat
  ptr = pad_note(ptr,1);

  // add lines for notes above or below the staff
  add_lines_outside_staff(ptr,pitchpos);

  // add a dot overwriting lines if necessary
  add_dot(ptr,pitchpos,dot);

  // add a sharp or flat
  add_pitch_mod(ptr,pitchpos,pitchmod(pitch));

  // pad the note with blank columns based on its duration. longer notes take up more space
  ptr = pad_note(ptr, get_padding(duration));

  return ptr;
}

char* draw_note(char pitchpos,char duration){
  char* ptr = malloc(HEIGHT*2+1);
  if (ptr == NULL){error("malloc failed.");}
  sprintf(ptr,"%s",blank_staff()); // print in a blank staff for the note

  if(duration != 1){ // check if this is a note with a stem
    strcat(ptr,blank_staff()); // add space for the vertical part of notes
    if(pitchpos >21){ // stem goes up
      ptr[pitchpos] = note_symbol(duration);
      ptr[HEIGHT+pitchpos-1] = '|';
      ptr[HEIGHT+pitchpos-2] = '|';
    }else{ // stem goes down
      ptr[HEIGHT+pitchpos] = note_symbol(duration);
      ptr[pitchpos+1] = '|';
      ptr[pitchpos+2] = '|';
    }
  }else{
    ptr[pitchpos] = note_symbol(duration);
  }
  return ptr;
}
// inserts lines above and below the staff if necessary
void add_lines_outside_staff(char* ptr,char pitchpos){
  // add lines for notes above / below the staff
  if(pitchpos == HEIGHT-1){ // middle C, add the lines
    ptr[pitchpos] = '-';
    ptr[HEIGHT*2+pitchpos] = '-';
  }else if(pitchpos < 16){ // above the staff
    int i;
    for( i = 17; i > pitchpos; i = i - 2){
      ptr[HEIGHT*2+i] = '-';
    }
    if (pitchpos % 2 == 1){ // don't add lines for 'spaces'
      ptr[HEIGHT+pitchpos] = '-';
      ptr[HEIGHT*3+pitchpos] = '-';
    }
  }
}
// inserts a dot if necessary
void add_dot(char* ptr, char pitchpos,char dot){
  if (dot == 1){
    ptr[HEIGHT*3+pitchpos] = '.';
  }
}
// inserts a sharp or flat as specified by 'mod'.  0 = no modifier, -1 = flat, 1 = sharp
void add_pitch_mod(char* ptr,char pitchpos, char mod){
  // add sharp or flat overwriting lines if necessary
  if (mod == 1){
    ptr[pitchpos] ='#';
  }else if(mod == -1){
    ptr[pitchpos] = 'b';
  }
}

// adds 'columns' number of blank staff columns before and after the given note.
char* pad_note(char* note, char columns){
  char* temp = malloc(strlen(note)+columns*HEIGHT*2+1);
  if (temp == NULL){error("malloc failed.");}

  // generate a padding string of the appropriate length
  char* pad = malloc(HEIGHT*columns);
  if (pad == NULL){error("malloc failed.");}

  pad[0] = '\0';
  int i;
  for (i = 0; i < columns; i++)
    strcat(pad,blank_staff());

  // add the padding string before and after the note
  sprintf(temp,"%s%s%s",pad,note,pad);
  //free(pad);  // this line somehow inserts a very odd bug, sort of folds the output string over on itself a little. no idea why it happens
  return temp;
}

// returns the number of blank columns to add on either side of a note of the given duration.  This lets longer notes take up more space.
char get_padding(char duration){
  switch(duration){
  case 1: return 4;
  case 2: return 3;
  case 4: return 2;
  case 8: return 1;
  case 16: return 0;
  }
  error("Bad Note Duration found when computing padding.");
  return 0;
}

// rotates each column in the string so everything prints out in the correct order
char* reformat_music(char* music){
  int length = strlen(music);
  int num_lines = ceil(((float)length/(float)HEIGHT)/(float)WIDTH)*HEIGHT;
  int memsize = length + 1 + num_lines;
  char* temp = malloc(memsize); //need the extra numlines for \n's
  temp[0] = '\0';
  int i,j;
  int k = 0; // k is the offset needed to handle wrapping
  while(k < length){
    for(i = 0; i < HEIGHT; i++){
      for(j = 0; (j < WIDTH)&&(j < (length-k)/HEIGHT); j++){
	char c[2] = {music[k+j*HEIGHT+i],'\0'};
	strcat(temp,c);
      }
      strcat(temp,"\n");
    }
    k += HEIGHT*WIDTH;
  }
  return temp;
}
