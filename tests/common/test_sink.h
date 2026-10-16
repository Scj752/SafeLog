#pragma once

// test_sink.h —— 测试专用输出端：把收到的日志记录保存在内存中供断言。
// 同时也是“如何自定义 Sink 扩展 SafeLog”的活示例（开闭原则）。

#include <mutex>
#include <string>
#include <vector>

#include "safelog/sink.h"

class TestSink final : public safelog::ISink {
public:
  void Write(const safelog::LogRecord& record, const std::string& formatted) override {
    std::lock_guard<std::mutex> lock(mutex_);
    records_.push_back(record);
    formatted_lines_.push_back(formatted);
  }

  void Flush() override {}

  std::size_t Count() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return records_.size();
  }

  safelog::LogRecord At(std::size_t index) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return records_.at(index);
  }

  std::string FormattedAt(std::size_t index) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return formatted_lines_.at(index);
  }

private:
  mutable std::mutex mutex_;
  std::vector<safelog::LogRecord> records_;
  std::vector<std::string> formatted_lines_;
};
