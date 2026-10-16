#pragma once

// sink.h —— 日志输出端抽象接口（抽象层）。
// 学习点：接口抽象（依赖倒置 DIP）、策略模式、里氏替换。
//
// Logger 只依赖 ISink 接口，不关心输出到控制台、文件还是网络。
// 新增输出端只需实现 ISink，无需改动 Logger（开闭原则 OCP）。
// 实现类必须保证 Write/Flush 本身线程安全（各自持有内部锁）。

#include <memory>
#include <string>

#include "safelog/log_record.h"

namespace safelog {

class ISink {
public:
  virtual ~ISink() = default;

  // 输出一条日志。record 为原始记录，formatted 为 Logger 统一格式化后的文本。
  // 由 Logger 格式化一次再分发给所有 sink，使 sink 与 formatter 完全解耦。
  virtual void Write(const LogRecord& record, const std::string& formatted) = 0;

  // 刷新缓冲，确保已写入内容落盘/输出。
  virtual void Flush() = 0;
};

using SinkPtr = std::shared_ptr<ISink>;

}  // namespace safelog
