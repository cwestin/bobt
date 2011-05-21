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
