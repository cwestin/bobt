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
#include <cstdio>
#include <cstring>

#ifndef PHOENIX4CPP_COMPARE_H
#include <compare.h>
#endif

#include "DiskPageCache.h"

#include "DoublyLinked.h"
#include "HashMap.h"

using namespace phoenix4cpp;
using namespace bookofbrilliantthings;

/* ---------------------- test code for DoublyLinked ------------------------ */
class TestItem
{
public:
    unsigned x;

    DoublyLinkedMembership dlMembership;
};


static void testDoublyLinked()
{
    DoublyLinkedList<TestItem, offsetof(TestItem, dlMembership)> list;

    unsigned i;
    for(i = 10; i > 0; --i)
    {
        TestItem *pItem = new TestItem;
        pItem->x = i;
        list.prepend(pItem);
    }

    TestItem *pItem;
    for(pItem = list.getLast(), i = 10; pItem;
        --i, pItem = list.getPrevious(pItem))
    {
        assert(pItem->x == i);
    }
    assert(i == 0); // we saw them all
}

/* ------------------------- test code for HashMap -------------------------- */

class TestStruct
{
public:
    unsigned voo;
    unsigned doo;

    unsigned long key;
    HashMapMembership hmMembership;

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
    virtual HashMapMembership *create();

    TestStructFactory(unsigned long key);

private:
    unsigned long key;
};

TestStructFactory::TestStructFactory(unsigned long k):
    key(k)
{
}

HashMapMembership *TestStructFactory::create()
{
    TestStruct *pts = new TestStruct;
    pts->key = key;

    pts->voo = key;
    pts->doo = key;

    return &pts->hmMembership;
}


static void testHashMap()
{
    HashMap<TestStruct, offsetof(TestStruct, hmMembership),
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


/* --------------------- test code for disk page cache ---------------------- */

static void testDiskPageCache()
{
    DiskPageCache *pdpc = DiskPageCache::create("foo.dat", "r+", 7);
    DiskPageCache::PageLock *pLock;
    void *p;

    unsigned long i;
    const size_t pageSize = pdpc->getPageSize();

    /* initialize the contents of the disk pages */
    for(i = 0; i < 10; ++i)
    {
        pLock = pdpc->find(i);
        p = pLock->getData();
        memset(p, i, pageSize);
        pLock->markDirty();
        delete pLock;
    }

    /* modify the contents of the disk pages */
    for(i = 0; i < 10; ++i)
    {
        pLock = pdpc->find(i);
        p = pLock->getData();
        memset(p, 9 - ((char *)p)[6], pageSize);
        pLock->markDirty();
        delete pLock;
    }

    /* modify the contents of the disk pages back to the original values */
    for(i = 0; i < 10; ++i)
    {
        pLock = pdpc->find(i);
        p = pLock->getData();
        memset(p, 9 - ((char *)p)[10], pageSize);
        pLock->markDirty();
        delete pLock;
    }

    /* cycle buffers in and out of memory, checking their contents */
    for(i = 13; i < 297; i += 3)
    {
        pLock = pdpc->find(i % 10);
        p = pLock->getData();

        if ((unsigned)(((char *)p)[10]) != i % 10)
            printf("incorrect value %d in page %lu\n", ((char *)p)[10], i % 10);

        delete pLock;
    }

    delete pdpc;
}


/* --------------------------------- main ----------------------------------- */

int main()
{
    testDoublyLinked();
    testHashMap();
    testDiskPageCache();

    return 0;
}
