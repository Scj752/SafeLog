// 【学习项目】关卡 1 练习文件：占位实现。请根据 TODO 完成，然后运行
//   ctest -R learn_stage1_core_test
// 对照参考实现：reference/src/console_sink.cpp

#include "safelog/console_sink.h"

namespace safelog {

void ConsoleSink::Write(const LogRecord& record, const std::string& formatted) {
  // TODO: 持锁后输出 formatted 并换行：
  //       WARN 及以上级别（record.level >= LogLevel::kWarn）写 std::cerr，其余写 std::cout
  (void)record;
  (void)formatted;
}

void ConsoleSink::Flush() {
  // TODO: 持锁刷新 std::cout 与 std::cerr
}

}  // namespace safelog
