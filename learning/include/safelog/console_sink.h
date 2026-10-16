#pragma once

// console_sink.h —— 控制台输出端（实现层）。
// 学习点：互斥锁保护共享资源（std::cout/cerr 多线程交错输出问题）。

#include <mutex>

#include "safelog/sink.h"

namespace safelog {

// WARN 及以上级别输出到 std::cerr，其余输出到 std::cout。
class ConsoleSink final : public ISink {
public:
  void Write(const LogRecord& record, const std::string& formatted) override;
  void Flush() override;

private:
  std::mutex mutex_;  // 保证多条日志不会交错输出
};

}  // namespace safelog
