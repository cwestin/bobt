/* Copyright 2011 Chris Westin.  All rights reserved. */
/*
  Example for discussion from "Data Driven Programming," from
  "The Book of Brilliant Things:"
  https://www.bookofbrilliantthings.com/eic/offsetof/data-driven-programming
 */

#include "ArgvParser.h"

#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "bsearch.h"

using namespace phoenix4cpp;

static void reverseStringList(ArgvParser::StringArg **ppList)
{
    ArgvParser::StringArg *pNew = NULL;
    while(*ppList != NULL)
    {
	/* take one off the list */
	ArgvParser::StringArg *pTemp = *ppList;
	*ppList = pTemp->pNext;

	/* add it to the new list */
	pTemp->pNext = pNew;
	pNew = pTemp;
    }

    /* put the new list in place */
    *ppList = pNew;
}

static void addStringArg(
    char *pSink, const ArgvParser::Descriptor *pD, const char *const pArg)
{
    ArgvParser::StringArg *pNewArg = new ArgvParser::StringArg;
    pNewArg->pArg = pArg;

    ArgvParser::StringArg **ppList =
	(ArgvParser::StringArg **)(pSink + pD->offset);
    pNewArg->pNext = *ppList;
    *ppList = pNewArg;
}

static void reportError(ArgvParser::Error *pError,
			const ArgvParser::Descriptor *pDesc, const char *pMsg)
{
    char optionbuf[128];
    char buf[256];

    if (pDesc->shortName[0])
    {
	if (pDesc->pLongName)
	    snprintf(optionbuf, sizeof(optionbuf),
		     "the -%s/--%s", pDesc->shortName,
		     pDesc->pLongName);
	else
	    snprintf(optionbuf, sizeof(optionbuf),
		     "the -%s", pDesc->shortName);
    }
    else
	snprintf(optionbuf, sizeof(optionbuf),
		 "the --%s", pDesc->pLongName);

    snprintf(buf, sizeof(buf), "%s option %s", optionbuf, pMsg);
    pError->report(pDesc, buf);
}

static bool processArgument(char *pSink, ArgvParser::Error *pError,
			    const ArgvParser::Descriptor *const pDesc,
			    int argc, const char *const argv[], unsigned iArg)
{
    switch(pDesc->argType)
    {
    case ArgvParser::typeBool:
	*(bool *)(pSink + pDesc->offset) = true;
	return false;

    case ArgvParser::typeString:
	/* we expect a follow-on argument */
	if (iArg + 1 >= (unsigned)argc)
	{
	    reportError(pError, pDesc, "requires a follow-on argument");
	    return false;
	}

	if (pDesc->options & ArgvParser::optionMulti)
	    addStringArg(pSink, pDesc, argv[iArg + 1]);
	else
	{
	    const char **ppS = (const char **)(pSink + pDesc->offset);
	    if (*ppS)
		reportError(pError, pDesc, "is specified more than once");
	    *(const char **)(pSink + pDesc->offset) = argv[iArg + 1];
	}

	return true;
	
    case ArgvParser::typeInt:
    {
	/* we expect a follow-on argument */
	if (iArg + 1 >= (unsigned)argc)
	{
	    reportError(pError, pDesc, "requires a follow-on argument");
	    return false;
	}

	const char *pN = argv[iArg + 1];
	for(const char *pC = pN; *pC; ++pC)
	{
	    if (!isdigit(*pC))
	    {
		reportError(pError, pDesc, "requires an integer argument");
		return true;
	    }
	}

	*(int *)(pSink + pDesc->offset) = atoi(pN);
	return true;
    }
    }

    /* NOTREACHED */
    return false;
}

bool ArgvParser::parse(void *pS, Error *pError,
		       const Descriptor *const pDesc, const size_t nD,
		       int argc, const char *const argv[])
{
    char *pSink = (char *)pS; /* make pointer arithmetic easier for setting */
    const Descriptor *pNakedDesc = NULL;

    /* loop variables we reuse */
    const Descriptor **ppD;
    const Descriptor *pD;
    int i;

    /* an index into the descriptors by short name character */
    const Descriptor *pCharIndex[256];

    /* initialize the character option index */
    for(ppD = pCharIndex, i = sizeof(pCharIndex)/sizeof(Descriptor *);
	i; ++ppD, --i)
	*ppD = NULL;

    /* make an initial pass over the descriptor to initialize things */
    for(pD = pDesc, i = nD; i; ++pD, --i)
    {
	/* make sure the long names are sorted alphabetically */
	if ((unsigned)i < nD) /* skip checking the first item */
	{
	    if (!pD->pLongName)
	    {
		/* if there's no longname, the previous one can't have one */
		assert(!pD[-1].pLongName);
	    }
	    else
	    {
		/* if there is a long name, then the prior must be empty, or
		   less then it */
		assert(!pD[-1].pLongName ||
		       (strcmp(pD[-1].pLongName, pD->pLongName) < 0));
	    }
	}

	/* build an index for the single character options */
	if (pD->shortName[0] != '\0')
	{
	    const char c = pD->shortName[0];
	    assert(!pCharIndex[(int)c]); /* this option used twice */
	    assert(isprint(c) && !isspace(c));/* should be a usable character */
	    pCharIndex[(unsigned)c] = pD;
	}

	/* have we got a place to gather naked arguments? */
	if (!pD->pLongName && (pD->shortName[0] == '\0') &&
	    (pD->argType == typeString) && (pD->options & optionMulti))
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
		*(const char **)(pSink + pD->offset) = NULL;
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
	    char buf[128];
	    sprintf(buf, "option %d is missing a short name", i);
	    pError->report(NULL, buf);
	    continue;
	}

	/* is it a long-name argument? */
	if (pArg[1] == '-')
	{
	    /* is the argument name missing? */
	    if (pArg[2] == '\0')
	    {
		char buf[128];
		sprintf(buf, "option %d is missing a long name", i);
		pError->report(NULL, buf);
		continue;
	    }

	    /* find it in the descriptor array */
	    const char *const pKey = &pArg[2];
	    const Descriptor *pArgDesc =
		bsearch<Descriptor, charstar, offsetof(Descriptor, pLongName)>(
		    &pKey, pDesc, nD);
	    if (!pArgDesc)
	    {
		char buf[128];
		snprintf(buf, sizeof(buf),
			 "unknown option %s", pArg);
		pError->report(NULL, buf);
		continue;
	    }
				     
	    bool twosie = processArgument(pSink, pError,
					  pArgDesc, argc, argv, i);
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
		char buf[32];
		sprintf(buf, "unknown option %c", *pC);
		pError->report(NULL, buf);
		continue;
	    }

	    if (pArgDesc->argType == typeBool)
	    {
		*(bool *)(pSink + pArgDesc->offset) = true;
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
		reportError(pError, pArgDesc, "requires a follow-on argument");
		continue;
	    }

	    bool twosie = processArgument(pSink, pError,
					  pArgDesc, argc, argv, i);
	    if (twosie)
		++i;
	}
    }

    /* make a final pass through the descriptors to finish up */
    for(pD = pDesc, i = nD; i; ++pD, --i)
    {
	/* do we need to reverse the list? */
	if (pD->options & optionMulti)
	    reverseStringList((StringArg **)(pSink + pD->offset));
    }

    /* if we got here, everything went ok */
    return true;
}

void ArgvParser::clean(void *pS, const Descriptor *pDesc, size_t nDesc)
{
    for(size_t i = nDesc; i; --i)
    {
	if ((pDesc->argType == typeString) &&
	    (pDesc->options & optionMulti))
	{
	    StringArg **ppSA = (StringArg **)(((char *)pS) + pDesc->offset);
	    StringArg *pSA;
	    while((pSA = *ppSA))
	    {
		/*
		  Remove the first item from the list and delete it.

		  Note we don't delete the string it points to, because these
		  came from argv, and we don't own them.
		*/
		*ppSA = pSA->pNext;
		delete pSA;
	    }
	}
    }
}
