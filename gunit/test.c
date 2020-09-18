#include <stdint.h>
#include "GUnit.h"

void test() {
  GASSERT(3, 3);
  GASSERT(15, 15);
  GASSERT('C', 'C');
  GASSERT(true, true);

  GNOT_ASSERT(3, 4);
  GNOT_ASSERT(15, 3000);
  GNOT_ASSERT('C', 'B');
  GNOT_ASSERT(true, false);

  uint8_t array1[] = {1, 2, 3};
  uint8_t array2[] = {1, 2, 3};
  uint8_t array3[] = {1, 2, 4};

  GARRAY_ASSERT(array1, array2, sizeof(array1));
  GARRAY_NOT_ASSERT(array1, array3, sizeof(array1));

  typedef struct {
    uint32_t a;
    uint16_t b;
    uint8_t c;
    uint8_t e;
  } test_t;

  test_t struct1 = {1, 2, 3, 4};
  test_t struct2 = {1, 2, 3, 4};
  test_t struct3 = {1, 3, 3, 4};

  GSTRUCT_ASSERT(struct1, struct2);
  GSTRUCT_NOT_ASSERT(struct1, struct3);
}

int main() {
  test();

  return 0;
}
