
DEBUG = -g

all:	testc testcpp

testc:	testc.c

testcpp:	testcpp.cpp

.c.:
	gcc $(DEBUG) -o $@ $@.c

.cpp.:
	g++ ($DEBUG) -o $@ $@.cpp
