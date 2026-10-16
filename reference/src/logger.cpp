#include "safelog/logger.h"

#include <utility>

namespace safelog {

Logger::Logger(std::string name, LogLevel level, std::shared_ptr<IFormatter> formatter)
    : name_(std::move(name)),
      level_(level),
      formatter_(formatter ? std::move(formatter)
                           : std::make_shared<PatternFormatter>()) {}

const std::string& Logger::name() const noexcept { return name_; }

void Logger::SetLevel(LogLevel level) noexcept {
  level_.store(level, std::memory_order_relaxed);
}

LogLevel Logger::level() const noexcept {
  return level_.load(std::memory_order_relaxed);
}

bool Logger::ShouldLog(LogLevel level) const noexcept {
  return level >= this->level();
}

void Logger::AddSink(SinkPtr sink) {
  if (!sink) return;
  std::lock_guard<std::mutex> lock(sinks_mutex_);
  sinks_.push_back(std::move(sink));
}

void Logger::Log(const LogRecord& record) {
  if (!ShouldLog(record.level)) return;
  Dispatch(record);
}

void Logger::Log(LogLevel level, std::string message, std::string file, int line) {
  if (!ShouldLog(level)) return;
  LogRecord record;  // 默认成员初始化器在此刻捕获时间戳与调用线程 id
  record.level = level;
  record.message = std::move(message);
  record.file = std::move(file);
  record.line = line;
  Dispatch(record);
}

void Logger::Trace(std::string message) { Log(LogLevel::kTrace, std::move(message)); }
void Logger::Debug(std::string message) { Log(LogLevel::kDebug, std::move(message)); }
void Logger::Info(std::string message)  { Log(LogLevel::kInfo,  std::move(message)); }
void Logger::Warn(std::string message)  { Log(LogLevel::kWarn,  std::move(message)); }
void Logger::Error(std::string message) { Log(LogLevel::kError, std::move(message)); }
void Logger::Fatal(std::string message) { Log(LogLevel::kFatal, std::move(message)); }

void Logger::Flush() {
  std::vector<SinkPtr> sinks;
  {
    std::lock_guard<std::mutex> lock(sinks_mutex_);
    sinks = sinks_;  // 拷贝快照，缩短持锁时间，也避免回调中死锁
  }
  for (const auto& sink : sinks) {
    sink->Flush();
  }
}

void Logger::Dispatch(const LogRecord& record) { WriteToSinks(record); }

void Logger::WriteToSinks(const LogRecord& record) {
  const std::string formatted = formatter_->Format(record);  // 无锁：formatter 不可变
  std::vector<SinkPtr> sinks;
  {
    std::lock_guard<std::mutex> lock(sinks_mutex_);
    sinks = sinks_;
  }
  for (const auto& sink : sinks) {
    sink->Write(record, formatted);  // 各 sink 内部自带锁，保证线程安全
  }
}

}  // namespace safelog
