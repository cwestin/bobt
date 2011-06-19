EXECUTABLES = searchsort tcompare
DEBUG = -ggdb
PHOENIX4CPP = ../../../../phoenix4cpp

INCLUDE = $(PHOENIX4CPP)/include/
LIBRARY = $(PHOENIX4CPP)/lib/phoenix4cpp.a

CFLAGS = -Wall -I$(INCLUDE) $(DEBUG)
CC = g++

all:	$(EXECUTABLES)

clean:
	rm $(EXECUTABLES) *~ *.exe

searchsort:	searchsort.cpp
	$(CC) $(CFLAGS) -o $@ $< -Wl,-l:$(LIBRARY)

tcompare:	tcompare.cpp
	$(CC) $(CFLAGS) -o $@ $< -Wl,-l:$(LIBRARY)

