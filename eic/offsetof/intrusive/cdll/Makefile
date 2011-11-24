EXECUTABLES = idllmain
DEBUG = -ggdb
PHOENIX4C = ../../../../../phoenix4c

INCLUDE = $(PHOENIX4C)/include/
LIBRARY = $(PHOENIX4C)/lib/phoenix4c.a

CFLAGS = -Wall -I$(INCLUDE) $(DEBUG)
CC = gcc

all:	$(EXECUTABLES)

clean:
	-rm $(EXECUTABLES) *~ *.o *.exe

idllmain:	idllmain.c $(INCLUDE)/pxlibDll.h
	$(CC) $(CFLAGS) -o $@ $<
