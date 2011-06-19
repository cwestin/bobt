/* Copyright 2011 Chris Westin.  All rights reserved. */
/*
  Example for discussion from "Using offsetof()," from "The Book of Brilliant
  Things:"  https://www.bookofbrilliantthings.com/eic/offsetof
 */

#include <stdio.h>
#include <stddef.h>

struct foo
{
  int a;
  int b;
  int c;
};
typedef struct foo foo;

int main()
{
  printf("offsetof(foo, a) == %u\n", offsetof(foo, a));
  printf("offsetof(foo, b) == %u\n", offsetof(foo, b));
  printf("offsetof(foo, c) == %u\n", offsetof(foo, c));

  return 0;
}
