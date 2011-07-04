EXECUTABLES = argparse
DEBUG = -ggdb
PHOENIX4CPP = ../../../../phoenix4cpp

INCLUDE = $(PHOENIX4CPP)/include/
LIBRARY = $(PHOENIX4CPP)/lib/phoenix4cpp.a

CFLAGS = -Wall -I$(INCLUDE) $(DEBUG)
CC = g++

all:	$(EXECUTABLES)

clean:
	rm $(EXECUTABLES) *~ *.o *.exe

ArgvParser.o:	ArgvParser.cpp ArgvParser.h
	$(CC) $(CFLAGS) -c $<

argparse:	argparse.cpp ArgvParser.o
	$(CC) $(CFLAGS) -o $@ $< ArgvParser.o -Wl,-l:$(LIBRARY)
