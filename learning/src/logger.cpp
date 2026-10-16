// 【学习项目】关卡 1 练习文件：占位实现。请根据 TODO 完成，然后运行
//   ctest -R learn_stage1_core_test
// 对照参考实现：reference/src/logger.cpp

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
  // TODO: 以 std::memory_order_relaxed 存入 level_
  (void)level;
}

LogLevel Logger::level() const noexcept {
  // TODO: 以 std::memory_order_relaxed 读取并返回 level_
  return LogLevel::kInfo;
}

bool Logger::ShouldLog(LogLevel level) const noexcept {
  // TODO: 返回 level >= this->level()（低于当前级别的日志被过滤）
  (void)level;
  return false;
}

void Logger::AddSink(SinkPtr sink) {
  // TODO: sink 为空直接返回；否则持锁 push_back 到 sinks_
  (void)sink;
}

void Logger::Log(const LogRecord& record) {
  // TODO: ShouldLog 过滤后调用 Dispatch(record)
  (void)record;
}

void Logger::Log(LogLevel level, std::string message, std::string file, int line) {
  // TODO: ShouldLog 过滤后构造 LogRecord（逐个字段赋值，时间戳/线程 id 由
  //       默认成员初始化器自动捕获），再 Dispatch(record)
  (void)level;
  (void)message;
  (void)file;
  (void)line;
}

void Logger::Trace(std::string message) { /* TODO: 转发 Log(LogLevel::kTrace, ...) */ (void)message; }
void Logger::Debug(std::string message) { /* TODO: 转发 Log(LogLevel::kDebug, ...) */ (void)message; }
void Logger::Info(std::string message)  { /* TODO: 转发 Log(LogLevel::kInfo, ...)  */ (void)message; }
void Logger::Warn(std::string message)  { /* TODO: 转发 Log(LogLevel::kWarn, ...)  */ (void)message; }
void Logger::Error(std::string message) { /* TODO: 转发 Log(LogLevel::kError, ...) */ (void)message; }
void Logger::Fatal(std::string message) { /* TODO: 转发 Log(LogLevel::kFatal, ...) */ (void)message; }

void Logger::Flush() {
  // TODO: 持锁拷贝 sinks_ 快照（缩短临界区），解锁后逐个调用 sink->Flush()
}

void Logger::Dispatch(const LogRecord& record) {
  // TODO: 调用 WriteToSinks(record)（本方法也是 AsyncLogger 的重写点）
  (void)record;
}

void Logger::WriteToSinks(const LogRecord& record) {
  // TODO: 1) 用 formatter_->Format(record) 得到文本（无需加锁，formatter 不可变）
  //       2) 持锁拷贝 sinks_ 快照，解锁后逐个 sink->Write(record, formatted)
  (void)record;
}

}  // namespace safelog
