EXECUTABLES = idllmain
DEBUG = -ggdb
#OPTIMIZER = -O3
PHOENIX4CPP = ../../../../../phoenix4cpp

INCLUDE = $(PHOENIX4CPP)/include/
LIBRARY = $(PHOENIX4CPP)/lib/phoenix4cpp.a

CFLAGS = -Wall -Wno-invalid-offsetof -I$(INCLUDE) $(DEBUG) $(OPTIMIZER)
CC = g++

all:	$(EXECUTABLES)

clean:
	-rm $(EXECUTABLES) *~ *.o *.exe

idllmain:	idllmain.cpp $(INCLUDE)/DoublyLinked.h $(LIBRARY)
	$(CC) $(CFLAGS) -o $@ $< -Wl,-l:$(LIBRARY)
