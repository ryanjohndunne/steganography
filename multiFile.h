#ifndef MULTIFILE_H_FILE
#define MULTIFILE_H_FILE

// the multi file main function
void multiFileProgram(char **argv);
// Sets file names back to their default value so that a new extension can be appended
void defaultFileNames(char *, char *);
// Check that number of files is less than 256 and that the loop hasn't passed 255 files
// Returns an incremented value of counter and checks size isn't too large
int counterCheck(int, int);
// Checks for correct flag and arguments for multi file program
void multiFileArgCheck(char **argv);
// Increase the char array's by one to get next file
// returns the string incremented by one
char *increaseByOne(char *);
// Take the default name, append the current file number, append .ppm
// then increase file number by one
void setFileName(char *, char *);

#endif
