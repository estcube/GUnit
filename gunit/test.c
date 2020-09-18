#include <stdint.h>

void gunit_hook(uint32_t expected, uint32_t result, uint32_t line_number) {
  if (expected == result)
    return;

  return;
}

int main() {

  gunit_hook(3, 3, __LINE__);
  gunit_hook(5, 333, __LINE__);
  gunit_hook(723, 99999999, __LINE__);

  return 0;
}
