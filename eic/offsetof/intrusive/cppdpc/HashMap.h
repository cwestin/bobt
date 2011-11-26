/* Copyright 2011 Chris Westin.  All rights reserved. */
/*
  Example for discussion from "Intrusive Data Structures," from
  "The Book of Brilliant Things:"
  https://www.bookofbrilliantthings.com/eic/offsetof/intrusive-data-structures
 */

#pragma once

#ifndef BOBT_HASHMAP_H
#define BOBT_HASHMAP_H

#ifndef BOBT_CSTDDEF_H
#include <cstddef>
#define BOBT_CSTDDEF_H
#endif

#ifndef PHOENIX4CPP_COMPARATOR_H
#include <Comparator.h>
#endif

#ifndef PHOENIX4CPP_HASHABLE_H
#include <Hashable.h>
#endif


namespace bookofbrilliantthings
{
    using namespace phoenix4cpp;

    class HashMapMember
    {
    public:
	HashMapMember();
	~HashMapMember();

	class Factory
	{
	public:
	    virtual ~Factory() {};
	    virtual HashMapMember *create() = 0;
	};

    private:
	friend class HashMapGeneric; // all mutations come from here

	HashMapMember *pNext;
	unsigned long hashValue;
    };

    class HashMapGeneric
    {
    public:
	~HashMapGeneric();

	HashMapGeneric(size_t initBuckets, size_t avgBucket,
		       size_t memberOffset, size_t keyOffset,
		       const Comparator *pComparator);
	size_t getCount() const;
	bool remove(HashMapMember *pMember);

	class Factory
	{
	public:
	    virtual ~Factory() {};
	    virtual HashMapMember *create() = 0;
	};

	HashMapMember *find(const Hashable *pKey, Factory *pFactory);

	void clear(void (*destroy)(void *));

    private:
	void upsize();

	struct Bucket
	{
	    HashMapMember *pList;
	};

	Bucket *pBucket;
	size_t nBuckets;
	size_t nItems;
	size_t avgBucket;
	size_t memberOffset;
	size_t keyOffset;
	const Comparator *pCmp;
    };

    template <class I, size_t memberOffset, class K, size_t keyOffset>
    class HashMap :
	protected HashMapGeneric
    {
    public:
	~HashMap();

	HashMap(size_t initBuckets, size_t avgBucket,
		const Comparator *pComparator);
	size_t getCount() const;
	bool remove(I *pI);

	I *find(const Hashable *pKey, Factory *pFactory);

    private:
	/* This will be instantiated for each type */
	static void itemDestroy(void *);
    };
}

/* ------------------------- PRIVATE IMPLEMENTATION ------------------------- */

namespace bookofbrilliantthings
{
    template <class I, size_t tMemberOffset, class K, size_t tKeyOffset>
    inline HashMap<I, tMemberOffset, K, tKeyOffset>::HashMap(
	size_t initBuckets, size_t avgBucket, const Comparator *pComparator):
	HashMapGeneric(initBuckets, avgBucket, tMemberOffset, tKeyOffset,
	    pComparator)
    {
    }

    template <class I, size_t memberOffset, class K, size_t keyOffset>
    inline HashMap<I, memberOffset, K, keyOffset>::~HashMap()
    {
	/*
	  Clear out the generic map first.

	  We have to do this from here in order to provide the callback that
	  will call the type-specific destructor.
	*/
	clear(itemDestroy);

	/* ~HashMapGeneric() will clean everything else up now */
    }

    template <class I, size_t memberOffset, class K, size_t keyOffset>
    inline size_t HashMap<I, memberOffset, K, keyOffset>::getCount() const
    {
	return HashMapGeneric::getCount();
    }

    template <class I, size_t tMemberOffset, class K, size_t keyOffset>
    inline bool HashMap<I, tMemberOffset, K, keyOffset>::remove(I *pI)
    {
	HashMapMember *pM = (HashMapMember *)(((char *)pI) + tMemberOffset);
	return HashMapGeneric::remove(pM);
    }

    template <class I, size_t tMemberOffset, class K, size_t keyOffset>
    inline I *HashMap<I, tMemberOffset, K, keyOffset>::find(
	const Hashable *pKey, Factory *pFactory)
    {
	HashMapMember *pM = HashMapGeneric::find(pKey, pFactory);
	if (pM)
	    return (I *)(((char *)pM) - tMemberOffset);

	/* we didn't find it */
	return NULL;
    }

    template <class I, size_t memberOffset, class K, size_t keyOffset>
    void HashMap<I, memberOffset, K, keyOffset>::itemDestroy(void *pItem)
    {
	I *pI = static_cast<I *>(pItem);
	delete pI;
    }

}

#endif /* BOBT_HASHMAP_H */
