/* Copyright 2011 Chris Westin.  All rights reserved. */
/*
  Example for discussion from "Intrusive Data Structures," from
  "The Book of Brilliant Things:"
  https://www.bookofbrilliantthings.com/eic/offsetof/intrusive-data-structures
 */

#include <stdio.h>
#include <stdlib.h>
#include <pxlibDll.h>


typedef struct myitem
{
    unsigned x;
    pxlibDllMember link;
} myitem;


int main()
{
    pxlibDllList list;

    /* initialize the list */
    pxlibDllListInit(&list);

    /* add a bunch of members to the tail of the list */
    unsigned i;
    for(i = 0; i < 20; ++i)
    {
	/* allocate the item */
	myitem *pitem = (myitem *)malloc(sizeof(myitem));

	/* initialize the membership */
	pxlibDllMemberInit(&pitem->link);

	/* set the value */
	pitem->x = i;

	/* append it to the list */
	pxlibDllAppend(&list, &pitem->link);
    }

    /* print out the list members */
    pxlibDllMember *pMember;
    for(pMember = pxlibDllFirst(&list); pMember;
	pMember = pxlibDllNext(&list, pMember))
    {
	myitem *pitem = pxlibDllGetMember(pMember, myitem, link);
	printf("%u ", pitem->x);
    }
    printf("\n");

    /* free all list elements to clean up */
    while((pMember = pxlibDllFirst(&list)))
    {
	myitem *pitem = pxlibDllGetMember(pMember, myitem, link);
	pxlibDllRemove(pMember);
	free(pitem);
    }

    return 0;
}
