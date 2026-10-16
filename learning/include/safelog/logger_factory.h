#pragma once

// logger_factory.h —— 配置化工厂与日志器注册表（门面层）。
// 学习点：简单工厂模式、线程安全的 Meyers 单例、配置对象聚合参数。
//
// LoggerFactory：按 LoggerConfig 组装 formatter/sink/同步或异步 logger，
//                调用方无需感知具体类型拼装过程（封装变化）。
// LoggerRegistry：按名字登记/查找 logger，并提供全局默认 logger 供宏使用。

#include <cstddef>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "safelog/log_level.h"
#include "safelog/logger.h"

namespace safelog {

struct LoggerConfig {
  std::string name = "app";
  LogLevel level = LogLevel::kInfo;
  bool async = false;              // true 时创建 AsyncLogger
  bool console = true;             // 是否附加控制台输出
  std::string file_path;           // 为空则不写文件
  std::size_t max_bytes = 0;       // 文件滚动阈值，0 表示不滚动
  std::size_t max_files = 3;       // 滚动保留的历史文件数
  std::string pattern = "[%d %t][%l][%T] %m";
  std::size_t queue_capacity = 8192;  // 异步队列容量
};

class LoggerFactory {
public:
  // 按配置创建 logger（同步或异步由 config.async 决定，统一返回基类指针）。
  static std::shared_ptr<Logger> Create(const LoggerConfig& config);
};

class LoggerRegistry {
public:
  // Meyers 单例：函数内静态局部变量，C++11 起初始化线程安全。
  static LoggerRegistry& Instance();

  LoggerRegistry(const LoggerRegistry&) = delete;
  LoggerRegistry& operator=(const LoggerRegistry&) = delete;

  // 以 logger->name() 为键登记；重名或非法时返回 false。
  bool Register(std::shared_ptr<Logger> logger);
  // 按名字查找；不存在返回 nullptr。
  std::shared_ptr<Logger> Get(const std::string& name) const;

  void SetDefault(std::shared_ptr<Logger> logger);
  std::shared_ptr<Logger> Default() const;

  // 清空登记表与默认 logger（主要用于测试隔离）。
  void Clear();

private:
  LoggerRegistry() = default;

  mutable std::mutex mutex_;
  std::unordered_map<std::string, std::shared_ptr<Logger>> loggers_;
  std::shared_ptr<Logger> default_logger_;
};

}  // namespace safelog
