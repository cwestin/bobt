/* Copyright 2011 Chris Westin.  All rights reserved. */
/*
  Example for discussion from "Intrusive Data Structures," from
  "The Book of Brilliant Things:"
  https://www.bookofbrilliantthings.com/eic/offsetof/intrusive-data-structures
 */

#ifndef BOBT_HASHMAP_H
#include "HashMap.h"
#endif

#ifndef BOBT_CASSERT_H
#include <cassert>
#define BOBT_CASSERT_H
#endif

#ifndef PHOENIX4CPP_HASHVALUE_H
#include <HashValue.h>
#endif


using namespace phoenix4cpp;

namespace bookofbrilliantthings
{

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


    HashMapGeneric::HashMapGeneric(size_t initBuckets, size_t ab, size_t mo,
				   size_t ko, const Comparator *pComparator):
	nItems(0),
	avgBucket(ab),
	memberOffset(mo),
	keyOffset(ko),
	pCmp(pComparator)
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

    bool HashMapGeneric::remove(HashMapMember *pMember)
    {
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
		*pOB = pBucket; iBucket; ++pNB, ++pOB, --iBucket)
	{
	    /* copy the list from the old bucket array to the new */
	    pNB->pList = pOB->pList;

	    /* initialize the second half of the new bucket array */
	    pNB2->pList = NULL;

	    /*
	      Find the trailing parts of the list that go on the new
	      second half of the bucket array.
	    */
	    for(HashMapMember **ppM = &pNB->pList; *ppM;
		ppM = &(*ppM)->pNext)
	    {
		/* move the rest of the list */
		if ((*ppM)->hashValue >= newN)
		{
		    pNB2->pList = *ppM;
		    *ppM = NULL;
		    break;
		}
	    }
	}

	/* replace the old values */
	delete[] pBucket;
	pBucket = pNewBucket;
	nBuckets = newN;
    }

    HashMapMember *HashMapGeneric::find(const Hashable *pKey, Factory *pFactory)
    {
	/*
	  If we might be adding something, and we're over the specified
	  average bucket size, resize the bucket array.
	*/
	if (pFactory && ((nItems + nBuckets - 1) / nBuckets > avgBucket))
	    upsize();

	/* find where the requested item belongs */
	HashValue hash;
	pKey->hash(&hash);
	unsigned long hashValue = hash.get();
	size_t iBucket = hashValue & (nBuckets - 1);
	Bucket *pB = pBucket + iBucket;
	HashMapMember **ppM;
	for(ppM = &pB->pList; *ppM; ppM = &(*ppM)->pNext)
	{
	    if ((*ppM)->hashValue == hashValue)
	    {
		const void *pLeft =
		    (const void *)(((char *)(*ppM)) - memberOffset + keyOffset);
		int cmp = pCmp->compare(pLeft, pKey->getRawPointer());
		if (cmp == 0)
		    return *ppM;

		if (cmp > 0)
		    break;
	    }

	    if ((*ppM)->hashValue > hashValue)
		break;
	}

	/* if we got here, there was no match for the key */
	if (!pFactory)
	    return NULL;

	HashMapMember *pNew = pFactory->create();
	pNew->hashValue = hashValue;
	pNew->pNext = *ppM;
	*ppM = pNew;
	++nItems;

	return pNew;
    }

    void HashMapGeneric::clear(void (*destroy)(void *))
    {
	size_t i;
	Bucket *pB;

	for(pB = pBucket, i = nBuckets; i; ++pBucket, --i)
	{
	    HashMapMember *pM;
	    while((pM = pB->pList))
	    {
		/* remove this one from the bucket */
		pB->pList = pM->pNext;
		pM->pNext = NULL;
		--nItems;

		(*destroy)((void *)(((char *)pM) - memberOffset));
	    }
	}
    }

    HashMapGeneric::~HashMapGeneric()
    {
	/* this had better already be empty */
	assert(!nItems);

	delete[] pBucket;
    }

}
