/* Copyright 2011 Chris Westin.  All rights reserved. */
/*
  Example for discussion from "Data Driven Programming," from
  "The Book of Brilliant Things:"
  https://www.bookofbrilliantthings.com/eic/offsetof/data-driven-programming
 */

#pragma once

#ifndef ARGV_PARSER_H
#define ARGV_PARSER_H

class ArgvParser
{
public:

    enum ArgType
    {
	typeBool = 1;
	typeString = 2;
	typeInt = 2;
    }

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
	const char *const pArg;
    };

    static bool parse(void *pSink, const Descriptor *pDescriptor, size_t nDesc,
		      int argc, const char *const argv[]);
};

#endif // ARGV_PARSER_H
