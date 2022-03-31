#pragma once

#pragma GCC push_options
#pragma GCC optimize ("-O0")

#include <string>
#include <list>
#include <concepts>
#include <functional>
#include <chrono>
#include <type_traits>

#include <cstdint>
#include <cstring>
#include <csetjmp>
#include <cstdarg>

// For string literals ""s
using namespace std::literals;

namespace gunit {

struct test;

static test *current_ = nullptr;
static jmp_buf jump_env_;

struct test {
  static inline uint32_t running_index = 0;

public:
  const uint32_t index;
  const std::string name;
  const std::function<void()> routine;
  const uint32_t key;
  bool failed = false;
  std::string reason = "";
  float elapsed = 0.0f;
  uint32_t location = 0;
  const char *suite;

  static inline std::list<test*> tests = std::list<test*>();

  test(std::string name, const std::function<void()> routine, uint32_t key = ~0U, const char *suite = __builtin_FILE()) : index(running_index), name(name), routine(routine), key(key), suite(suite) {
    tests.push_back(this);
    running_index += 1;
  }
};

template<typename T>
concept numeric = std::integral<T> || std::floating_point<T> || std::is_enum_v<T>;

template<typename T>
concept iterable = requires(T container) {
  { container.begin() } -> std::iterator;
  { container.end() } -> std::iterator;
};

template<typename T>
concept carray = std::equality_comparable<T> && std::is_array<T>::value;

static void fail(const std::string &reason, int line = __builtin_LINE()) {
  current_->failed = true;
  current_->reason = reason;
  current_->location = line;

  // Exit the test
  longjmp(jump_env_, 1);
}

template<typename T>
struct affirm {
protected:
  const T &val;
  const size_t size;
  const int line;

  void fail(const std::string &reason) const noexcept {
    current_->failed = true;
    current_->reason = reason;
    current_->location = this->line;

    // Exit the test
    longjmp(jump_env_, 1);
  }

  template <typename S>
  constexpr std::string to_string(const S &arg) const noexcept requires numeric<S> {
    return std::to_string(arg);
  }

  constexpr std::string to_string(const auto &arg) const noexcept {
    return std::to_string((uintptr_t) &arg);
  }

public:
  constexpr affirm(const T &val, int line = __builtin_LINE()) noexcept : val(val), size(sizeof(val)), line(line) {}

  constexpr affirm(const T &val, size_t size, int line = __builtin_LINE()) noexcept : val(val), size(size), line(line) {}

  bool operator ==(const affirm<T> &other) const noexcept requires iterable<T> {
    if (std::equal(this->val, other.val))
      return true;


    this->fail("Assertion fail: "s + this->to_string(this->val) + " == "s + this->to_string(other.val));
    return false;
  }

  bool operator !=(const affirm<T> &other) const noexcept requires iterable<T> {
    if (!std::equal(this->val, other.val))
      return true;

    this->fail("Assertion fail: "s + this->to_string(this->val) + " != "s + this->to_string(other.val));
    return false;
  }

  // NOTE: the array has to have constant size
  bool operator ==(const affirm<T> &other) const noexcept requires carray<T> {
    for (uint32_t i = 0; i < this->size / sizeof(this->val[0]); ++i) {
      if (this->val[i] != other.val[i]) {
        this->fail("Assertion fail: "s + this->to_string(this->val[i]) + " == "s + this->to_string(other.val[i]) + " at index " + this->to_string(i));
        return false;
      }
    }

    return true;
  }

  // NOTE: the array has to have constant size
  bool operator !=(const affirm<T> &other) const noexcept requires carray<T> {
    for (uint32_t i = 0; i < this->size / sizeof(this->val[0]); ++i) {
      if (this->val[i] != other.val[i]) {
        return true;
      }
    }

    this->fail("Assertion fail: the arrays are equal"s);
    return false;
  }

  bool operator ==(const affirm<T> &other) const noexcept requires std::equality_comparable<T> {
    if (this->val == other.val)
      return true;

    this->fail("Assertion fail: "s + this->to_string(this->val) + " == "s + this->to_string(other.val));
    return false;
  }

  bool operator !=(const affirm<T> &other) const noexcept requires std::equality_comparable<T> {
    if (this->val != other.val)
      return true;

    this->fail("Assertion fail: "s + this->to_string(this->val) + " != "s + this->to_string(other.val));
    return false;
  }

  bool operator <(const affirm<T> &other) const noexcept requires std::totally_ordered<T> {
    if (this->val < other.val)
      return true;

    this->fail("Assertion fail: "s + this->to_string(this->val) + " < "s + this->to_string(other.val));
    return false;
  }

  bool operator >(const affirm<T> &other) const noexcept requires std::totally_ordered<T> {
    if (this->val > other.val)
      return true;

    this->fail("Assertion fail: "s + this->to_string(this->val) + " > "s + this->to_string(other.val));
    return false;
  }

  bool operator <=(const affirm<T> &other) const noexcept requires std::totally_ordered<T> {
    if (this->val <= other.val)
      return true;

    this->fail("Assertion fail: "s + this->to_string(this->val) + " <= "s + this->to_string(other.val));
    return false;
  }

  bool operator >=(const affirm<T> &other) const noexcept requires std::totally_ordered<T> {
    if (this->val >= other.val)
      return true;

    this->fail("Assertion fail: "s + this->to_string(this->val) + " >= "s + this->to_string(other.val));
    return false;
  }

  bool operator ==(const affirm<T> &other) const noexcept {
    if (0 == memcmp(&this->val, &other.val, this->size))
      return true;


    this->fail("Assertion fail: "s + this->to_string(this->val) + " == "s + this->to_string(other.val));
    return false;
  }

  bool operator !=(const affirm<T> &other) const noexcept {
    if (0 != memcmp(&this->val, &other.val, this->size))
      return true;

    this->fail("Assertion fail: "s + this->to_string(this->val) + " != "s + this->to_string(other.val));
    return false;
  }
};

namespace suite {
  /**
   * Fails all the tests in the suite
   */
  static inline void fail(std::string reason, uint32_t key = ~0U, const char *suite = __builtin_FILE()) noexcept {
    for (test *tes: test::tests) {
      if ((key != ~0U && tes->key != key) || tes->suite != suite)
        continue;

      tes->failed = true;
      tes->reason = reason;
    }
  }

  /**
   * Runs all the tests in the suite
   */
  static inline void run(const std::function<void()> before, const std::function<void()> after, const std::function<void()> onfail, uint32_t key = ~0U, const char *suite = __builtin_FILE()) noexcept {
    int jump_val = 0;

    if (test::tests.size() == 0)
      return;

    bool executed_tests[test::tests.size()] = {false};

start:

    for (test *tes: test::tests) {
      // Only execute unit tests with specified key
      if ((key != ~0U && tes->key != key) || tes->suite != suite)
        continue;

      // Indicate that this test was tested
      // Only the ones that are marked true are reported
      executed_tests[tes->index] = true;

      // Only execute unit tests that have not been failed already
      if (tes->failed)
        continue;

      // Update the global state, so failed asserts can be logged and processed
      current_ = tes;

      auto start_time = std::chrono::high_resolution_clock::now();

      // When an assert fails, we return here, but isntead of jump_val being 0, it
      // is 1. Therefore we don't rerun the test.
      jump_val = setjmp(jump_env_);
      if (jump_val == 0) {
        before();
        tes->routine();
        after();
      }

      // When this branch is selected, an assert failed
      else
        onfail();


      // Record the elapsed time
      std::chrono::duration<float, std::milli> test_duration = std::chrono::high_resolution_clock::now() - start_time;
      tes->elapsed = test_duration.count();
    }

    // Data collection
    int n = 0;
    for (bool tested: executed_tests) {
      if (tested)
        n += 1;
    }

    if (n == 0)
      return;

    const char *test_name[n];
    const char *test_reason[n];
    bool test_failed[n];
    float test_elapsed[n];
    uint32_t test_location[n];

    int i = 0;
    int j = 0;
    for (test *tes: test::tests) {
      if (executed_tests[j]) {
        test_name[i] = tes->name.c_str();
        test_failed[i] = tes->failed;
        test_reason[i] = tes->reason.c_str();
        test_elapsed[i] = tes->elapsed;
        test_location[i] = tes->location;
        ++i;
      }
      ++j;
    }

end:;
  }

  static inline void run(const std::function<void()> before, const std::function<void()> after, uint32_t key = ~0U, const char *suite = __builtin_FILE()) noexcept {
    suite::run(before, after, [](){}, key, suite);
  }

  static inline void run(uint32_t key = ~0U, const char *suite = __builtin_FILE()) noexcept {
    suite::run([](){}, [](){}, key, suite);
  }
} // namespace suite

static inline void end() {
  // Do nothing
}

} // namespace gunit

#pragma GCC pop_options
