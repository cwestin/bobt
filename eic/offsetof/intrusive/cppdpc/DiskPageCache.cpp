/* Copyright 2012 Chris Westin.  All rights reserved. */
/*
  Example for discussion from "Intrusive Data Structures," from
  "The Book of Brilliant Things:"
  https://www.bookofbrilliantthings.com/book/eic/offsetof/intrusive-data-structures

  The page cache uses two intrusive data structures to provide its
  functionality.  There is a hash table keyed on pageId, used for holding
  the pages that are currently cached, and there is a doubly linked
  list used for managing the replacement policy.
        
  For a description of the page replacement policy, see the disk page
  cache header file.

  A find call is a simple lookup in the hash table for the requested
  page.  Before returning, we implement the replacement policy's
  aging behavior on the list.

  The cache also maintains a list of all the outstanding PageLocks
  so that they can be invalidated if the cache is destroyed.

  Within this implementation file, all class members are public; various
  parts of the implementation need to be able to see each other, and in this
  limited context, there's no need to create extra methods and layers of
  security; the units of encapsulation are the cache the its locks, as declared
  in the header file.
*/

#ifndef DISKPAGECACHE_H
#include "DiskPageCache.h"
#endif


#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef PHOENIX4CPP_DOUBLYLINKED_H
#include "DoublyLinked.h"
#endif

#ifndef PHOENIX4CPP_COMPARE_H
#include "compare.h"
#endif

#ifndef PHOENIX4CPP_HASH_H
#include "hash.h"
#endif

#include "HashMap.h"


using namespace phoenix4cpp;
using namespace bookofbrilliantthings;


class DiskPageCache_i :
    public DiskPageCache
{
public:
    // virtuals from DiskPageCache
    virtual ~DiskPageCache_i();
    virtual PageLock *find(unsigned long pageId);
    virtual size_t getPageSize() const;
    virtual void flush();

    DiskPageCache_i(unsigned long maxPages);

    struct Page
    {
        unsigned long pageId; /* the pageId for this page */
        unsigned flag; enum
        {
            FDIRTY = 0x0001, /* page is dirty */
        };

        unsigned nLocks;  /* a reference count; page locks held on this page */

        DiskPageCache_i *pdpc; /* pointer back to the cache implementation */
        HashMapMembership hmm; /* membership in the cache's page hash table */
        DoublyLinkedMembership link; /* membership on the mfu or free list */

        enum { SIZE = 2048 }; /* size of a page */
        char buf[SIZE]; /* the actual page content (not aligned) */
    };

    class PageFactory :
        public HashMapGeneric::Factory
    {
    public:
        // virtuals from HashMap::Factory
        virtual HashMapMembership *create();

        PageFactory(DiskPageCache_i *pdpc, unsigned long pageId);
        unsigned long pageId;
        DiskPageCache_i *pdpc;
    };

    class PageLock_i :
        public DiskPageCache::PageLock
    {
    public:
        // virtuals from PageLock
        virtual ~PageLock_i();
        virtual void *getData() const;
        virtual void markDirty();

        Page *pPage; /* the locked page */
        DoublyLinkedMembership locksLink;/* membership on a list of all locks */
    };


    static void zeroPage(FILE *fp);
    /*
      Writes SIZE zeroes to [fp] at the current position.  Used
      for initializing new pages at the end of a file.
    */
        
    FILE *fp; /* the opened file */
    size_t fileSize; /* how big the file is currently (in bytes) */
    unsigned long maxPages; /* the maximum size of the cache, in pages */

    typedef HashMap<Page, offsetof(Page, hmm), unsigned long,
            offsetof(Page, pageId)> PageMapType;
    PageMapType pageMap;
    DoublyLinkedList<Page, offsetof(Page,  link)> mfu;
               /* age list: infrequently used items drift right (to the end) */
    DoublyLinkedList<PageLock_i, offsetof(PageLock_i, locksLink)> locks;
                                 /* list of all locks held, for invalidation */

    class PageWriter :
        public PageMapType::Visitor
    {
    public:
        // virtuals from PageMapType::Visitor
        virtual bool visit(Page *pPage);
    };
    static PageWriter pageWriter; /* we only need one of these */

    /*
      We will manage free pages ourselves.  At creation time, we allocate
      an array of pages, then we manage a simple linked list of free pages.
     */
    DoublyLinkedList<Page, offsetof(Page, link)> free;
    Page *pPages; /* allocated array of pages */
};


DiskPageCache_i::DiskPageCache_i(unsigned long mp):
    fp(NULL),
    fileSize(0),
    maxPages(mp),
    pageMap(mp, 0, hashUnsignedLong, compareUnsignedLong, NULL),
    mfu(),
    locks(),
    free(),
    pPages(NULL)
{
}

DiskPageCache_i::PageFactory::PageFactory(
    DiskPageCache_i *p, unsigned long pid):
    pageId(pid),
    pdpc(p)
{
}

HashMapMembership *DiskPageCache_i::PageFactory::create()
{
    /* get an unused page frame */
    Page *pPage = pdpc->free.getFirst();
    assert(pPage); /* we always keep one extra around */
    pPage->link.remove();

    /* initialize */
    pPage->pageId = pageId;
    pPage->flag = 0;
    pPage->nLocks = 0;
    pPage->pdpc = pdpc;

    /* load the page */
    fseek(pPage->pdpc->fp, pPage->pageId * DiskPageCache_i::Page::SIZE,
          SEEK_SET);
    fread(pPage->buf, sizeof(char), DiskPageCache_i::Page::SIZE,
          pPage->pdpc->fp);

    /* put the new page at the head (left) of the mfu list */
    pPage->pdpc->mfu.prepend(pPage);

    return &pPage->hmm;
}

bool DiskPageCache_i::PageWriter::visit(Page *pPage)
{
    /*
      If the page is dirty, seek to its position in the file, and write
      it out.
    */
    if (pPage->flag & Page::FDIRTY)
    {
        fseek(pPage->pdpc->fp, pPage->pageId * Page::SIZE, SEEK_SET);
        fwrite(pPage->buf, sizeof(char), Page::SIZE, pPage->pdpc->fp);
        pPage->flag &= ~Page::FDIRTY;
    }

    /* continue with the visit */
    return false;
}

DiskPageCache_i::PageWriter DiskPageCache_i::pageWriter;


void *DiskPageCache_i::PageLock_i::getData() const
{
    if (!pPage)
        return NULL; /* cache has been destroyed */
        
    return pPage->buf;
}

void DiskPageCache_i::PageLock_i::markDirty()
{
    if (!pPage)
        return; /* cache has been destroyed */

    pPage->flag |= DiskPageCache_i::Page::FDIRTY;
}

DiskPageCache_i::PageLock_i::~PageLock_i()
{
    if (!pPage)
        return; /* cache has been destroyed */

    /* the page is no longer locked */
    --pPage->nLocks;
    pPage->pdpc->locks.remove(this);
}


void DiskPageCache_i::zeroPage(FILE *fp)
{
    static const char buf[DiskPageCache_i::Page::SIZE] = {0};
                                       /* will be zeroed once at compile-time */

    fwrite(buf, sizeof(char), DiskPageCache_i::Page::SIZE, fp);
}

DiskPageCache::PageLock *DiskPageCache_i::find(unsigned long pageId)
{
    /*
      Validate page range.  The pageId must already exist, or be no
      more than one beyond the last page.  If not, then there's no such
      page.  In that case, we'll create it and all the intervening pages.
    */
    if (pageId > fileSize / DiskPageCache_i::Page::SIZE)
    {
        /* go to the current end of the file */
        fseek(fp, fileSize, SEEK_SET);

        /*
          Write out pages up to and including the requested page.  This
          has the handy side-effect of zeroing those pages, so there won't
          be garbage anywhere the user hasn't written.
        */
        while(pageId > fileSize / DiskPageCache_i::Page::SIZE)
        {
            DiskPageCache_i::zeroPage(fp);
            fileSize += DiskPageCache_i::Page::SIZE;
        }
    }

    /* look for the page in the cache */
    PageFactory pf(this, pageId);
    Page *pFound = (Page *)pageMap.find(&pageId, &pf);
    if (!pFound)
        return(NULL);

    /*
      For a page hit, we always move the page one place to the left on
      the mfu list, bringing it one place closer to the head of the
      list.  Note that it might already be at the head of the list, in
      which case we do nothing; we detect this by a lack of previous
      member. (The PageFactory places new pages at the head of the list, so
      those won't get moved here.)
    */
    Page *pPrevious = mfu.getPrevious(pFound);
    if (pPrevious)
    {
        mfu.remove(pFound);
        mfu.addBefore(pFound, pPrevious);
    }

    /* evict a page if we're at the maximum size */
    if (pageMap.getCount() >= maxPages)
    {
        /*
          Look for the least recently used page that is not locked.  To
          find a least recently used page, we start at the end (the
          far right) of the mfu list and work our way left until we find
          an unlocked page.

          Evicting this will make sure we have an empty slot available for
          the next request.

          LATER
          We might try to be more clever, and be prepared to evict this
          and other pages by writing them now, so that we have non-dirty
          older pages ready to evict if we need them. For now, we just assume
          the page's age is an indicator that there's no need for it anymore.
        */
        Page *pEvict;
        for(pEvict = mfu.getLast(); pEvict; pEvict = mfu.getPrevious(pEvict))
        {
            if (pEvict->nLocks)
                continue;

            DiskPageCache_i::pageWriter.visit(pEvict);

            /*
              Remove this page from the mfu list and the hash map.  Place it
              on the free list.
            */
            mfu.remove(pEvict);
            pageMap.remove(pEvict);
            free.append(pEvict);

            /*
              We successfully evicted a page, so we can stop looking for one
              to evict.
            */
            break;
        }
    }

    /* return the locked page */
    PageLock_i *pLock = new PageLock_i;
    pLock->pPage = pFound;
    ++pFound->nLocks;
    locks.append(pLock);
    return pLock;
}

size_t DiskPageCache_i::getPageSize() const
{
    return DiskPageCache_i::Page::SIZE;
}

void DiskPageCache_i::flush()
{
    pageMap.visit(&DiskPageCache_i::pageWriter);
}

DiskPageCache *DiskPageCache::create(
    const char *pFilename, const char *pMode, unsigned long maxPages)
{
    /* check on the file */
    struct stat sb;
    sb.st_size = 0; /* initialize this in case the file doesn't exist */
    int s = stat(pFilename, &sb);
    FILE *fp;

    /*
      If the file doesn't exist, and we want to read and write to it,
      create it, so the fopen call won't fail.
    */
    if (s && !strcmp(pMode, "r+"))
    {
        fp = fopen(pFilename, "w");
        DiskPageCache_i::zeroPage(fp);
        fclose(fp);
        sb.st_size = DiskPageCache_i::Page::SIZE;
    }

    /* try to open the requested file; if we can't, then return no cache */
    fp = fopen(pFilename, pMode);
    if (!fp)
        return NULL;
        
    DiskPageCache_i *pdpc = new DiskPageCache_i(maxPages);

    pdpc->fp = fp;
    pdpc->fileSize = sb.st_size;

    /*
      Allocate all the page frames in advance and put them on the page free
      list.

      We allocate one more than the maximum number of pages so there is
      always a free one we can allocate before evicting something.
    */
    pdpc->pPages = new DiskPageCache_i::Page[maxPages + 1];
    DiskPageCache_i::Page *pPage = pdpc->pPages;
    unsigned long i;
    for(i = maxPages; i; ++pPage, --i)
        pdpc->free.append(pPage);
    
    return pdpc;
}

DiskPageCache_i::~DiskPageCache_i()
{
    /*
      Invalidate any outstanding locks; deleting those lock structures
      is the client's responsibility.  We take the pagelocks off the
      list of locks here, because the list is no longer needed.
    */
    PageLock_i *pl;
    while((pl = locks.getFirst()))
    {
        pl->pPage = NULL;
        locks.remove(pl);
    }
        
    /* write out any dirty pages */
    flush();

    /* close the file */
    fclose(fp);

    /* remove everything from the hash table so we can delete the pages */
    pageMap.clear();

    /* free the array of pages */
    delete[] pPages;
}
