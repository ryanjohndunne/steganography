#ifndef DISPLAYIMAGE_H_FILE
#include <stdio.h>
#define DISPLAYIMAGE_H_FILE

// Decodes a hidden message
// Returns 1 if end found, 0 if not
int decodeMessage(FILE *);
// Decodes multiple messages at once
void multiFileUnhide(int, char** argv);
// Increase the char array's by one to get next file
// returns the string incremented by one
char *increaseByOne(char *);
// Take the default name, append the current file number, append .ppm
// then increase file number by one
void setFileName(char *, char *);
// Sets file names back to their default value so that a new extension can be appended
void defaultFileNames(char*, char*);

#endif
