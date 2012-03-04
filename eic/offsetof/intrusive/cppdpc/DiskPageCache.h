/* Copyright 2012 Chris Westin.  All rights reserved. */
/*
  Example for discussion from "Intrusive Data Structures," from
  "The Book of Brilliant Things:"
  https://www.bookofbrilliantthings.com/book/eic/offsetof/intrusive-data-structures
 */

/*
  The disk page cache caches a number of pages from a disk file in memory.

  One obvious application is a simple non-transacted database which
  keeps a set of records in a file.  The entire file may be too large
  to keep in memory.   If more complex index structures are built,
  various pages may be accessed repeatedly, while those containing
  target records may only be accessed sporadically.

  The disk page cache provides a simple mechanism for requesting access
  to disk pages (equal sized blocks of file content) according to a
  page number.  Page content can be read from and written to.  Pages can
  be marked dirty, which will cause them to be written back to disk
  before they are removed from memory.

  The disk page cache caches a number of pages chosen at the time it
  is created.  When that number is reached, each request for a page
  that is not yet loaded will cause a loaded page's memory image to be
  discarded.  Dirty pages are written back to disk before they are discarded.
  The replacement policy roughly approximates least frequently used.
  Pages in the cache are kept on a list, considered to go from left
  to right.  Each time a new page is loaded, it is put on the head of
  the list, on the left.  When a page is requested and found to be
  already loaded (a cache hit), that page is removed from the list,
  and put back on the list one position towards the left;  pages that
  are already at the head of the list stay there.  When a page must
  be selected for replacement, the rightmost page (at the end of the
  list) is selected.  Pages that are used frequently tend to move to the
  left, and those that are not used, or that are used less frequently,
  tend to drift to the right, hence the notion of least frequent usage.

  More than one page can be examined at a time.  To prevent a page
  that is being examined from being discarded when a second page is
  requested, a pagelock is issued.  A locked page cannot be discarded
  until the pagelock is destroyed.  If the maximum number of
  pages should be exceeded, the cache will shrink back down to the originally
  specified size over time as references are made to pages that are already
  loaded.

  The first time a page is requested, it will be zeroed.

  The disk page cache is presented as an abstract class, and it is the
  user's responsibility to ensure that it is cleaned up.  Page locks
  are also abstract classes that the user must explicitly clean up.
	
  The current implementation is not MT safe; the goal here is to demonstrate
  the use of multiple intrusive data structures, so the implementation is
  not necessarily complete on all fronts.
*/

#ifndef DISKPAGECACHE_H
#define DISKPAGECACHE_H

#include <stddef.h>


class DiskPageCache
{
public:

    /*
      Destructor.

      Flushes any unwritten dirty pages to disk, and cleans up internal
      data structures. Any data pointers still held by the client
      become invalid. Any outstanding PageLocks held become invalid,
      and operations on them will do nothing; Pagelock::GetData() on those will
      return NULL.
    */
    virtual ~DiskPageCache() {};

    /*
      Create a disk page cache.

      @param pFilename the name of the file to cache
      @param pMode mode string for fopen, with the exception that using "r+"
        will cause the file to be created if it does not already exist
      @param maxPages the maximum size of the cache, in pages
      @returns the newly created cache, or NULL if it could not be created
        (file didn't exist, illegal open modes, ...)
    */
    static DiskPageCache *create(
	const char *pFilename, const char *pMode, unsigned long maxPages);

    class PageLock
    {
    public:
	/*
	  Destructor.

	  Releases this PageLock.
	*/
	virtual ~PageLock() {};

	/*
	  Return a pointer to the data for the page associated with this
	  lock.  The data pointer becomes invalid if this PageLock is
	  destroyed, or if the spawning DiskPageCache is destroyed.  The length
	  of the data page is given by DiskPageCache::GetPageSize().

	  @returns pointer to the locked page's data
	*/
	virtual void *getData() const = 0;
		
	/*
	  Marks the page represented by this PageLock as dirty.
	*/
	virtual void markDirty() = 0;
    };
	
    /*
      Find the page pageId.

      Pages are numbered from 0 up.  It is an error to request a page that is
      more than one page beyond the end of the cached file; a request for the
      page one beyond the end of the file extends the file to accomodate that
      page.

      If pageId does not exist, returns NULL.  The same page may be locked
      more than once.  A page cannot be removed from the cache while it is
      locked.  If the number of locked pages equals the number of pages that
      are to be cached, and a page request is made for a page that is not
      already in the cache, the cache will be grown beyond its original size
      to accomodate the new page.

      @param pageId the id of the page sought
      @returns a pointer to the lock, if the page was found
    */
    virtual PageLock *find(unsigned long pageId) = 0;

    /*
      Returns the size of the cached disk pages; this is the length of data
      pointed to by the return value of a PageLock::GetData() call.

      @returns size of cached pages
    */
    virtual size_t getPageSize() const = 0;
	
    /*
      Writes any unwritten dirty pages back to disk.
    */
    virtual void flush() = 0;
};

#endif /* DISKPAGECACHE_H */
