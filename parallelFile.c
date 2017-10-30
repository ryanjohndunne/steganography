#include "parallelFile.h"
#include "hide.h"

// Parallel program
void parallelProgram(char **argv) {
        // Variables
        int i = 0;
        char *token, *textFile;
        char delim[3] = " \n";
        // Initialise all to null
        char line[150] = "";
        // array[size of array][size of strings]
        char messageText[50][80], inPPM[50][80], outPPM[50][80];

        // Methods
        // Get the text file that holds information to read in
        textFile = argv[2];

        // Open up the text file to read from
        FILE *fp = fopen(textFile, "r+");
        // fopen returns 0, the NULL pointer, on failure
        if (fp == 0)
        {
                printf("Could not open file: %s\n", textFile);
                exit(-1);
        }

        // check that it's correctly formated
        correctFormatFile(fp);
        // Read in from file and save to the arrays
        printf("Reading from file: %s\n", textFile);
        while(fgets(line, 150, fp) != '\0') {
                // first token - message.txt
                token = strtok(line, delim);
                strcpy(&messageText[i][0], token);
                // second token - in.ppm
                token = strtok(NULL, delim);
                strcpy(&inPPM[i][0], token);
                // third token - out.ppm
                token = strtok(NULL, delim);
                strcpy(&outPPM[i][0], token);
                printf("Message File: %s, PPM in: %s, PPM out:%s\n", messageText[i], inPPM[i], outPPM[i]);

                // Move to next value in array
                i++;
                if (i >= 48) {
                        printf("Error: more than 50 lines in file document, exiting\n");
                        exit(-1);
                }
        }
        // Terminate the arrays early if they weren't completely used (or at array-1 if they were)
        messageText[i][0] = '\0', inPPM[i][0] = '\0', outPPM[i][0] = '\0';
        i = 0;

        // Create args, assign space, populate - 4 arguments are used
        char *args[5];
        char *hide = "./hide";
        args[4] = NULL;

        while(messageText[i][0] != '\0') {
                // assign memory and copy into space (+1 for the null)
                args[0] = malloc(sizeof(char) * strlen(hide) + 1);
                args[3] = malloc(sizeof(char) * strlen(&messageText[i][0]) + 1);
                args[1] = malloc(sizeof(char) * strlen(&inPPM[i][0]) + 1);
                args[2] = malloc(sizeof(char) * strlen(&outPPM[i][0]) + 1);
                strcpy(args[0], hide);
                strcpy(args[3], &messageText[i][0]);
                strcpy(args[1], &inPPM[i][0]);
                strcpy(args[2], &outPPM[i][0]);

                // create Fork
                pid_t pid = fork();
                // Returns -1 on Error
                if (pid == -1) {
                        perror ("Fork\n");
                }
                // Create Child
                if (pid == 0) {
                        if (execvp (args[0], args) == -1) {
                                perror ("Exec");
                        }
                }
                // wait for child process to join with parent
                if (pid > 0) {
                        wait(0);
                }

                free(args[3]), free(args[1]), free(args[2]), free(args[0]);
                i++;
        }

        // Close file and return
        fclose(fp);
        exit(0);
        return;
}

// Checks that the format of the file is correct for parallel execution
void correctFormatFile(FILE *fp){
        char *token;
        char line[150] = "";
        char delim[3] = " \n";
        int i = 0;

        while(fgets(line, 150, fp)) {
                // first token - message.txt
                token = strtok(line, delim);
                while(token != NULL) {
                        // second token - in.ppm
                        token = strtok(NULL, delim);
                        i++;
                }
                if (i > 3) {
                        printf("Error: Incorrect formatting of file for parallel execution\n");
                        exit(0);
                }
                i = 0;
        }

        rewind(fp);
}
