#pragma once

// test_framework.h —— 约 60 行的迷你单元测试框架（零外部依赖）。
// 供参考实现与学习项目共用，保证“关卡”判定标准完全一致。
//
// 用法：
//   TEST_CASE(MyTest_Name) { CHECK_TRUE(1 + 1 == 2); }
//   MINI_TEST_MAIN()

#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace mini_test {

struct TestCase {
  std::string name;
  void (*fn)();
};

inline std::vector<TestCase>& Registry() {
  static std::vector<TestCase> tests;
  return tests;
}

struct Registrar {
  Registrar(const char* name, void (*fn)()) { Registry().push_back({name, fn}); }
};

[[noreturn]] inline void Fail(const std::string& expr, const char* file, int line) {
  throw std::runtime_error(expr + "  (" + file + ":" + std::to_string(line) + ")");
}

inline int RunAll() {
  int passed = 0;
  int failed = 0;
  for (const auto& tc : Registry()) {
    try {
      tc.fn();
      ++passed;
      std::cout << "[PASS] " << tc.name << '\n';
    } catch (const std::exception& e) {
      ++failed;
      std::cout << "[FAIL] " << tc.name << " -> " << e.what() << '\n';
    } catch (...) {
      ++failed;
      std::cout << "[FAIL] " << tc.name << " -> unknown exception\n";
    }
  }
  std::cout << "==== " << passed << " passed, " << failed << " failed, "
            << Registry().size() << " total ====\n";
  return failed == 0 ? 0 : 1;
}

}  // namespace mini_test

#define TEST_CASE(name)                                   \
  static void name();                                     \
  static ::mini_test::Registrar _reg_##name(#name, name); \
  static void name()

#define CHECK_TRUE(cond)                                        \
  do {                                                          \
    if (!(cond)) ::mini_test::Fail("CHECK_TRUE(" #cond ")", __FILE__, __LINE__); \
  } while (0)

#define CHECK_FALSE(cond) CHECK_TRUE(!(cond))

#define CHECK_EQ(a, b)                                                     \
  do {                                                                     \
    if (!((a) == (b))) ::mini_test::Fail("CHECK_EQ(" #a ", " #b ")", __FILE__, __LINE__); \
  } while (0)

#define MINI_TEST_MAIN() int main() { return ::mini_test::RunAll(); }
