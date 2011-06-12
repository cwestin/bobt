#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

/* the definition of a student */
typedef struct
{
  int key;
  char *pLastName;
  char *pFirstName;
} Student;


/* table of students, sorted by key */
const Student enrolled[] =
{
  { 2, "Sommers", "Jamie"},
  { 4, "Austin", "Steve"},
  { 9, "Caidin", "Martin"},
};

/* a callback comparison function for students */
int compareStudent(const void *p1, const void *p2)
{
  const Student *ps1 = (const Student *)p1;
  const Student *ps2 = (const Student *)p2;

  if (ps1->key < ps2->key)
    return -1;

  if (ps1->key > ps2->key)
    return 1;

  return 0;
}


/* the definition of a professor */
typedef struct
{
  int salary;
  char *pFirstName;
  char *pLastName;
  int key;
} Professor;

/* table of faculty, sorted by key */
const Professor faculty[] =
{
  {40000, "Gottfried", "Liebniz", 7},
  {45000, "Isaac", "Newton", 9},
  {50000, "Omar", "Khayyam", 11}
};

int compareProfessor(const void *p1, const void *p2)
{
  const Professor *ps1 = (const Professor *)p1;
  const Professor *ps2 = (const Professor *)p2;

  if (ps1->key < ps2->key)
    return -1;

  if (ps1->key > ps2->key)
    return 1;

  return 0;
}


void *bsearchOffset(
  const void *pkey,       /* pointer to key itself */
  const void *pbase,      /* pointer to base of array */
  size_t n,               /* number of items in array */
  size_t size,            /* size of one array element */
  size_t keyoffset,       /* offset of the key within an array element */
  int (*cmp)(const void *pk, const void *pd)
                           /* comparison routine */
  )
{
  size_t mid;       /* the middle array element's index */
  const char *pmid; /* a pointer to the middle array element */
  int cmpval;   /* the result of comparing the middle element with the key */

  /* keep dividing the array in half until we find a matching item */
  for(; n;)
  {
    mid = n / 2;
    pmid = ((const char *)pbase) + mid * size;
    cmpval = (*cmp)(pkey, (const void *)(pmid + keyoffset));

    /* do a three way comparison */
    if (!cmpval)
      return (void *)pmid;  /* we found a match */
    if (cmpval < 0)
      n = mid;  /* search an array one smaller than half size */
    else
    {
      /*
	Search an array one smaller than half the size, and starting one
	element after the midpoint.
      */ 
      n -= mid + 1;
      pbase = pmid + size;
    }
  }

  /* a matching item could not be found */
  return NULL;
}


int compareInt(const void *p1, const void *p2)
{
  const int *pi1 = (const int *)p1;
  const int *pi2 = (const int *)p2;

  if (*pi1 < *pi2)
    return -1;

  if (*pi1 > *pi2)
    return 1;

  return 0;
}


int main()
{
  const Student *pStudent; /* a pointer to a student we found */
  const Professor *pProfessor; /* a pointer to a professor we found */
  /*
    Searching using classic bsearch()
   */
  Student studentKey; /* a Student structure we'll need to use as a key */
  studentKey.key = 2;
  pStudent = (const Student *)bsearch(&studentKey, enrolled,
				      sizeof(enrolled)/sizeof(Student),
				      sizeof(Student),
				      compareStudent);
  if (pStudent)
  {
    printf("ID: %d, last name: %s, first name: %s\n",
	   pStudent->key, pStudent->pLastName, pStudent->pFirstName);
  }

  Professor professorKey; /* a Professor structure we'll need to use as a key */
  professorKey.key = 7;
  pProfessor = (const Professor *)bsearch(&professorKey, faculty,
					  sizeof(faculty)/sizeof(Professor),
					  sizeof(Professor),
					  compareProfessor);
  if (pProfessor)
  {
    printf("ID: %d, last name %s, first name: %s\n",
	   pProfessor->key, pProfessor->pLastName, pProfessor->pFirstName);
  }
  
  /*
    Searching using bsearchOffset()
  */
  /* find the student with ID key */
  int keyStudent = 9;
  pStudent = (const Student *)
    bsearchOffset(&keyStudent, enrolled,
		  sizeof(enrolled)/sizeof(Student),
		  sizeof(Student),
		  offsetof(Student, key), compareInt);

  if (pStudent)
  {
    printf("ID: %d, last name: %s, first name: %s\n",
	   pStudent->key, pStudent->pLastName, pStudent->pFirstName);
  }

  /* find the professor with ID key */
  int keyProfessor = 11;
  pProfessor = (const Professor *)
    bsearchOffset(&keyProfessor, faculty,
		  sizeof(faculty)/sizeof(Professor),
		  sizeof(Professor),
		  offsetof(Professor, key), compareInt);

  if (pProfessor)
  {
    printf("ID: %d, last name %s, first name: %s\n",
	   pProfessor->key, pProfessor->pLastName, pProfessor->pFirstName);
  }

  return 0;
}
