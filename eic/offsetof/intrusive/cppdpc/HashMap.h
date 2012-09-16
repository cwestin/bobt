/* Copyright 2012 Chris Westin.  All rights reserved. */
/*
  Example for discussion from "Intrusive Data Structures," from
  "The Book of Brilliant Things:"
  https://www.bookofbrilliantthings.com/book/eic/offsetof/intrusive-data-structures
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

    class HashMapMembership
    {
    public:
        HashMapMembership();
        ~HashMapMembership();

        /* $$$ get rid of this? */
        class Factory
        {
        public:
            virtual ~Factory() {};
            virtual HashMapMembership *create() = 0;
        };

    private:
        friend class HashMapGeneric; // all mutations come from here

        HashMapMembership *pNext;
        unsigned long hashValue;
    };

    class HashMapGeneric
    {
    public:
        ~HashMapGeneric();

        HashMapGeneric(size_t initBuckets, size_t avgBucket,
                       size_t membershipOffset, size_t keyOffset,
                       void (*hash)(HashValue *pHashValue, const void *pKey),
                       int (*cmp)(const void *pL, const void *pR),
                       void (*destroy)(void *pI));

        size_t getCount() const;

        bool remove(void *pI);

        class Factory
        {
        public:
            virtual ~Factory() {};
            virtual HashMapMembership *create() = 0;
        };

        void *find(const void *pKey, Factory *pFactory);

        class Unknown; // for a generic class instance pointer
        class VisitorUnknown
        {
        public:
            virtual ~VisitorUnknown() {};
            virtual bool visit(Unknown *pU) = 0;
        };

        void visit(VisitorUnknown *pVU);

        void clear();

    private:
        void upsize();

        struct Bucket
        {
            HashMapMembership *pList;
        };

        HashMapMembership *findInBucket(
            Bucket *pB, unsigned long hashValue, const void *pKey,
            HashMapMembership *pNew, Factory *pFactory);

        Bucket *pBucket;
        size_t nBuckets;
        size_t nItems;
        size_t avgBucket;
        size_t membershipOffset;
        size_t keyOffset;
        void (*hashf)(HashValue *, const void *);
        int (*cmpf)(const void *, const void *);
        void (*destroyf)(void *);
    };

    template <class I, size_t membershipOffset, class K, size_t keyOffset>
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

        class Visitor
        {
        public:
            virtual ~Visitor() {};
            virtual bool visit(I *pI) = 0;
        };

        void visit(Visitor *pV);

        void clear();
    };

}


/* ========================= PRIVATE IMPLEMENTATION ========================= */

namespace bookofbrilliantthings
{
    template <class I, size_t tMembershipOffset, class K, size_t tKeyOffset>
    inline HashMap<I, tMembershipOffset, K, tKeyOffset>::HashMap(
        size_t initBuckets, size_t avgBucket,
        void (*hash)(HashValue *pHashValue, const K *pKey),
        int (*cmp)(const K *pLeft, const K *pRight),
        void (*destroy)(I *pI)):
        HashMapGeneric(
            initBuckets, avgBucket, tMembershipOffset, tKeyOffset,
            (void (*)(HashValue *, const void *))hash,
            (int (*)(const void *, const void *))cmp,
            (void (*)(void *))destroy)
    {
    }

    template <class I, size_t membershipOffset, class K, size_t keyOffset>
    inline HashMap<I, membershipOffset, K, keyOffset>::~HashMap()
    {
    }

    template <class I, size_t membershipOffset, class K, size_t keyOffset>
    inline size_t HashMap<I, membershipOffset, K, keyOffset>::getCount() const
    {
        return HashMapGeneric::getCount();
    }

    template <class I, size_t tMembershipOffset, class K, size_t keyOffset>
    inline bool HashMap<I, tMembershipOffset, K, keyOffset>::remove(I *pI)
    {
        return HashMapGeneric::remove(pI);
    }

    template <class I, size_t tMembershipOffset, class K, size_t keyOffset>
    inline I *HashMap<I, tMembershipOffset, K, keyOffset>::find(
        const K *pKey, Factory *pFactory)
    {
        return (I *)HashMapGeneric::find(pKey, pFactory);
    }

    template <class I, size_t tMembershipOffset, class K, size_t keyOffset>
    inline void HashMap<I, tMembershipOffset, K, keyOffset>::visit(
        Visitor *pV)
    {
        HashMapGeneric::visit((VisitorUnknown *)pV);
    }

    template <class I, size_t tMembershipOffset, class K, size_t keyOffset>
    inline void HashMap<I, tMembershipOffset, K, keyOffset>::clear()
    {
        HashMapGeneric::clear();
    }

}

#endif /* BOBT_HASHMAP_H */
