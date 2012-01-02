/* Copyright 2011 Chris Westin.  All rights reserved. */
/*
  Example for discussion from "Intrusive Data Structures," from
  "The Book of Brilliant Things:"
  https://www.bookofbrilliantthings.com/eic/offsetof/intrusive-data-structures
 */

#ifndef BOBT_CASSERT_H
#include <cassert>
#define BOBT_CASSERT_H
#endif

#ifndef BOBT_HASHMAP_H
#include "HashMap.h"
#endif

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
};

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
    ComparatorUnsignedLong cmpul;
    HashMap<TestStruct, offsetof(TestStruct, hmMember),
	unsigned long, offsetof(TestStruct, key)> hashMap(0, 0, &cmpul);
    assert(hashMap.getCount() == 0);

    const unsigned n = 100;

    /* insert a bunch of items into the map */
    for(unsigned i = 0; i < n; ++i)
    {
	HashableUnsignedLong hul(i);
	TestStructFactory tsf(i);
	TestStruct *pts = hashMap.find(&hul, &tsf);

	assert(pts);
	assert(pts->key == i);
	assert(pts->voo == i);
	assert(pts->doo == i);
    }

    assert(hashMap.getCount() == n);

    /* check that the items are in the map, and remove them */
    for(unsigned i = 0; i < n; ++i)
    {
	HashableUnsignedLong hul(i);
	TestStruct *pts = hashMap.find(&hul, NULL);

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
}


/* --------------------------------- main ----------------------------------- */

int main()
{
    /* test the HashMap */
    testHashMap();

    return 0;
}


