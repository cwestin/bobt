/* Copyright 2011 Chris Westin.  All rights reserved. */
/*
  Example for discussion from "Intrusive Data Structures," from
  "The Book of Brilliant Things:"
  https://www.bookofbrilliantthings.com/eic/offsetof/intrusive
 */

#include <stdio.h>
#include <stdlib.h>
#include "idll.h"


typedef struct myitem
{
    unsigned x;
    idll link;
} myitem;


int main()
{
    idll list;

    /* initialize the list */
    idllInit(&list);

    /* add a bunch of members to the tail of the list */
    unsigned i;
    for(i = 0; i < 20; ++i)
    {
	/* allocate the item */
	myitem *pitem = (myitem *)malloc(sizeof(myitem));

	/* initialize the membership */
	idllInit(&pitem->link);

	/* set the value */
	pitem->x = i;

	/* append it to the list */
	idllAppend(&list, &pitem->link);
    }

    /* print out the list members */
    idll *pm;
    for(pm = idllFirst(&list); pm; pm = idllNext(&list, pm))
    {
	myitem *pitem = idllMember(pm, myitem, link);
	printf("%u ", pitem->x);
    }
    printf("\n");

    /* free all list elements to clean up */
    while((pm = idllFirst(&list)))
    {
	myitem *pitem = idllMember(pm, myitem, link);
	idllRemove(pm);
	free(pitem);
    }

    return 0;
}
