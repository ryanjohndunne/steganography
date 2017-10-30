#include "multiFile.h"
#include "hide.h"

// Multi file program
void multiFileProgram(char **argv) {
        // Variables
        int numberOfFiles = 0, counter = 0;
        char baseName[50] = "", outBaseName[50] = "";
        char baseNumber[5] = "-000", outNumber[5] = "-000";
        char *defaultBaseName, *defaultOutBaseName, *message, *messageReversed;

        // Methods
        // Check args are correct
        multiFileArgCheck(argv);
        // set number of files
        numberOfFiles = atoi(argv[2]);
        // set base name
        defaultBaseName = argv[3];
        strcat(baseName, defaultBaseName);
        // set output base name
        defaultOutBaseName = argv[4];
        strcat(outBaseName, defaultOutBaseName);
        // If in and out are the same name it won't work
        if (strcmp(baseName, outBaseName) == 0) {
                printf("Error: Cannot have the same base and output name\n");
                exit(0);
        }

        // Get the message from user
        printf("Please enter message: \n");
        messageReversed = inputString(stdin, 10);
        // reverse the message so that we can remove from the end
        messageReversed = strrev(messageReversed);

        // open file
        while(messageReversed[0] !=  '\0') {
                // if this loops through more than 255 images we are to end the encoding
                counter = counterCheck(counter, numberOfFiles);
                // set the file names to be read/written to
                setFileName(baseName, baseNumber);
                setFileName(outBaseName, outNumber);
                // We assume argv[1] is a filename to read from
                FILE *fp = fopen(baseName, "r+");
                // fopen returns 0, the NULL pointer, on failure
                if (fp == 0)
                {
                        printf("Could not open file\n");
                        free(messageReversed);
                        exit(0);
                }
                // We assume argv[2] is a filename to write to
                FILE *fp_t = fopen(outBaseName, "w");
                // fopen returns 0, or NULL, on failure
                if (fp_t == 0)
                {
                        printf("Could not open file\n");
                        free(messageReversed);
                        exit(0);
                }

                // If it is a ppm file continue
                if(isPPM(fp)) {
                        // Check for comments after the PPM declaration (remove them too)
                        comments(fp);
                        // Get the pixel width and height
                        int pixelWidth;
                        int pixelHeight;
                        getWidthAndHeight(fp, &pixelWidth, &pixelHeight);
                        // Calculate the total amount of pixels
                        int totalPixelValues = (pixelHeight * pixelWidth) * 3;
                        int totalCharsThatFit = totalPixelValues / 8;
                        // Check for comments after the width and height (remove them too)
                        comments(fp);
                        // allowed maximum colour value
                        int maxValue = getMaxValue(fp);

                        // size of the message, multiplied by 8 (each letter stored in 8 bits)
                        int lenOfMessageReversed = strlen(messageReversed);
                        // if the size of the message is less than what we can fit in the image
                        if(lenOfMessageReversed < totalCharsThatFit) {
                                // assign message memory the size of messageReversed
                                message = malloc(sizeof(char) * lenOfMessageReversed + 1);
                                // Copy messageReversed into message
                                strcpy(message, messageReversed);
                                // Reverse the string back to correct position
                                message = strrev(message);
                                // Close messageReversed by making the first entry a null
                                messageReversed[0] = '\0';
                        }
                        // if the message isn't going to completely fit in the image
                        else {
                                // Assign space the size of characters that will fit
                                message = malloc(sizeof(char) * totalCharsThatFit + 1);
                                // copy the last X values of the string, x being totalCharsThatFit
                                strcpy(message, &messageReversed[lenOfMessageReversed - totalCharsThatFit]);
                                // reverse the string back to correct position
                                message = strrev(message);
                                // delete the last x values by putting a null at that point in the array
                                messageReversed[(strlen(messageReversed) - totalCharsThatFit)] = '\0';
                        }

                        printf("Message Encoded to: %s\n", outBaseName);
                        // Write header
                        header(fp, fp_t);
                        // Encode message (input file, outputfile, amount of bits to store, the message, pixelWidth, pixelHeight)
                        encodeMessage(fp, fp_t, (strlen(message)*8), message, pixelWidth, pixelHeight);
                        // Close files
                        fclose(fp);
                        fclose(fp_t);
                }
                // Incorrect PPM file detected in the else
                else
                {
                        printf("File is in the wrong format\n");
                        free(messageReversed);
                        exit(0);
                }
                // set base and out base names back to their default values
                defaultFileNames(defaultBaseName, baseName);
                defaultFileNames(defaultOutBaseName, outBaseName);
                // Free message's memory as it will be resized every loop
                free(message);
        }
        // free memory and close files
        free(messageReversed);

        return;
}


// Sets file names back to their default value so that a new extension can be appended
void defaultFileNames(char* defaultName, char* currentName) {
        currentName[0] = '\0';
        strcat(currentName, defaultName);

        return;
}

// Check that number of files is less than 256 and that the loop hasn't passed 255 files
// Returns an incremented value of counter and checks size isn't too large
int counterCheck(int counter, int numberOfFiles) {
        if (numberOfFiles > 255) {
                printf("Error: Number of files is more than 255\n");
                exit(0);
        }
        if(counter > 255 || counter > numberOfFiles) {
                printf("Error: Your message is too long to fit in either the %d files chosen\n"
                "or 255 maximum, remaining characters will not be hidden\n", numberOfFiles);
                exit(0);
        }
        counter++;
        return counter;
}

// Checks for correct flag and arguments for multi file program
void multiFileArgCheck(char **argv) {
        // Check for correct input of argument
        char temp;
        int i = 0;
        if(argv[1][0] != '-' || argv[1][1] != 'm') {
                printf("Incorrect flag passed, use -m\n");
                exit(0);
        }
        // Check for a valid integer for number of files
        while ((temp = argv[2][i]) != 0) {
                if(!isdigit(temp)) {
                        printf("Number of files is not a valid integer\n");
                        exit(0);
                }
                i++;
        }

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
