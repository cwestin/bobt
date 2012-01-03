/* Copyright 2011 Chris Westin.  All rights reserved. */
/*
  Example for discussion from "Intrusive Data Structures," from
  "The Book of Brilliant Things:"
  https://www.bookofbrilliantthings.com/eic/offsetof/intrusive-data-structures
 */

#pragma once

#ifndef BOBT_HASHMAP_H
#define BOBT_HASHMAP_H

#include <cstddef>

#ifndef PHOENIX4CPP_HASHVALUE_H
#include <HashValue.h>
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
		       void (*hash)(HashValue *pHashValue, const void *pKey),
		       int (*cmp)(const void *pL, const void *pR),
		       void (*destroy)(void *pI));

	size_t getCount() const;

	bool remove(void *pI);

	class Factory
	{
	public:
	    virtual ~Factory() {};
	    virtual HashMapMember *create() = 0;
	};

	void *find(const void *pKey, Factory *pFactory);

	void clear();

    private:
	void upsize();

	struct Bucket
	{
	    HashMapMember *pList;
	};

	HashMapMember *findInBucket(
	    Bucket *pB, unsigned long hashValue, const void *pKey,
	    HashMapMember *pNew, Factory *pFactory);

	Bucket *pBucket;
	size_t nBuckets;
	size_t nItems;
	size_t avgBucket;
	size_t memberOffset;
	size_t keyOffset;
	void (*hashf)(HashValue *, const void *);
        int (*cmpf)(const void *, const void *);
	void (*destroyf)(void *);
    };

    template <class I, size_t memberOffset, class K, size_t keyOffset>
    class HashMap :
	protected HashMapGeneric
    {
    public:
	~HashMap();

	HashMap(size_t initBuckets, size_t avgBucket,
		void (*hash)(HashValue *pHashValue, const K *pKey),
		int (*cmp)(const K *pLeft, const K *pRight),
	        void (*destroy)(I *pI));

	size_t getCount() const;

	bool remove(I *pI);

	I *find(const K *pKey, Factory *pFactory);
    };

}


/* ========================= PRIVATE IMPLEMENTATION ========================= */

namespace bookofbrilliantthings
{
    template <class I, size_t tMemberOffset, class K, size_t tKeyOffset>
    inline HashMap<I, tMemberOffset, K, tKeyOffset>::HashMap(
	size_t initBuckets, size_t avgBucket,
	void (*hash)(HashValue *pHashValue, const K *pKey),
	int (*cmp)(const K *pLeft, const K *pRight),
	void (*destroy)(I *pI)):
	HashMapGeneric(
	    initBuckets, avgBucket, tMemberOffset, tKeyOffset,
	    (void (*)(HashValue *, const void *))hash,
	    (int (*)(const void *, const void *))cmp,
	    (void (*)(void *))destroy)
    {
    }

    template <class I, size_t memberOffset, class K, size_t keyOffset>
    inline HashMap<I, memberOffset, K, keyOffset>::~HashMap()
    {
    }

    template <class I, size_t memberOffset, class K, size_t keyOffset>
    inline size_t HashMap<I, memberOffset, K, keyOffset>::getCount() const
    {
	return HashMapGeneric::getCount();
    }

    template <class I, size_t tMemberOffset, class K, size_t keyOffset>
    inline bool HashMap<I, tMemberOffset, K, keyOffset>::remove(I *pI)
    {
	return HashMapGeneric::remove(pI);
    }

    template <class I, size_t tMemberOffset, class K, size_t keyOffset>
    inline I *HashMap<I, tMemberOffset, K, keyOffset>::find(
	const K *pKey, Factory *pFactory)
    {
	return (I *)HashMapGeneric::find(pKey, pFactory);
    }

}

#endif /* BOBT_HASHMAP_H */
