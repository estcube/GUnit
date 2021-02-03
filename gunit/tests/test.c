#include "../GUnit.h"
#include <stddef.h>

void test0() {
  GASSERT(9, 9);
}

void test1() {
  GASSERT(5, 5);
}

void test2() {
  GASSERT(4, 3);
}

void test3() {
  GLESS_ASSERT(10, 3);
}

void test4() {
  GLESS_ASSERT(100, 100);
}

void test5() {
  GGREATER_ASSERT(100, 20);
}

int main() {
  GFAIL(test0);
  GSIMPLE_EXECUTE(test1, test2, test3, test4, test5);
  GEND();
  return 0;
}
