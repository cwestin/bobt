/* Copyright 2011 Chris Westin.  All rights reserved. */
/*
  Example for discussion from "Data Driven Programming," from
  "The Book of Brilliant Things:"
  https://www.bookofbrilliantthings.com/eic/offsetof/data-driven-programming
 */

#include "ArgvParser.h"

#include <cassert>
#include "bsearch.h"

using namespace phoenix4cpp;

static void reverseStringList(StringArg **ppList)
{
    StringArg *pNew = NULL;
    while(*ppList != NULL)
    {
	/* take one off the list */
	StringArg *pTemp = *ppList;
	*ppList = pTemp->pNext;

	/* add it to the new list */
	pTemp->pNext = pNew;
	pNew = pTemp;
    }

    /* put the new list in place */
    *ppList = pNew;
}

static void addStringArg(
    char *pSink, const Descriptor *pD, const char *const pArg)
{
    StringArg *pNewArg = new StringArg;
    pNewArg->pArg = pArg;

    StringArg **ppList = (StringArg **)(pSink + pD->offset);
    pNewArg->pNext = *ppList;
    *ppList = pNewArg;
}

static bool processArgument(char *pSink, const Descriptor *const pDesc,
			    int argc, char *argv[], unsigned iArg)
{
    $$$;
}

bool ArgvParser::parse(void *pS, const Descriptor *const pDesc, const size_t nD,
		       int argc, const char *const argv[])
{
    char *pSink = (char *)pS; /* make pointer arithmetic easier for setting */
    const Descriptor *pNakedDesc = NULL;
    const Descriptor *pCharIndex[256];

    /* loop variables we reuse */
    const Descriptor *pD;
    int i;

    /* initialize the character option index */
    for(pD = pCharIndex; i = sizeof(pCharIndex)/sizeof(Descriptor *); i;
	++pD, --i)
	*pD = NULL;

    /* make an initial pass over the descriptor to initialize things */
    for(pD = pDesc; i = nD; i; ++pD, --i)
    {
	/* build an index for the single character options */
	if (pD->shortName[0] != '\0')
	{
	    const char c = pD->shortName[0];
	    assert(!pCharIndex[c]); /* this option used twice */
	    assert(isprint(c) && !isspace(c));/* should be a usable character */
	    pCharIndex[(unsigned)c] = pD;
	}

	/* have we got a place to gather naked arguments? */
	if (!pD->pLongName && (pD->shortName[0] == '\0'))
	{
	    assert(!pNakedDesc); /* two naked descriptors supplied */
	    pNakedDesc = pD;
	}

	/* initialize the arguments in the descriptor */
	switch(pD->argType)
	{
	case typeBool:
	    assert(!(pD->options & optionMulti));
	    *(bool *)(pSink + pD->offset) = false;
	    break;

	case typeString:
	    if (pD->options & optionMulti)
		*(StringArg **)(pSink + pD->offset) = NULL;
	    else
		*(char **)(pSink + pD->offset) = NULL;
	    break;

	case typeInt:
	    assert(!(pD->options & optionMulti));
	    *(int *)(pSink + pD->offset) = pD->intDefault;
	    break;
	}
    }

    /* make sure we have a place to stash naked arguments */
    assert(pNakedDesc);
    assert(pNakedDesc->options & optionMulti);

    /* go through the arguments */
    for(i = 1; i < argc; ++i)
    {
	const char *const pArg = argv[i];

	/* skip over anything empty */
	if (!pArg || (pArg[0] == '\0'))
	    continue;

	/* gather naked string arguments */
	if (pArg[0] != '-')
	{
	    addStringArg(pSink, pNakedDesc, pArg);
	    continue;
	}

	/* is it just a lone hyphen? */
	if (pArg[1] == '\0')
	{
	    error(i);
	}

	/* is it a long-name argument? */
	if (pArg[1] == '-')
	{
	    /* is the argument name missing? */
	    if (pArg[2] == '\0')
	    {
		error(argv, i);
	    }

	    /* find it in the descriptor array */
	    const Descriptor *pArgDesc =
		bsearch<Descriptor, charstar, offsetof(Descriptor, pLongName)>(
		    &pArg[2], pDesc, nD);
	    if (!pArgDesc)
		error(argv, i);
				     
	    bool twosie = processArgument(pSink, pArgDesc, argc, argv, i);
	    if (twosie)
		++i;
	}

	/* if we got here, it's one or more short-name arguments */
	for(const char *pC = pArg + 1; *pC; ++pC)
	{
	    /* look for a matching descriptor */
	    const Descriptor *pArgDesc = pCharIndex[(unsigned)*pC];
	    if (!pArgDesc)
	    {
		error(argv, i);
	    }

	    if (pArgDesc->argType == typeBool)
	    {
		*(bool *)(pSink + pD->offset) = true;
		continue;
	    }

	    /*
	      If we got here, the argument type is not boolean.  The non-
	      boolean types don't allow adjacent specification without spaces
	      (and the requisite intervening values).  Therefore, this had
	      better be the last flag in this set, i.e.,  "-abcd foo"
	      where 'a', 'b', and 'c' are boolean, and d expects the
	      sub-argument 'foo'.
	     */
	    if (pC[1] != '\0')
	    {
		error(argv, i);
	    }

	    bool twosie = processArgument(pSink, pArgDesc, argc, argv, i);
	    if (twosie)
		++i;
	}
    }

    /* make a final pass through the descriptors to finish up */
    for(pD = pDesc; i = nD; i; ++pD, --i)
    {
	/* do we need to reverse the list? */
	if (pD->options & optionMulti)
	    reverseStringList((StringArg **)(pSink + pD->offset));
    }

    /* if we got here, everything went ok */
    return true;
}
