#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "stego.h"

// Return 1 if PPM file, else return 0
int isPPM(FILE *fp)
{
    // Check first value for a P
    if(fgetc(fp) == 'P')
    {
        // Check second value for a 6
        if(fgetc(fp) == '6')
        {
            // clear white space after ppm declaration
            fgetc(fp);
            // True
            return 1;
        }
    }
    return 0;
}

// We don't want any information from comments
void comments(FILE *fp)
{
    // Variables
    char temp;

    // Methods
    // get char to look for a hash (the symbol that starts a comment)
    temp = fgetc(fp);
    // Each time that there's a # at the start
    while(temp == '#')
    {
        // read the rest of the line, to get rid of it
        while((temp = fgetc(fp) != '\n')) {}
        // get the first value for the next row, check for another comment
        temp = fgetc(fp);
    }
    // else put the char back that we just took
    ungetc(temp, fp);

    return;
}

// Sets height and width of file
// Width is the first value in PPM format
void getWidthAndHeight(FILE *fp, int *pixelWidth, int *pixelHeight){
    // Variables
    char temp[10] = {0};
    int value = 0;
    int currValue;
    int i;
    int test = 0;

    // Methods
    // Get first number
    // fscanf returns the number of correctly read in values
    // as we are reading one value, it must be == 1 to be correct
    if(fscanf(fp, "%s", temp) != 1)
    {
        printf("Error: Can't read width\n");
        exit(0);
    }
    else
    {
        // Get each array entry
        for(i = 0; temp[i] != 0; i++){
            // Convert to int
            currValue = temp[i] - '0';
            // 0-9 is numbers, anything greater shows that we've found a letter
            if(currValue > 9){
                printf("Error: Invalid width value\n");
                exit(0);
            }
            // Save into value
            value = (value * 10) + currValue;
        }
        // Set width
        *pixelWidth = value;
        // Reset value
        value = 0;
    }

    // Get second number
    // fscanf returns the number of correctly read in values
    // as we are reading one value, it must be == 1 to be correct
    if(fscanf(fp, "%s", temp) != 1)
    {
        printf("Error: Can't read height\n");
        exit(0);
    }
    else
    {
        // Get each array entry
        for(i = 0; temp[i] != 0; i++){
            // Convert to int
            currValue = temp[i] - '0';
            // 0-9 is numbers, anything greater shows that we've found a letter
            if(currValue > 9){
                printf("Error: Invalid height value\n");
                exit(0);
            }
            // Save into value
            value = (value * 10) + currValue;
        }
        // Set width
        *pixelHeight = value;
        // Reset value
        value = 0;
    }

    return;
}

// Returns the maximum value colour an entry can be
int getMaxValue(FILE *fp) {
    // Variables
    char temp[10] = {0};
    int value = 0;
    int currValue = 0;
    int i;

    // Get max colour value
    // fscanf returns the number of correctly read in values
    // as we are reading one value, it must be == 1 to be correct
    if(fscanf(fp, "%s", temp) != 1)
    {
        printf("Error: Can't read max colour value\n");
        exit(0);
    }
    else
    {
        // Get each array entry
        for(i = 0; temp[i] != 0; i++){
            // Convert to int
            currValue = temp[i] - '0';
            // 0-9 is numbers, anything greater shows that we've found a letter
            if(currValue > 9){
                printf("Error: Invalid max colour value\n");
                exit(0);
            }
            // Save into value
            value = (value * 10) + currValue;
        }
    }

    return value;
}
