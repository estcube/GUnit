#pragma once

#pragma GCC push_options
#pragma GCC optimize ("-O0")

#include <string>
#include <list>
#include <concepts>
#include <functional>
#include <chrono>

#include <cstdint>
#include <cstring>
#include <csetjmp>

// For string literals ""s
using namespace std::literals;

namespace gunit {

struct test;

static std::list<test*> tests_ = std::list<test*>();
static test *current_ = nullptr;
static jmp_buf jump_env_;

struct test {
public:
  const std::string name;
  const std::function<void()> routine;
  const uint32_t key;
  bool failed = false;
  std::string reason = "";
  float elapsed = 0.0f;
  uint32_t location = 0;

  test(std::string name, const std::function<void()> routine, uint32_t key = ~0U) : name(name), routine(routine), key(key) {
    tests_.push_back(this);
  }
};

template<typename T>
concept numeric = std::integral<T> || std::floating_point<T>;

template<typename T>
concept iterable = requires(T container) {
  { container.begin() } -> std::iterator;
  { container.end() } -> std::iterator;
};

template<typename T>
struct affirm {
protected:
  const T val;
  const size_t size;
  const int line;

  void fail(const std::string &reason) const noexcept {
    current_->failed = true;
    current_->reason = reason;
    current_->location = this->line;

    // Exit the test
    longjmp(jump_env_, 1);
  }

  constexpr std::string to_string(const T &arg) const noexcept requires numeric<T> {
    return std::to_string(arg);
  }

  constexpr std::string to_string(const T &arg) const noexcept {
    return std::to_string((uintptr_t) &arg);
  }

public:
  //constexpr affirm(T val, int line = __builtin_LINE()) noexcept : val(val), size(sizeof(val)), line(line) {}

  //constexpr affirm(T val, size_t size, int line = __builtin_LINE()) noexcept : val(val), size(size), line(line) {}

  constexpr affirm(const T val, int line = __builtin_LINE()) noexcept : val(val), size(sizeof(val)), line(line) {}

  constexpr affirm(const T val, size_t size, int line = __builtin_LINE()) noexcept : val(val), size(size), line(line) {}

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
  static inline void fail(std::string reason, uint32_t key = ~0U) noexcept {
    for (test *tes: tests_) {
      if (key != ~0U && tes->key != key)
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

start:

    for (test *tes: tests_) {
      // Only execute unit tests with specified key
      if (key != ~0U && tes->key != key)
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
    int n = tests_.size();
    const char *test_name[n];
    const char *test_reason[n];
    bool test_failed[n];
    float test_elapsed[n];
    uint32_t test_location[n];

    int i = 0;
    for (test *test: tests_) {
      test_name[i] = test->name.c_str();
      test_failed[i] = test->failed;
      test_reason[i] = test->reason.c_str();
      test_elapsed[i] = test->elapsed;
      test_location[i] = test->location;
      i += 1;
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
