all: clean unhide hide

CFLAGS = -I/usr/include/SDL2
LDFLAGS = -lSDL2

unhide: stego.c unhide.c
	$(CC) -o unhide stego.c unhide.c

hide: stego.c hide.c multiFile.c parallelFile.c displayImage.c
	$(CC) -o hide stego.c hide.c multiFile.c parallelFile.c displayImage.c $(CFLAGS) $(LDFLAGS)

clean:
	rm -f unhide hide out.ppm
