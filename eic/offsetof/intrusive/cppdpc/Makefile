EXECUTABLES = dpcmain
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

HashMap.o:	HashMap.cpp HashMap.h
	$(CC) $(CFLAGS) -c $<

DiskPageCache.o:	DiskPageCache.cpp DiskPageCache.h HashMap.h $(INCLUDE)/DoublyLinked.h
	$(CC) $(CFLAGS) -c $<

dpcmain:	dpcmain.cpp DiskPageCache.o HashMap.o \
	$(INCLUDE)/DoublyLinked.h \
	$(INCLUDE)/compare.h \
	$(INCLUDE)/hash.h \
	$(LIBRARY)
	$(CC) $(CFLAGS) -o $@ $< DiskPageCache.o HashMap.o -Wl,-l:$(LIBRARY)
