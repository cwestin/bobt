
#include <cassert>
#include <cstring>

#include "bsearch.h"

using namespace phoenix4cpp;

template<class T>
int compare(const T *pl, const T *pr)
{
    if (*pl < *pr)
	return -1;
    if (*pl == *pr)
	return 0;

    return 1;
}

/* ------------------------------------------------------------------------- */

/*
template<>
int compare<char *>(const char **pl, const char **pr)
{
    return strcmp(*pl, *pr);
}
*/

typedef const char *charstar;

template<>
int compare<charstar>(const charstar *pl, const charstar *pr);

template<>
int compare<charstar>(const charstar *pl, const charstar *pr)
{
    return strcmp(*pl, *pr);
}

template<class T>
int compareReverse(const T *pl, const T *pr)
{
    return -1*compare<T>(pl, pr);
}

/* ------------------------------------------------------------------------- */

template<class T, class K, size_t keyOffset>
inline const T *tbsearch(const K *pKey, const T *pArray, size_t n,
			 int (*cmp)(const K *pl, const K *pr) = compare<K>)
{
    return (const T *)bsearch(
	(const void *)pKey, (const void *)pArray, n, sizeof(T),
	keyOffset, (int (*)(const void *, const void *))cmp);
}

int main()
{
    const int five = 5;
    const int seven = 7;
    assert(compare<int>(&five, &seven) < 0);
    assert(compareReverse<int>(&five, &seven) > 0);

    const unsigned long twelve = 12;
    const unsigned long fifteen = 15;
    assert(compare<unsigned long>(&fifteen, &twelve) > 0);
    assert(compareReverse<unsigned long>(&fifteen, &twelve) < 0);

    const char *pBfoo = "bfoo";
    const char *pBar = "bar";
    //assert(compare<char *>(&pBfoo, &pBar) > 0);
    assert(compare<charstar>(&pBfoo, &pBar) > 0);
    assert(compareReverse<charstar>(&pBfoo, &pBar) < 0);

    unsigned u[3];
    u[0] = 17;
    u[1] = 42;
    u[2] = 2010;
    tbsearch<unsigned, unsigned, 0>(&u[1], u, 3, compare<unsigned>);
    tbsearch<unsigned int, unsigned int, 0>(&u[1], u, 3);

    unsigned long ul[3];
    ul[0] = 17;
    ul[1] = 42;
    ul[2] = 2010;
    tbsearch<unsigned long, unsigned long, 0>(&ul[1], ul, 3);

    const char *const ps[3] = {"alpha", "bravo", "charlie"};
    //tbsearch<char *, char *, 0>(&ps[2], ps, sizeof(ps));
    tbsearch<charstar, charstar, 0>(&ps[2], ps, 3);

    return 0;
}
