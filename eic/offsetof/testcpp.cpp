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
  //cout << "offsetof(Bar, x) == " << offsetof(Bar, x) << endl;
  //cout << "offsetof(Bar, x) == " << cppOffsetOf(Bar, x) << endl;
  cout << "offsetof(Bar, x) == " << myOffsetOf(Bar, x) << endl;
  cout << "offsetof(Bar, y) == " << myOffsetOf(Bar, y) << endl;
  //cout << "offsetof(Bar, z) == " << myOffsetOf(Bar, z) << endl;
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
  cout << "offsetof(Foo, a) == " << offsetof(Foo, a) << endl;
  cout << "offsetof(Foo, b) == " << offsetof(Foo, b) << endl;
  cout << "offsetof(Foo, c) == " << offsetof(Foo, c) << endl;

  int z = 42;
  Bar bar(z);
  bar.printOffsets();

  Bar::staticPrintOffsets();

  // cout << "non-member offsetof(Bar, y) == " << myOffsetOf(Bar, y) << endl;

  return 0;
}
