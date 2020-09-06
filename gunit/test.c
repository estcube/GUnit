#include <stdint.h>

void gunit_hook(uint32_t expected, uint32_t result, uint32_t line_number) {
  if (expected == result)
    return;

  while(1);
}

int main() {

  gunit_hook(3, 3, 12);

  return 0;
}
