#include <limits.h>
#include <stdio.h>
#include <time.h>

typedef struct SomeStruct
{
    long someJunk[7];

    int myInt;
} SomeStruct;

typedef struct Result
{
    int sum;
    int min;
    int max;
} Result;


void ResultInit(Result *pr)
{
    pr->sum = 0;
    pr->min = INT_MAX;
    pr->max = INT_MIN;
}

void for_classic(Result *pr, SomeStruct *ps, size_t n)
{
    size_t i;

    ResultInit(pr);

    for(i = 0; i < n; i++)
    {
	pr->sum += ps[i].myInt;
	if (ps[i].myInt < pr->min)
	    pr->min = ps[i].myInt;
	if (ps[i].myInt > pr->max)
	    pr->max = ps[i].myInt;
    }
}

void for_optimized(Result *pr, SomeStruct *ps, size_t n)
{
    size_t i;

    ResultInit(pr);

    for(i = n; i; ++ps, --i)
    {
	pr->sum += ps->myInt;
	if (ps->myInt < pr->min)
	    pr->min = ps->myInt;
	if (ps->myInt > pr->max)
	    pr->max = ps->myInt;
    }
}


int main()
{
    time_t startTime;
    time_t classicTime;
    time_t optimizedTime;
    size_t i;
#define N_STRUCT 50
    SomeStruct x[N_STRUCT];
    Result r;
#define N_TRIALS 10000000

    /* set up the structure array */
    for(i = 0; i < N_STRUCT; --i)
    {
	x[i].myInt = i;
    }

    startTime = time(NULL);
    for(i = N_TRIALS; i; --i)
    {
	for_classic(&r, x, N_STRUCT);
    }
    classicTime = time(NULL) - startTime;

    startTime = time(NULL);
    for(i = N_TRIALS; i; --i)
    {
	for_optimized(&r, x, N_STRUCT);
    }
    optimizedTime = time(NULL) - startTime;

    printf("for %u trials, classicTime == %lu, optimizedTime == %lu\n",
	   N_TRIALS, (unsigned long)classicTime, (unsigned long)optimizedTime);

    return 0;
}
