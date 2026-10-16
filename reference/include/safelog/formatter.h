#pragma once

// formatter.h —— 日志格式化抽象与模式串实现（抽象层 + 实现层）。
// 学习点：策略模式、const 正确性、不可变对象的天然线程安全。
//
// PatternFormatter 构造后 pattern_ 不再修改，Format 为 const 方法，
// 因此实例可被多线程并发调用而无需加锁。

#include <string>

#include "safelog/log_record.h"

namespace safelog {

class IFormatter {
public:
  virtual ~IFormatter() = default;
  virtual std::string Format(const LogRecord& record) const = 0;
};

// 按模式串格式化，支持占位符：
//   %l 级别   %m 消息   %t 时间(HH:MM:SS)   %d 日期(YYYY-MM-DD)
//   %T 线程id %s 源文件 %# 行号             %% 字面量 %
class PatternFormatter final : public IFormatter {
public:
  explicit PatternFormatter(std::string pattern = "[%d %t][%l][%T] %m");

  std::string Format(const LogRecord& record) const override;

private:
  std::string pattern_;
};

}  // namespace safelog
