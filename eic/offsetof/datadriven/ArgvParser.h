/* Copyright 2011 Chris Westin.  All rights reserved. */
/*
  Example for discussion from "Data Driven Programming," from
  "The Book of Brilliant Things:"
  https://www.bookofbrilliantthings.com/eic/offsetof/data-driven-programming
 */

#pragma once

#ifndef ARGV_PARSER_H
#define ARGV_PARSER_H

#include <cstddef>

class ArgvParser
{
public:

    enum ArgType
    {
	typeBool = 1,
	typeString = 2,
	typeInt = 3,
    };

    static const unsigned optionMulti = 0x00000001;
    /*
      For arguments of typeString, this will cause the parser to use a list of
      StringArg (defined below) instances.
     */
    
    struct Descriptor
    {
	char shortName[2];
	const char *pLongName;
	ArgType argType;
	unsigned options; /* logical OR of option* values from above */
	size_t offset;
	int intDefault;
    };

    struct StringArg
    {
	struct StringArg *pNext;
	const char *pArg;
    };

    class Error
    {
    public:
	virtual ~Error() {};
	virtual void report(const Descriptor *pD, const char *pMsg) = 0;
    };

    static bool parse(void *pSink, Error *pError,
		      const Descriptor *pDescriptor, size_t nDesc,
		      int argc, const char *const argv[]);

    static void clean(void *pSink, const Descriptor *pDescriptor, size_t nDesc);
};

#endif // ARGV_PARSER_H
