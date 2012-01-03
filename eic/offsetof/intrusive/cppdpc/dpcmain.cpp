/* Copyright 2011 Chris Westin.  All rights reserved. */
/*
  Example for discussion from "Intrusive Data Structures," from
  "The Book of Brilliant Things:"
  https://www.bookofbrilliantthings.com/eic/offsetof/intrusive-data-structures
 */

#ifndef PHOENIX4CPP_HASH_H
#include "hash.h"
#endif

#include <cassert>

#ifndef PHOENIX4CPP_COMPARE_H
#include <compare.h>
#endif

#include "HashMap.h"

using namespace phoenix4cpp;
using namespace bookofbrilliantthings;


/* ------------------------- test code for HashMap -------------------------- */

class TestStruct
{
public:
    unsigned voo;
    unsigned doo;

    unsigned long key;
    HashMapMember hmMember;

    static void destroy(TestStruct *p);
};

void TestStruct::destroy(TestStruct *p)
{
    delete p;
}


class TestStructFactory :
    public HashMapGeneric::Factory
{
public:
    // virtuals from Factory
    virtual HashMapMember *create();

    TestStructFactory(unsigned long key);

private:
    unsigned long key;
};

TestStructFactory::TestStructFactory(unsigned long k):
    key(k)
{
}

HashMapMember *TestStructFactory::create()
{
    TestStruct *pts = new TestStruct;
    pts->key = key;

    pts->voo = key;
    pts->doo = key;

    return &pts->hmMember;
}


static void testHashMap()
{
    HashMap<TestStruct, offsetof(TestStruct, hmMember),
	unsigned long, offsetof(TestStruct, key)> hashMap(
	    0, 0, hashUnsignedLong, compareUnsignedLong, TestStruct::destroy);
    assert(hashMap.getCount() == 0);

    const unsigned n = 100;

    /* insert a bunch of items into the map */
    for(unsigned long i = 0; i < n; ++i)
    {
	TestStructFactory tsf(i);
	TestStruct *pts = hashMap.find(&i, &tsf);
	assert(pts);
	assert(pts->key == i);
	assert(pts->voo == i);
	assert(pts->doo == i);
	assert(hashMap.getCount() == i + 1);

	/* look for it again */
	pts = hashMap.find(&i, NULL);
	assert(pts);
	assert(pts->key == i);
	assert(pts->voo == i);
	assert(pts->doo == i);

	/* look for the last one */
	if (i > 0)
	{
	    unsigned long prev = i - 1;
	    pts = hashMap.find(&prev, NULL);
	    assert(pts);
	    assert(pts->key == i - 1);
	    assert(pts->voo == i - 1);
	    assert(pts->doo == i - 1);
	}
    }

    assert(hashMap.getCount() == n);

    /* check that the items are in the map, and remove them */
    for(unsigned long i = 0; i < n; ++i)
    {
	TestStruct *pts = hashMap.find(&i, NULL);
	assert(pts);
	assert(pts->key == i);
	assert(pts->voo == i);
	assert(pts->doo == i);

	bool r = hashMap.remove(pts);
	assert(r);

	/* make sure its gone */
	assert(!hashMap.remove(pts));
	assert(hashMap.getCount() == n - (i + 1));
    }

    assert(hashMap.getCount() == 0);
}


/* --------------------------------- main ----------------------------------- */

int main()
{
    /* test the HashMap */
    testHashMap();

    return 0;
}


