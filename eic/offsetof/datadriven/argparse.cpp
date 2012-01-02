/* Copyright 2011 Chris Westin.  All rights reserved. */
/*
  Example for discussion from "Data Driven Programming," from
  "The Book of Brilliant Things:"
  https://www.bookofbrilliantthings.com/eic/offsetof/data-driven-programming
 */

#include <cstdio>
#include "ArgvParser.h"

struct MyArgs
{
    int i1;
    bool c;
    int i2;
    int i;
    int i3;
    char *pS;
    int i4;
    ArgvParser::StringArg *pStringList;
    ArgvParser::StringArg *pNakedArgs;
};

static const ArgvParser::Descriptor argDesc[] =
{
    /* these must be in alphabetical order by long name (second initializer) */
    {"", NULL, ArgvParser::typeString, ArgvParser::optionMulti,
     offsetof(MyArgs, pNakedArgs), 0},
    {"c", "cflag", ArgvParser::typeBool, 0, offsetof(MyArgs, c), 0},
    {"i", "ioption", ArgvParser::typeInt, 0, offsetof(MyArgs, i), 42},
    {"l", "loption", ArgvParser::typeString, ArgvParser::optionMulti,
     offsetof(MyArgs, pStringList), 0},
    {"s", "soption", ArgvParser::typeString, 0, offsetof(MyArgs, pS), 0},
};
static const size_t argCount = sizeof(argDesc)/sizeof(argDesc[0]);


class MyError :
    public ArgvParser::Error
{
public:
    // virtuals from ArgvParser::Error
    virtual void report(const ArgvParser::Descriptor *pDesc,
			const char *pMsg);

    MyError(const char *a0);

private:
    const char *argv0;
};

void MyError::report(const ArgvParser::Descriptor *pDesc,
		     const char *pMsg)
{
    printf("%s: %s\n", argv0, pMsg);
}

MyError::MyError(const char *a0):
    argv0(a0)
{
}


int main(int argc, char *argv[])
{
    MyArgs myArgs;
    MyError myError(argv[0]);

    if (!ArgvParser::parse(&myArgs, &myError, argDesc, argCount, argc, argv))
	return -1;

    /* show us what we got */
    if (myArgs.c)
	printf(" -c\n");

    printf(" -i %i\n", myArgs.i);

    if (myArgs.pStringList)
    {
	const ArgvParser::StringArg *pSA = myArgs.pStringList;
	for(; pSA; pSA = pSA->pNext)
	    printf(" -l %s\n", pSA->pArg);
    }

    if (myArgs.pS)
	printf(" -s %s\n", myArgs.pS);

    if (myArgs.pNakedArgs)
    {
	const ArgvParser::StringArg *pSA = myArgs.pNakedArgs;
	for(; pSA; pSA = pSA->pNext)
	    printf(" %s\n", pSA->pArg);
    }

    /* clean up any allocated memory */
    ArgvParser::clean(&myArgs, argDesc, argCount);

    return 0;
}
