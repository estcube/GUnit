#include "../GUnit.h"
#include <stddef.h>

void test1() {
  GASSERT(5, 5);
}

int main() {
  GSIMPLE_EXECUTE(test1);
  GEND();
  return 0;
}
