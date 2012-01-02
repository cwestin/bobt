/* Copyright 2011 Chris Westin.  All rights reserved. */
/*
  Example for discussion from "Intrusive Data Structures," from
  "The Book of Brilliant Things:"
  https://www.bookofbrilliantthings.com/eic/offsetof/intrusive-data-structures
 */

#include <cstddef>
#include <stdio.h>
#include <DoublyLinked.h>


using namespace phoenix4cpp;

class MyItem
{
public:
    unsigned x;

    DoublyLinkedMembership link;
};


int main()
{
    DoublyLinkedList<MyItem, offsetof(MyItem, link)> list;
    /*
      Under g++, this must be compiled with -Wno-invalid-offsetof, otherwise it
      will generate the following error:
      idllmain.cpp:26: warning: invalid access to non-static data member `MyItem::link' of NULL object
      idllmain.cpp:26: warning: (perhaps the `offsetof' macro was used incorrectly)
    */

    /* add a bunch of members to the tail of the list */
    unsigned i;
    for(i = 0; i < 20; ++i)
    {
	/* allocate the item */
	MyItem *pItem = new MyItem;

	/* set the value */
	pItem->x = i;

	/* append it to the list */
	list.append(pItem);
    }

    /* print out the list members */
    MyItem *pItem;
    for(pItem = list.getFirst(); pItem; pItem = list.getNext(pItem))
    {
	printf("%u ", pItem->x);
    }
    printf("\n");

    /* free all list elements to clean up */
#ifdef NEVER // the list head destructor will take care of this here
    while((pItem = list.getFirst()))
    {
	// pItem->link.remove();
	list.remove(pItem);
	delete pItem;
    }
#endif

    return 0;
}
