/* Copyright 2011 Chris Westin.  All rights reserved. */
/*
  Example for discussion from "Intrusive Data Structures," from
  "The Book of Brilliant Things:"
  https://www.bookofbrilliantthings.com/eic/offsetof/intrusive-data-structures
 */

#ifndef BOBT_HASHMAP_H
#include "HashMap.h"
#endif

#include <cassert>

#ifndef PHOENIX4CPP_HASHVALUE_H
#include <HashValue.h>
#endif


namespace bookofbrilliantthings
{
    using namespace phoenix4cpp;

    HashMapMember::HashMapMember():
	pNext(NULL),
	hashValue(0)
    {
    }

    HashMapMember::~HashMapMember()
    {
	/*
	  Don't allow this if it is still on the bucket chain

	  As a design choice, we could instead keep a pointer to the current
	  owner HashMap in HashMapMember, and then we could remove this item
	  here by using that pointer.  This implementation opted for keeping
	  the intrusive member smaller and requiring that items be removed
	  before being destroyed.
	*/
	assert(!pNext);
    }


    HashMapGeneric::HashMapGeneric(
	size_t initBuckets, size_t ab, size_t mo, size_t ko,
	void (*hash)(HashValue *pHashValue, const void *pKey),
	int (*cmp)(const void *pL, const void *pR),
	void (*destroy)(void *)):
	nItems(0),
	avgBucket(ab),
	memberOffset(mo),
	keyOffset(ko),
	hashf(hash),
	cmpf(cmp),
	destroyf(destroy)
    {

	/*
	  Round up the initial number of buckets to the next highest power
	  of 2.  This uses one of the Stanford Graphics bithacks:
	  http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
	 */
	--initBuckets;
	initBuckets |= initBuckets >> 1;
	initBuckets |= initBuckets >> 2;
	initBuckets |= initBuckets >> 4;
	initBuckets |= initBuckets >> 8;
	initBuckets |= initBuckets >> 16;
	++initBuckets;

	/*
	  But don't use anything less than eight, as we'll have to resize
	  that pretty quickly.
	*/
	if (initBuckets < 8)
	    initBuckets = 8;
	nBuckets = initBuckets;

	/* don't get into trouble with no average number of items per bucket */
	if (avgBucket < 1)
	    avgBucket = 1;

	/* initialize the bucket array */
	pBucket = new Bucket[nBuckets];
	for(size_t i = 0; i < nBuckets; ++i)
	    pBucket[i].pList = NULL;
    }

    size_t HashMapGeneric::getCount() const
    {
	return nItems;
    }

    bool HashMapGeneric::remove(void *pI)
    {
	HashMapMember *pMember = (HashMapMember *)(((char *)pI) + memberOffset);
	const size_t iBucket = pMember->hashValue & (nBuckets - 1);
	Bucket *const pB = pBucket + iBucket;
	for(HashMapMember **ppM = &pB->pList; *ppM; ppM = &(*ppM)->pNext)
	{
	    if (*ppM == pMember)
	    {
		*ppM = pMember->pNext;
		pMember->pNext = NULL;
		--nItems;

		return true;
	    }

	    /*
	      We can cut out early if we've passed the point we should have
	      found it at.
	    */
	    if ((*ppM)->hashValue > pMember->hashValue)
		break;
	}

	/* if we got here, we didn't find it */
	return false;
    }

    void HashMapGeneric::upsize()
    {
	/* check that the new size isn't an overflow */
	size_t newN = nBuckets << 1;
	if (newN == 0)
	    return; /* it's as big as it gets */

	/* allocate a new bucket list */
	Bucket *pNewBucket = new Bucket[newN];

	/* copy the lists */
	size_t iBucket = nBuckets;
	for(Bucket *pNB = pNewBucket, *pNB2 = pNB + nBuckets,
		*pOB = pBucket; iBucket; ++pNB, ++pOB, ++pNB2, --iBucket)
	{
	    /* copy the list from the old bucket array to the new */
	    pNB->pList = pOB->pList;

	    /* initialize the second half of the new bucket array */
	    pNB2->pList = NULL;

	    /*
	      Find the elements on this list that go on the new
	      second half of the bucket array.
	    */
	    for(HashMapMember **ppM = &pNB->pList; *ppM;)
	    {
		/*
		  This item should only move if it uses the uppermost bit of the
		  new mask.
		*/
		if (!((*ppM)->hashValue & nBuckets))
		{
		    /* advance along the list */
		    ppM = &(*ppM)->pNext;
		    continue;
		}

		/* remove the item from it's current location */
		HashMapMember *pM = *ppM;
		*ppM = (*ppM)->pNext;

		findInBucket(pNB2, pM->hashValue,
			     ((const char *)pM) - memberOffset + keyOffset, pM,
			     NULL);
	    }
	}

	/* replace the old values */
	delete[] pBucket;
	pBucket = pNewBucket;
	nBuckets = newN;
    }

    HashMapMember *HashMapGeneric::findInBucket(
	Bucket *pB, unsigned long hashValue, const void *pKey,
	HashMapMember *pNew, Factory *pFactory)
    {
	HashMapMember **ppM;

	for(ppM = &pB->pList; *ppM; ppM = &(*ppM)->pNext)
	{
	    if ((*ppM)->hashValue == hashValue)
	    {
		const void *pLeft =
		    (const void *)(((char *)(*ppM)) - memberOffset + keyOffset);
		int cmp = (*cmpf)(pLeft, pKey);
		if (cmp == 0)
		    return *ppM;

		if (cmp > 0)
		    break;
	    }

	    if ((*ppM)->hashValue > hashValue)
		break;
	}

	if (!pNew)
	{
	    /* if we got here, there was no match for the key */
	    if (!pFactory)
		return NULL;

	    pNew = pFactory->create();
	    pNew->hashValue = hashValue;
	    ++nItems;
	}

	/* link the new item into the bucket list at the place found above */
	pNew->pNext = *ppM;
	*ppM = pNew;

	return pNew;
    }
						
    void *HashMapGeneric::find(const void *pKey, Factory *pFactory)
    {
	/*
	  If we might be adding something, and we're over the specified
	  average bucket size, resize the bucket array.
	*/
	if (pFactory && ((nItems + nBuckets - 1) / nBuckets > avgBucket))
	    upsize();

	/* find where the requested item belongs */
	HashValue hash;
	(*hashf)(&hash, pKey);
	unsigned long hashValue = hash.get();
	size_t iBucket = hashValue & (nBuckets - 1);
	HashMapMember *pM = findInBucket(
	    pBucket + iBucket, hashValue, pKey, NULL, pFactory);

	if (pM)
	    return (void *)(((char *)pM) - memberOffset);
	return NULL;
    }

    void HashMapGeneric::clear()
    {
	size_t i;
	Bucket *pB;

	for(pB = pBucket, i = nBuckets; i; ++pB, --i)
	{
	    HashMapMember *pM;
	    while((pM = pB->pList))
	    {
		/* remove this one from the bucket */
		pB->pList = pM->pNext;
		pM->pNext = NULL;
		--nItems;

		(*destroyf)((void *)(((char *)pM) - memberOffset));
	    }
	}
    }

    HashMapGeneric::~HashMapGeneric()
    {
	clear();

	/* this had better already be empty */
	assert(!nItems);

	delete[] pBucket;
    }

}
