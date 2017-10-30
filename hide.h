#ifndef HIDE_H_FILE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Allows for process handling
#include <unistd.h>
// Allow for waiting on process
#include <sys/wait.h>
// ctype allows to check isdigit
#include <ctype.h>
#include "stego.h"
#include "multiFile.h"
#include "parallelFile.h"
#include "displayImage.h"
#define HIDE_H_FILE

// Get's size of message
int getSize(char *, int *, char **argv);
// Check that command line arguments are correct
// Return 0 = single file program (or parallel), 1 = multi file program,
int commandLineCheck(int);
// Get's users message
char *getMessage(char *, char *, int *);

char *inputString(FILE*, size_t);

char *strrev(char *);

void header(FILE *, FILE *);

void encodeLength(FILE *, FILE *, int);

// Encode message and print everything else to file
void encodeMessage(FILE *, FILE *, int, char *, int, int);

void singleFileProgram(int, char **argv);

// initialise arrays that are used as conditional jumps to all null
void initialiseArray(char *, int);

#endif
