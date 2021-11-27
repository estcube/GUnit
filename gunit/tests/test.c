#include "../GUnit.h"
#include <stddef.h>

void equal1() {
  GASSERT(9, 9);
}

void equal2() {
  GASSERT(5, 5);
}

void notequal() {
  GASSERT(4, 3);
}

void less() {
  GLESS_ASSERT(10, 3);
}

void notless() {
  GLESS_ASSERT(100, 100);
}

void greater() {
  GGREATER_ASSERT(20, 100);
}

void notgreater() {
  GGREATER_ASSERT(100, 20);
}

void in() {
  GINTERVAL_ASSERT(10, 15, 13);
}

void notin() {
  GINTERVAL_ASSERT(10, 15, 20);
}

void booltrue() {
  GBOOL(true);
}

void boolfalse() {
  GBOOL(false);
}

void typefloatequal() {
  GASSERT(1.7, 1.7);
}

void typefloatnotequal() {
  GASSERT(1.7, 2.3);
}

void typeuint16equal() {
  uint16_t a = 10;
  uint16_t b = 10;
  GASSERT(a, b);
}

void typeuint16notequal() {
  uint16_t a = 10;
  uint16_t b = 1000;
  GASSERT(a, b);
}

void typeint8equal() {
  int8_t a = -15;
  int8_t b = -15;
  GASSERT(a, b);
}

void typeint8notequal() {
  int8_t a = 125;
  int8_t b = -115;
  GASSERT(a, b);
}

void forfail() {
  GASSERT(20, 20);
}

int main() {
  GFAIL(forfail);
  GSIMPLE_EXECUTE(
      equal1,
      equal2,
      notequal,
      less,
      notless,
      greater,
      notgreater,
      in,
      notin,
      booltrue,
      boolfalse,
      typefloatequal,
      typefloatnotequal,
      typeuint16equal,
      typeuint16notequal,
      typeint8equal,
      typeint8notequal
  );
  GEND();
  return 0;
}
