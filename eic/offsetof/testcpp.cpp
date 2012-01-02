/* Copyright 2011 Chris Westin.  All rights reserved. */
/*
  Example for discussion from "Using offsetof()," from "The Book of Brilliant
  Things:"  https://www.bookofbrilliantthings.com/eic/offsetof
 */

#include <cstddef>
#include <iostream>

using namespace std;

struct Foo
{
  int a;
  int b;
  int c;
};

class Bar
{
public:
  void printOffsets() const;

  static void staticPrintOffsets();

  static const int offsets[];

  Bar(int &z);
private:
  int x;
  int y;
  int &z;
};

Bar::Bar(int &_z):
  x(11),
  y(17),
  z(_z)
{
}

#define cppOffsetOf(s, m) ((size_t)&(((s *)NULL)->m))
#define myOffsetOf(s, m) ((size_t)((((char *)&(((s *)1)->m)) - 1)))

void Bar::printOffsets() const
{
  // cout << "offsetof(Bar, x) == " << offsetof(Bar, x) << endl;
  /*
    g++ 3.4.4 says:
    testcpp.cpp:41: warning: invalid access to non-static data member `Bar::x' of NULL object
    testcpp.cpp:41: warning: (perhaps the `offsetof' macro was used incorrectly)
  */

  // cout << "offsetof(Bar, x) == " << cppOffsetOf(Bar, x) << endl;
  /*
    g++ 3.4.4 says:
    testcpp.cpp:48: warning: invalid access to non-static data member `Bar::x' of NULL object
    testcpp.cpp:48: warning: (perhaps the `offsetof' macro was used incorrectly)
  */

  cout << "offsetof(Bar, x) == " << myOffsetOf(Bar, x) << endl;

  cout << "offsetof(Bar, y) == " << myOffsetOf(Bar, y) << endl;

  // cout << "offsetof(Bar, z) == " << myOffsetOf(Bar, z) << endl;
  /*
    This would crash at run-time.
  */
}

void Bar::staticPrintOffsets()
{
  cout << "static offsetof(Bar, x) == " << myOffsetOf(Bar, x) << endl;
  cout << "static offsetof(Bar, y) == " << myOffsetOf(Bar, y) << endl;
}

const int Bar::offsets[] =
{
  myOffsetOf(Bar, x),
  myOffsetOf(Bar, y),
};  

int main()
{
  /*
    The classical definition works fine for PODs:  "Plain Old Data
    structures"
  */
  cout << "offsetof(Foo, a) == " << offsetof(Foo, a) << endl;
  cout << "offsetof(Foo, b) == " << offsetof(Foo, b) << endl;
  cout << "offsetof(Foo, c) == " << offsetof(Foo, c) << endl;

  int z = 42;
  Bar bar(z);
  bar.printOffsets();

  Bar::staticPrintOffsets();

  // cout << "non-member offsetof(Bar, y) == " << myOffsetOf(Bar, y) << endl;
  /*
    g++ 3.4.4 says:
    testcpp.cpp:25: error: `int Bar::y' is private
    testcpp.cpp:93: error: within this context
  */

  return 0;
}
