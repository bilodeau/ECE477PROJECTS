#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>

#ifndef STRUCT_FOR_HEADER
#define STRUCT_FOR_HEADER
// struct to hold a single note
struct note {
   	int divisor;
   	int cycles;
    	int octave;
   	int dot;
	char pitch[3];
	int length;
};
// struct to hold default values
struct defaults {
  	int bpm;
   	int len;
   	int oct;
};
#endif
