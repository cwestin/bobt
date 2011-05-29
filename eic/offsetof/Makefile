EXECUTABLES = testc testcpp
DEBUG = -g

all:	$(EXECUTABLES)

clean:
	rm ls$(EXECUTABLES)

testc:	testc.c

testcpp:	testcpp.cpp

.c.:
	gcc $(DEBUG) -o $@ $@.c

.cpp.:
	g++ ($DEBUG) -o $@ $@.cpp
