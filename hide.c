#include "hide.h"

// main method
int main(int argc, char **argv) {
        // Variables
        int temp = 0;
        temp = commandLineCheck(argc);

        // Single file program detected
        if (temp == 0) {
                // Check for correct input of arguments
                if(argv[1][0] == '-' && argv[1][1] == 'p') {
                        parallelProgram(argv);
                }
                if(argv[1][0] == '-' && argv[1][1] == 's') {
                        imageFileProgram(argc, argv);
                }
                else{
                        singleFileProgram(argc, argv);
                }
        }
        // Multi file program detected
        else if(temp == 1) {
                multiFileProgram(argv);
        }

        return 0;
}

// Single file program
void singleFileProgram(int argc, char **argv) {

        // When 4 arguements are detected it's for the parallel process
        // use the filename as stdin
        if (argc == 4) {
                FILE* myfile = fopen(argv[3], "r+");
                // fopen returns 0, the NULL pointer, on failure
                if (myfile == 0) {
                        printf("Could not open file to read from: %s\n", argv[3]);
                        exit(0);
                }
                dup2(fileno(myfile), STDIN_FILENO);
        }
        // We assume argv[1] is a filename to read from
        FILE *fp = fopen(argv[1], "r+");

        // fopen returns 0, the NULL pointer, on failure
        if (fp == 0) {
                printf("Could not open file: %s\n", argv[1]);
                printf("If this seems wrong, check command line arguments\n");
                exit(0);
        }

        // We assume argv[2] is a filename to write to
        FILE *fp_t = fopen(argv[2], "w");
        // fopen returns 0, or NULL, on failure
        if (fp_t == 0) {
                printf("Could not open file: %s\n", argv[2]);
                printf("If this seems wrong, check command line arguments\n");
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
                // Check for comments after the width and height (remove them too)
                comments(fp);
                // allowed maximum colour value
                int maxValue = getMaxValue(fp);

                // Maximum length of message
                int maxMessageLength = (totalPixelValues / 8) - 8;
                char initialMessage[maxMessageLength];
                initialiseArray(initialMessage, maxMessageLength);
                // Sets the message into initialMessage and returns the length
                int size = getSize(initialMessage, &maxMessageLength, argv);
                // Store the message into an array of it's exact size
                char message[size];
                initialiseArray(message, size);
                getMessage(initialMessage, message, &size);

                // Write header
                header(fp, fp_t);
                // Encode message
                encodeMessage(fp, fp_t, size, message, pixelWidth, pixelHeight);
                // Let user know it's done
                printf("Message encoded into file: %s\n", argv[2]);

                // Close files
                fclose(fp);
                fclose(fp_t);
        }
        // Incorrect PPM file detected in the else
        else
        {
                printf("File is in the wrong format");
                exit(0);
        }

        return;
}

// initialise arrays that are used as conditional jumps to all null
void initialiseArray(char *array, int length) {
        for(int i = 0; i < length; i++) {
                array[i] = '\0';
        }

        return;
}

// Check that command line arguments are correct
// Return 0 = single file program (or parallel), 1 = multi file program,
int commandLineCheck(int argc){
        // check for 3 arguements for single file program 4 for parallel
        if (argc == 3 || argc == 4)
        {
                return 0;
        }
        // check for 4 arguements for multi file program
        else if (argc == 5) {
                return 1;
        }
        // Incorrect arguments found
        else {
                // Wrong command amount of arguements detected
                printf("Incorrect command line arguements\n");
                exit(0);
        }
}

// Encode message and print everything else to file
void encodeMessage(FILE *in, FILE *out, int num_to_encode, char* my_message, int w, int h) {
        // Variables
        unsigned char temp = '\0';
        int index = 0, num_coppied = 0, encoded = 0, i = 0, j = 0, nullCheck = 0;
        char current = '\0';
        int fileSize = (w * h * 3);

        // Methods
        // Encode the message into the picture to hide it
        for(i = 0; i < fileSize; i++) {
                // Go through original image char by char
                temp = fgetc(in);

                // Get value of message char by char
                current = my_message[index];

                // current is the current value being hidden
                // starting at the left most bit store it within the images
                // rgb values for each pixel
                current >>= 7 - num_coppied;

                // The amount of characters from the message we've copied to the new file
                num_coppied++;

                // if we still have bits left to encode
                if(encoded <= num_to_encode) {
                        // check the least significant bit
                        // if current value LSB is == 1
                        if((current & 1) == 1) {
                                // if temp LSB == 0
                                if((temp & 1) == 0) {
                                        // increase it to 1, which is what current is
                                        temp++;
                                }
                                // Otherwise decrement temp cause current will be 0
                        } else {
                                if((temp & 1) == 1) {
                                        temp--;
                                }
                        }
                        // if we've done 8 successful copies
                        if(num_coppied == 8) {
                                // go to the next value in the message
                                index++;
                                // reset bits encoded to 0
                                num_coppied = 0;
                        }
                }

                // put a null at the end of the message
                // only do this once or it'll destroy the image
                else if(nullCheck == 0) {
                        // This has to be == 1 or else the preceding 0's Could
                        // cause it to terminate early
                        temp = fgetc(in);
                        if((temp & 1) == 0) {
                                temp++;
                        }
                        fputc(temp, out);

                        // terminate the message by putting 0 in 8 LSB's in a row
                        for(j = 0; j <= 8; j++) {
                                temp = fgetc(in);
                                if((temp & 1) == 1) {
                                        temp--;
                                }
                                fputc(temp, out);
                        }
                        nullCheck = 1;
                }
                // increase count of encoded values
                encoded++;
                fputc(temp, out);
        }
        return;
}

// Get the message so we can determine the length and quit if too long
// returns the size required to store the message
int getSize(char *initialMessage, int *maxMessageLength, char **argv) {
        // Variables
        int c = 0;
        int i = 0;

        // Methods
        // Set the last value in the array to a null
        initialMessage[*maxMessageLength-1] = '\0';

        if (argv[1][0] == '-' && argv[1][1] == 'p') {
                ;
        } else{
                printf("Enter any message shorter than %d letters long, end message with CTRL-D: \n", *maxMessageLength);
        }

        // Keep getting input until EOF (user presses ctrl-D)
        while((c = getchar()) != EOF) {
                initialMessage[i] = c;
                i++;
                if ( i >= *maxMessageLength ) {
                        printf("Error: message is too long\n");
                        exit(0);
                }
        }

        // Check that the last value is still a null
        // if it's no longer a null, they've entered too many letters
        if(initialMessage[*maxMessageLength-1] != '\0') {
                printf("\nError: message is too long");
                exit(0);
        }

        // Increase the size by * 8 because each letter takes 8 bits to store
        int size = (strlen(initialMessage) * 8);

        return size;
}

// Rebuild the array as the exact size needed
// returns the message to be encoded
char *getMessage(char *initialMessage, char *message, int *size) {
        // Variables
        int i = 0;

        // Methods
        // take the original message and store it in an array which is
        // resized to be the exact length of the message, rather than the
        // maxiumum length that a message can be
        while(i < (*size / 8)) {
                message[i] = initialMessage[i];
                i++;
        }

        return message;
}

// Print header information to the new file
void header(FILE *fp, FILE *fp_t) {
        // Variables
        int counter = 0;

        // Assuming that the header lines are no longer than 80 characters
        char line[80] = {0};
        int temp;

        // Methods
        // Return to the start of the file
        rewind(fp);

        // There's 3 values we want, Width/Height/MaxValue, PPM always will be
        // the first value on the first line, so we don't need to check for it
        while(counter < 3)
        {
                // TODO: TODO: TODO
                // Make it check that height, width, max value are all there
                fgets(line, 80, fp);
                fputs(line, fp_t);
                counter++;
        }

        return;
}

// Creates a dynamic string to allow for a message of any length
// returns the message
char *inputString(FILE* fp, size_t size){
        // Variables
        char *str;
        int ch;
        size_t len = 0;

        // Methods
        // Allocate the initial amount of memory, which is 10
        str = realloc(NULL, sizeof(char)*size);
        // If it's not a string return
        if(!str) return str;
        // While not end of file
        while(EOF != (ch=fgetc(fp))) {
                // save character
                str[len++] = ch;
                // Increase the size of the string if it's not finished
                if(len==size) {
                        str = realloc(str, sizeof(char)*(size+=16));
                        if(!str) return str;
                }
        }
        // terminate with a null
        str[len++] = '\0';

        // return the string
        return realloc(str, sizeof(char)*len);
}

// Returns reversed string
char * strrev(char *str)
{
        // i is = size of array - 1 (start at 0 in arrays)
        int i = strlen(str)-1, j=0;

        char ch;
        // Loop through array and reverse it
        while(i>j)
        {
                ch = str[i];
                str[i]= str[j];
                str[j] = ch;
                i--;
                j++;
        }
        return str;
}
