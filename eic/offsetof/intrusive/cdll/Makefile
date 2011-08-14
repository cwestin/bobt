EXECUTABLES = idllmain
DEBUG = -ggdb

CFLAGS = -Wall -I$(INCLUDE) $(DEBUG)
CC = gcc

all:	$(EXECUTABLES)

clean:
	rm $(EXECUTABLES) *~ *.o *.exe

idllmain:	idllmain.c idll.h
	$(CC) $(CFLAGS) -o $@ $<
