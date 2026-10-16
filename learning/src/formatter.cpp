// 【学习项目】关卡 2 练习文件：占位实现。请根据 TODO 完成，然后运行
//   ctest -R learn_stage2_format_file_test
// 对照参考实现：reference/src/formatter.cpp

#include "safelog/formatter.h"

#include <utility>

namespace safelog {

PatternFormatter::PatternFormatter(std::string pattern) : pattern_(std::move(pattern)) {}

std::string PatternFormatter::Format(const LogRecord& record) const {
  // TODO: 扫描 pattern_，遇 '%' 读取下一个字符作为占位符并替换：
  //         %l 级别（ToString）   %m 消息          %t 时间 HH:MM:SS
  //         %d 日期 YYYY-MM-DD    %T 线程 id       %s 源文件
  //         %# 行号               %% 字面量 %      未知占位符原样保留
  //       时间转换注意线程安全：Windows 用 localtime_s，其他平台用 localtime_r，
  //       不要用 std::localtime（静态缓冲区，多线程下数据竞争）
  (void)record;
  return {};
}

}  // namespace safelog
