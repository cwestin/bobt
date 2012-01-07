/* Copyright 2011 Chris Westin.  All rights reserved. */
/*
  Example for discussion from "Using offsetof()," from "The Book of Brilliant
  Things:"  https://www.bookofbrilliantthings.com/eic/offsetof/searching-sorting
 */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include <bsearch.h>
#include <compare.h>

using namespace phoenix4cpp;


/* the definition of a student */
typedef struct
{
    unsigned key;
    const char *pLastName;
    const char *pFirstName;
} Student;


/* a table of students, sorted by key */
const Student enrolled[] =
{
    { 2, "Sommers", "Jamie"},
    { 4, "Austin", "Steve"},
    { 9, "Caidin", "Martin"},
};

/* a callback comparison function for students */
int compareStudent(const void *pl, const void *pr)
{
    const Student *pls = (const Student *)pl;
    const Student *prs = (const Student *)pr;

    if (pls->key < prs->key)
        return -1;

    if (pls->key > prs->key)
        return 1;

    return 0;
}


/* the definition of a professor */
typedef struct
{
    unsigned salary;
    const char *pFirstName;
    const char *pLastName;
    unsigned key;
} Professor;

/* a table of faculty, sorted by key */
const Professor faculty[] =
{
    {40000, "Gottfried", "Liebniz", 7},
    {45000, "Isaac", "Newton", 9},
    {50000, "Omar", "Khayyam", 11}
};

int compareProfessor(const void *pl, const void *pr)
{
    const Professor *plp = (const Professor *)pl;
    const Professor *prp = (const Professor *)pr;

    if (plp->key < prp->key)
        return -1;

    if (plp->key > prp->key)
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
    pStudent = (const Student *)bsearch(
        &studentKey, enrolled, sizeof(enrolled)/sizeof(Student),
        sizeof(Student), compareStudent);

    if (pStudent)
    {
        printf("ID: %d, last name: %s, first name: %s\n",
               pStudent->key, pStudent->pLastName, pStudent->pFirstName);
    }

    Professor professorKey;
                          /* a Professor structure we'll need to use as a key */
    professorKey.key = 7;
    pProfessor = (const Professor *)bsearch(
        &professorKey, faculty, sizeof(faculty)/sizeof(Professor),
        sizeof(Professor), compareProfessor);
    if (pProfessor)
    {
        printf("ID: %d, last name %s, first name: %s\n",
               pProfessor->key, pProfessor->pLastName, pProfessor->pFirstName);
    }
  
    /*
      Searching using bsearch() with offsets.
    */
    /* find the student with ID key */
    unsigned keyStudent = 9;
    pStudent = (const Student *)
        bsearch<Student, unsigned, offsetof(Student, key)>(
            &keyStudent, enrolled, sizeof(enrolled)/sizeof(Student),
            compareUnsigned);

    if (pStudent)
    {
        printf("ID: %d, last name: %s, first name: %s\n",
               pStudent->key, pStudent->pLastName, pStudent->pFirstName);
    }

    /* find the professor with ID key */
    unsigned keyProfessor = 11;
    pProfessor = (const Professor *)
        bsearch<Professor, unsigned, offsetof(Professor, key)>(
            &keyProfessor, faculty, sizeof(faculty)/sizeof(Professor),
            compareUnsigned);

    if (pProfessor)
    {
        printf("ID: %d, last name %s, first name: %s\n",
               pProfessor->key, pProfessor->pLastName, pProfessor->pFirstName);
    }

    return 0;
}
