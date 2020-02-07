rm=/bin/rm -f
CC=cc
DEFS=
INCLUDES=-I.
LIBS=

DEFINES= $(INCLUDES) $(DEFS)
CFLAGS= -std=c99 $(DEFINES) -O2 -fomit-frame-pointer -funroll-loops

all: skein_driver

skein_driver: skein_driver.c threefish.o skein.o
	$(CC) $(CFLAGS) -o skein_driver skein_driver.c threefish.o skein.o $(LIBS)

threefish.o: threefish.c
	$(CC) $(CFLAGS) -c threefish.c $(LIBS)

skein.o: skein.c
	$(CC) $(CFLAGS) -c skein.c $(LIBS)

clean:
	$(rm) skein_driver threefish.o skein.o *.o core *~

