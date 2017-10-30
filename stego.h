#ifndef STEGO_H_FILE
#include <stdio.h>
#define STEGO_H_FILE

//Check that the file is P6 format
int isPPM(FILE *);
//Skips past the comments in the file
void comments(FILE *);
// get width and height
void getWidthAndHeight(FILE *, int *, int *);
// get the max colour value
int getMaxValue(FILE *);
//Returns 1 if 255, else returns 0
int read_color_depth(FILE *);

#endif
