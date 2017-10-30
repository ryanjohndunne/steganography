#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stego.h"
#include "multiFile.h"
#include "unhide.h"

int main(int argc, char **argv) {
        if(argc < 2 || argc > 3) {
                printf("Wrong amount of arguments\n");
                exit(0);
        }

        if (argc == 3) {
                if(argv[1][0] == '-' && argv[1][1] == 'm') {
                        multiFileUnhide(argc, argv);
                        return 0;
                }
        }
        else{
                // We assume argv[1] is a filename to open
                FILE *fp = fopen(argv[1], "rb");
                // fopen returns 0, the NULL pointer, on failure
                if (fp == 0)
                {
                        printf("Could not open file, or incorrect arguments\n");
                        exit(0);
                }

                if(isPPM(fp)) {
                        // Check for comments after PPM declaration (remove them too)
                        comments(fp);

                        // Get the width and height of image
                        int pixelWidth;
                        int pixelHeight;
                        getWidthAndHeight(fp, &pixelWidth, &pixelHeight);

                        // Check for comments and remove
                        comments(fp);

                        // Get the maximum colour value
                        int maxValue = getMaxValue(fp);
                        int temp = decodeMessage(fp);
                }
                fclose(fp);

        }
        return 0;
}

void multiFileUnhide(int argc, char **argv) {
        char baseName[50] = "";
        char baseNumber[5] = "-000";
        char *defaultBaseName;
        defaultBaseName = argv[2];
        strcat(baseName, defaultBaseName);

        int endOfMessageCheck = 0;
        while(endOfMessageCheck < 5) {
                setFileName(baseName, baseNumber);
                // We assume argv[2] is a filename to open
                FILE *fp = fopen(baseName, "rb");
                // fopen returns 0, the NULL pointer, on failure
                if (fp == 0)
                {
                        printf("\nMessage ended or file doesn't exist\n");
                        exit(0);
                }
                if(isPPM(fp)) {
                        // Check for comments after PPM declaration (remove them too)
                        comments(fp);
                        // Get the width and height of image
                        int pixelWidth;
                        int pixelHeight;
                        getWidthAndHeight(fp, &pixelWidth, &pixelHeight);
                        // Check for comments and remove
                        comments(fp);
                        // Get the maximum colour value
                        int maxValue = getMaxValue(fp);
                        // Decode the message
                        endOfMessageCheck = decodeMessage(fp);
                }
                defaultFileNames(defaultBaseName, baseName);
                fclose(fp);
        }
        printf("\n\n%d\n\n", endOfMessageCheck);
        return;
}

// Sets file names back to their default value so that a new extension can be appended
void defaultFileNames(char* defaultName, char* currentName) {
        currentName[0] = '\0';
        strcat(currentName, defaultName);

        return;
}

// Increase the char array's by one to get next file
// returns the string incremented by one
char *increaseByOne(char *toBeIncremented) {
        // if the right most value is 9, change back to 0 and increment ten value
        if(toBeIncremented[3] == '9') {
                toBeIncremented[3] = '0';
                // if ten value is also 9 increment hundred value
                if(toBeIncremented[2] == '9') {
                        toBeIncremented[2] = '0';
                        toBeIncremented[1] = toBeIncremented[1] + 1;
                        return toBeIncremented;
                }
                // otherwise just increment the ten value
                else {
                        toBeIncremented[2] = toBeIncremented[2] + 1;
                        return toBeIncremented;
                }
        }
        // otherwise just increment the right most value
        else {
                toBeIncremented[3] = toBeIncremented[3] + 1;
                return toBeIncremented;
        }

        return "Error: incrementing";
}

// Take the default name, append the current file number, append .ppm
// then increase file number by one
void setFileName(char* name, char* number) {
        strcat(name, number);
        strcat(name, ".ppm");
        // Increase file number by one
        *number = *increaseByOne(number);

        return;
}

// keep searching untill 8 0's are found in LSB or EOF
int decodeMessage(FILE *fp) {
        // Variables
        char temp;
        int counter = 0;
        unsigned char charBuffer = 0;
        int numRead = 0, idx = 0, endOfMessageCheck = 0;
        char value[2];


        // Methods
        // first value is junk
        fgetc(fp);

        while((temp = fgetc(fp)) != EOF) {
                if((temp&1) == 0) {
                        counter++;
                }
                else if((temp&1) == 1) {
                        counter = 0;
                }
                if(counter >= 8) {
                        printf("\nEnd Message\n");
                        return 0;
                }
                if(numRead % 8 == 0) {
                        value[0] = charBuffer;
                        // because it terminates with 0's, it will find the @ symbol first
                        if(value[0] == 64) {
                                temp = fgetc(fp);
                                if((temp&1) == 0) {
                                        endOfMessageCheck = 1;
                                        return endOfMessageCheck;
                                }
                                else{
                                        printf("@");
                                }
                        }
                        printf("%c", value[0]);
                        idx++;
                        charBuffer = 0;
                } else {
                        charBuffer <<= 1;
                }
                charBuffer |= (temp & 1);
                numRead++;
        }
        return endOfMessageCheck;
}
