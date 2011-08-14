/* Copyright 2011 Chris Westin.  All rights reserved. */
/*
  Example for discussion from "Intrusive Data Structures," from
  "The Book of Brilliant Things:"
  https://www.bookofbrilliantthings.com/eic/offsetof/intrusive
 */

#pragma once

#ifndef IDLL_H
#define IDLL_H

#include <stddef.h>

/*
  Intrusive Doubly Linked List

  Embed this membership structure inside structures that are to be members of
  a linked list.

  The membership data is considered private, and will be maintained by the
  list implementation.  idll structures must always be initialized with
  idllInit().

  This structure is also used as the head of a list, and when used as such,
  must also be initialized with idllInit().

  IMPLEMENTATION NOTES
  The head of a list also functions as a sentinel:  the last element of the
  list points back to the head, and this is used to detect the end of the
  list.  This simplifies the insertion and removal operations on the list,
  and eliminates tests for handling empty lists.

  The implementation is simple enough and stable enough to be inlined, so it
  is provided as a set of preprocessor macros.
*/
typedef struct idll
{
    struct idll *pnext;
    struct idll *pprev;
} idll;


/*
  Given a pointer to a membership known to be inside a structure,
  recover a pointer to the original containing structure.

  @param pm pointer to membership
  @param s structure type name
  @param n member type name
  @returns pointer to an s
*/
#define idllMember(pm, s, n) ((s *)(((char *)(pm)) - offsetof(s, n)))

/*
  Initialize a list link or list head.

  @param pointer to a membership or list head
*/
#define idllInit(pm) ((pm)->pnext = (pm), (pm)->pprev = (pm))

/*
  Add a new link after a specified position.

  Adding the new link after a list head prepends it to the list.

  @param pnew the new link to be added
  @param ppos the position after which to add the new link
*/
#define idllAddAfter(pnew, ppos) ( \
    (pnew)->pprev = (ppos), \
    (pnew)->pnext = (ppos)->pnext, \
    (ppos)->pnext->pprev = (pnew), \
    (ppos)->pnext = (pnew) \
    )


/*
  Add a new link before a specified position.

  Adding the new link before a list head appends it to the list.

  @param pnew the new link to be added
  @param ppos the position after which to add the new link
*/
#define idllAddBefore(pnew, ppos) ( \
    (pnew)->pnext = (ppos), \
    (pnew)->pprev = (ppos)->pprev, \
    (ppos)->pprev->pnext = (pnew), \
    (ppos)->pprev = (pnew) \
    )

/*
  Remove a link from a list.

  Removing the list head will cause problems; do not remove it.

  @param pm pointer to a list link
*/
#define idllRemove(pm) ( \
    (pm)->pprev->pnext = (pm)->pnext, \
    (pm)->pnext->pprev = (pm)->pprev \
    )

/*
  Return the membership in the list after a specified membership.

  @param plist pointer to the list head
  @param pm pointer to a membership
  @returns pointer to the membership after pm on the list or NULL if pm
    is the last membership
*/
#define idllNext(plist, pm) ((pm)->pnext == (plist) ? NULL : (pm)->pnext)

/*
  Return the membership in the list before a specified membership.

  @param plist pointer to the list head
  @param pm pointer to a membership
  @returns pointer to the membership before pm on the list or NULL if pm
    is the first membership
*/
#define idllPrev(plist, pm) ((pm)->pprev == (plist) ? NULL : (pm)->pprev)

/*
  Return the first membership on the list.

  @param plist pointer to the list head
  @returns the first membership in the list, or NULL if the list is empty
*/
#define idllFirst(plist) idllNext(plist, plist)

/*
  Return the last membership on the list.

  @param plist pointer to the list head
  @returns the last membership in the list, or NULL if the list is empty
*/
#define idllLast(plist) idllPrev(plist, plist)

/*
  Append a new membership to a list.

  @param pnew pointer to the new membership
  @param plist pointer to the head of the list
*/
#define idllAppend(plist, pnew) idllAddBefore(pnew, plist)

/*
  Prepend a new membership to a list.

  @param pnew pointer to the new membership
  @param plist pointer to the head of the list
*/
#define idllPrepend(plist, pnew) idllAddAfter(pnew, plist)

#endif /* IDLL_H */
