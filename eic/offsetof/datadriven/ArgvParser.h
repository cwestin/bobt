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

/*
  ArgvParser is a command-line argument parser.

  This implementation is not full-featured, but provides simple functionality
  sufficient to demonstrate discussion in the article cited above.

  To use the parser, the caller provides the parse() function with an array of
  argument descriptors.  Each element in the descriptor array provides
  information about the argument, including its short (single-character) form,
  its long name, its data type, and the offset of where to put the value found,
  if any.  The offset is to a location within an argument structure which is
  also passed in to the parser.

  Before the calling program exits, it should also call the clean() function
  to free any memory allocated by the parser.
 */
class ArgvParser
{
public:

    /*
      These are the argument types supported by the parser.
     */
    enum ArgType
    {
        typeBool = 1, /* single value "flags," such as "-c", or "--myoption" */
        typeString = 2, /* for flags that precede a string argument */
        typeInt = 3, /* for options that precede an integer argument */
    };

    static const unsigned optionMulti = 0x00000001;
    /*
      For arguments of typeString, this will cause the parser to use a list of
      StringArg (defined below) instances, allowing the parser to capture
      multiple values.  The option must repeat before each one, as
        progname -x foo -x bar -x zed
     */
    
    /*
      Argument descriptor.

      An argument can have a short name (a single printable character followed
      by a '\0'), a long name (a pointer to a character string), or both.

      The offset is the location of where to put the value of the argument
      in the structure passed in at parse() time.

      By default, boolean arguments are set to false.  They are only set to
      true if they are found.  String arguments are set to NULL unless a value
      is found for them.  For integer values, the intDefault value is used
      unless a value is found for them.
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

    /*
      Utility structure used to create a linked list of string argument values.
     */
    struct StringArg
    {
        struct StringArg *pNext;
        const char *pArg;
    };

    /*
      Utility service class for reporting errors.

      The caller is required to pass in a concrete implementation of this
      class in order for errors that are found to be reported.
     */
    class Error
    {
    public:
        virtual ~Error() {};
        virtual void report(const Descriptor *pD, const char *pMsg) = 0;
    };

    /*
      parse command line arguments

      Boolean and integer values are set as found.  String values are set or
      gathered into a list.

      The Descriptor array must be sorted by pLongName values; any NULL
      pLongName values must appear first.  The very first member of the
      descriptor array should have neither a short name nor a long name, and
      must be string-valued and specify the optionsMulti option; this
      will be used to locate a list pointer that will hold all free arguments
      that are not preceded by one of the declared argument names.

      @param pSink pointer to a structure in which to place the found arguments;
        any particular argument will be put at the offset specified in its
        argument descriptor
      @param pError pointer to a class to use to report parsing errors
      @param pDescriptor base of an array of argument descriptors
      @param nDesc count of entries in the pDescriptor array
      @param argc count of arguments
      @param argv array of string pointers to arguments
      @returns true if there are no parse errors, false otherwise
     */
    static bool parse(void *pSink, Error *pError,
                      const Descriptor *pDescriptor, size_t nDesc,
                      int argc, const char *const argv[]);

    /*
      clean up any memory allocated by parse()

      Options such as optionMulti will create additional structures.  This
      function will clean them up and deallocate them.

      Note that this does not deallocate any of the argument values themselves;
      argv values are owned by main()'s caller, and are left untouched.

      @param pSink pointer to a structure in which to place the found arguments;
        any particular argument will be at the offset specified in its
        argument descriptor
      @param pDescriptor base of an array of argument descriptors
      @param nDesc count of entries in the pDescriptor array
     */
    static void clean(void *pSink, const Descriptor *pDescriptor, size_t nDesc);
};

#endif // ARGV_PARSER_H
