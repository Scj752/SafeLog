#include "safelog/logger_factory.h"

#include <utility>
#include <vector>

#include "safelog/async_logger.h"
#include "safelog/console_sink.h"
#include "safelog/file_sink.h"
#include "safelog/formatter.h"

namespace safelog {

std::shared_ptr<Logger> LoggerFactory::Create(const LoggerConfig& config) {
  auto formatter = std::make_shared<PatternFormatter>(config.pattern);

  std::shared_ptr<Logger> logger =
      config.async
          ? std::static_pointer_cast<Logger>(std::make_shared<AsyncLogger>(
                config.name, config.level, formatter, config.queue_capacity))
          : std::make_shared<Logger>(config.name, config.level, formatter);

  if (config.console) {
    logger->AddSink(std::make_shared<ConsoleSink>());
  }
  if (!config.file_path.empty()) {
    logger->AddSink(
        std::make_shared<FileSink>(config.file_path, config.max_bytes, config.max_files));
  }
  return logger;
}

LoggerRegistry& LoggerRegistry::Instance() {
  static LoggerRegistry instance;  // C++11 保证静态局部变量初始化线程安全
  return instance;
}

bool LoggerRegistry::Register(std::shared_ptr<Logger> logger) {
  if (!logger || logger->name().empty()) return false;
  std::lock_guard<std::mutex> lock(mutex_);
  return loggers_.emplace(logger->name(), std::move(logger)).second;
}

std::shared_ptr<Logger> LoggerRegistry::Get(const std::string& name) const {
  std::lock_guard<std::mutex> lock(mutex_);
  const auto it = loggers_.find(name);
  return it == loggers_.end() ? nullptr : it->second;
}

void LoggerRegistry::SetDefault(std::shared_ptr<Logger> logger) {
  std::lock_guard<std::mutex> lock(mutex_);
  default_logger_ = std::move(logger);
}

std::shared_ptr<Logger> LoggerRegistry::Default() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return default_logger_;
}

void LoggerRegistry::Clear() {
  std::lock_guard<std::mutex> lock(mutex_);
  loggers_.clear();
  default_logger_.reset();
}

}  // namespace safelog
