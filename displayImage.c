#include <SDL.h>
#include "displayImage.h"
#include "hide.h"

// Single file program that displays image
void imageFileProgram(int argc, char **argv) {
        // argv[2] is a filename to read from
        FILE *fp = fopen(argv[2], "r+");
        // fopen returns 0, the NULL pointer, on failure
        if (fp == 0) {
                printf("Could not open file: %s\n", argv[1]);
                exit(-1);
        }
        // argv[3] is a filename to write to
        FILE *fp_t = fopen(argv[3], "w");
        // fopen returns 0, or NULL, on failure
        if (fp_t == 0) {
                printf("Could not open file: %s\n", argv[2]);
                exit(-1);
        }

        // If it is a ppm file continue
        if(isPPM(fp)) {
                // Check for comments after the PPM declaration (remove them too)
                comments(fp);
                // Get the pixel width and height
                int pixelWidth = 0;
                int pixelHeight = 0;
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
                printf("Message encoded into file: %s\n", argv[3]);
        }
        // close file (it's in write mode)
        fclose(fp_t);

        // We assume argv[3] is a filename to write to
        FILE *fpTwo= fopen(argv[3], "r+");
        // fopen returns 0, or NULL, on failure
        if (fpTwo == 0) {
                printf("Could not open file: %s\n", argv[2]);
                exit(-1);
        }
        rewind(fp);
        displayImage(argv, fp, fpTwo);

        // Close files
        fclose(fp);
        fclose(fpTwo);

        return;
}

// display the image to user
void displayImage(char **argv, FILE *fp, FILE *fpTwo) {
        // Variables
        SDL_Window *window = NULL;
        SDL_Surface *screenSurface = NULL;
        char temp;
        int j = 0;
        int SCREEN_WIDTH;
        int SCREEN_HEIGHT;
        int red = 0, green = 0, blue = 0;

        // Methods
        // Clear header stuff
        isPPM(fp);
        comments(fp);
        getWidthAndHeight(fp, &SCREEN_WIDTH, &SCREEN_HEIGHT);
        comments(fp);
        int maxValue = getMaxValue(fp);
        // Get rid of the new line after max value
        fgetc(fp);
        // Double height cause two images
        int DoubleScreenHeight = SCREEN_HEIGHT * 2;
        // clear header stuff for second file
        isPPM(fpTwo);
        comments(fpTwo);
        getWidthAndHeight(fpTwo, &SCREEN_WIDTH, &SCREEN_HEIGHT);
        comments(fpTwo);
        maxValue = getMaxValue(fpTwo);
        // Get rid of the new line after max value
        fgetc(fpTwo);

        // Create an SDL window to print images to
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
                printf("Error: SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        } else {
                window = SDL_CreateWindow("Images", SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, DoubleScreenHeight, SDL_WINDOW_SHOWN);

                if (window == NULL) {
                        printf("Error: Window could not be created! SDL_Error: %s\n", SDL_GetError());
                } else {
                        // setup the screen surface to draw on
                        screenSurface = SDL_GetWindowSurface(window);
                        SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xff, 0xff, 0xff));
                        SDL_UpdateWindowSurface(window);
                        for(int i = 0; i < SCREEN_HEIGHT; i++) {
                                for(int j = 0; j < SCREEN_WIDTH; j++) {
                                        int *p = (int *)((char *)screenSurface->pixels +
                                                         i * screenSurface->pitch + j * screenSurface->format->BytesPerPixel);
                                        // draw the pixel
                                        red = fgetc(fp);
                                        green = fgetc(fp);
                                        blue = fgetc(fp);
                                        *p = SDL_MapRGB(screenSurface->format, red, green, blue);
                                }
                        }

                        for(int i = SCREEN_HEIGHT; i < DoubleScreenHeight; i++) {
                                for(int j = 0; j < SCREEN_WIDTH; j++) {
                                        int *p = (int *)((char *)screenSurface->pixels +
                                                         i * screenSurface->pitch + j * screenSurface->format->BytesPerPixel);
                                        // draw the pixel
                                        red = fgetc(fpTwo);
                                        green = fgetc(fpTwo);
                                        blue = fgetc(fpTwo);
                                        *p = SDL_MapRGB(screenSurface->format, red, green, blue);
                                }
                        }
                        // update surface to display what has been drawn
                        SDL_UpdateWindowSurface(window);


                        // Show image for 5000 ms
                        SDL_Delay(5000);
                }

                // destroy the window and quit
                SDL_DestroyWindow(window);
                SDL_Quit();
        }

        return;
}
