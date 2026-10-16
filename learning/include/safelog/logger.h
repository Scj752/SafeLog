#pragma once

// logger.h —— 同步日志核心（核心层）。
// 学习点：原子操作、模板方法模式（Dispatch 虚函数）、委托构造函数思想。
//
// 职责：级别过滤 -> 构造/接收 LogRecord -> Dispatch 分发。
// 默认 Dispatch 实现为“格式化一次，写入所有 sink”；AsyncLogger 通过
// 重写 Dispatch 把记录投递到队列，从而复用本类的级别过滤与 sink 写入逻辑。

#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "safelog/formatter.h"
#include "safelog/log_level.h"
#include "safelog/log_record.h"
#include "safelog/sink.h"

namespace safelog {

class Logger {
public:
  explicit Logger(std::string name,
                  LogLevel level = LogLevel::kInfo,
                  std::shared_ptr<IFormatter> formatter = nullptr);
  virtual ~Logger() = default;

  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;

  const std::string& name() const noexcept;
  void SetLevel(LogLevel level) noexcept;
  LogLevel level() const noexcept;
  // 级别过滤：低于当前级别的日志直接丢弃。原子读，无锁。
  bool ShouldLog(LogLevel level) const noexcept;

  void AddSink(SinkPtr sink);

  // 主入口：过滤后交给 Dispatch。
  void Log(const LogRecord& record);
  // 便捷入口：由调用线程构造 LogRecord（自动捕获时间戳/线程 id）。
  void Log(LogLevel level, std::string message, std::string file = {}, int line = 0);

  void Trace(std::string message);
  void Debug(std::string message);
  void Info(std::string message);
  void Warn(std::string message);
  void Error(std::string message);
  void Fatal(std::string message);

  virtual void Flush();

protected:
  // 模板方法钩子：同步 Logger 直接写 sink；AsyncLogger 重写为入队。
  virtual void Dispatch(const LogRecord& record);
  // 格式化并写入所有 sink（AsyncLogger 工作线程复用）。
  void WriteToSinks(const LogRecord& record);

private:
  std::string name_;
  std::atomic<LogLevel> level_;
  std::shared_ptr<IFormatter> formatter_;
  mutable std::mutex sinks_mutex_;  // 仅保护 sinks_ 容器增删/快照
  std::vector<SinkPtr> sinks_;
};

}  // namespace safelog
