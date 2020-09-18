#pragma once
#include <stdbool.h>

/**
 * All asserts will call this function. GDB will catch any call to this function
 * and logs it
 */
inline static bool gunit_hook(uintmax_t expected, uintmax_t result, uintmax_t line_number, bool no) {
  return (expected == result) ^ no;
}

inline static bool gunit_array(void *expected, void *result, uintmax_t size, uintmax_t line_number, bool no) {
  uint8_t *ex = (uint8_t *) expected;
  uint8_t *re = (uint8_t *) result;

  // Check if equal
  for (uint32_t i = 0; i < size; ++i) {
    if (ex[i] != re[i])
      return gunit_hook(ex[i], re[i], line_number, no);
  }

  // If no differences were found
  return gunit_hook((uintmax_t) expected, (uintmax_t) result, line_number, !no);
}

/**
 * Assert if given values are equel.
 */
#define GASSERT(expected, result)  {if (!gunit_hook((uintmax_t) expected, (uintmax_t) result, __LINE__, false)) return;}

/**
 * Assert if given values are not equal.
 */
#define GNOT_ASSERT(not_expected, result) {if (!gunit_hook((uintmax_t) not_expected, (uintmax_t) result, __LINE__, true)) return;}

/**
 * Assert if given arrays contain same elements
 */
#define GARRAY_ASSERT(expected, result, nr_of_elements) {for (int __gi = 0; __gi < nr_of_elements; ++__gi) {if (!gunit_array((void *) expected, (void *) result, nr_of_elements * sizeof(typeof(expected[0])), __LINE__, false)) return;}}

/**
 * Assert if given arrays don't have same elements
 */
#define GARRAY_NOT_ASSERT(not_expected, result, nr_of_elements) {for (int __gi = 0; __gi < nr_of_elements; ++__gi) {if (!gunit_array((void *) not_expected, (void *) result, nr_of_elements * sizeof(typeof(not_expected[0])), __LINE__, true)) return;}}

/**
 * Assert if given structs contain same values
 */
#define GSTRUCT_ASSERT(expected, result) {if (!gunit_array((void *) &expected, (void *) &result, sizeof(typeof(expected)), __LINE__, false)) return;}

/**
 * Assert if given structs don't contain same values
 */
#define GSTRUCT_NOT_ASSERT(not_expected, result) {if (!gunit_array((void *) &not_expected, (void *) &result, sizeof(typeof(not_expected)), __LINE__, true)) return;}
