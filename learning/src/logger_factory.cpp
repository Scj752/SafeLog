// 【学习项目】关卡 4 练习文件：占位实现。请根据 TODO 完成，然后运行
//   ctest -R learn_stage4_factory_test
// 对照参考实现：reference/src/logger_factory.cpp
//
// 前置：需先完成关卡 1~3（Logger/ConsoleSink/FileSink/AsyncLogger 均可用）。

#include "safelog/logger_factory.h"

#include <utility>

namespace safelog {

std::shared_ptr<Logger> LoggerFactory::Create(const LoggerConfig& config) {
  // TODO: 1) 用 config.pattern 创建 PatternFormatter
  //       2) 按 config.async 创建 AsyncLogger 或 Logger（统一以 shared_ptr<Logger> 返回，
  //          异步类型可用 std::static_pointer_cast 向上转换）
  //       3) config.console 为 true 时 AddSink(ConsoleSink)
  //       4) config.file_path 非空时 AddSink(FileSink)（带上 max_bytes/max_files）
  //       5) 返回 logger
  (void)config;
  return nullptr;
}

LoggerRegistry& LoggerRegistry::Instance() {
  static LoggerRegistry instance;  // Meyers 单例，已为你实现（C++11 线程安全）
  return instance;
}

bool LoggerRegistry::Register(std::shared_ptr<Logger> logger) {
  // TODO: logger 为空或名字为空返回 false；
  //       否则持锁用 emplace 插入 loggers_，返回是否真正插入（重名拒绝）
  (void)logger;
  return false;
}

std::shared_ptr<Logger> LoggerRegistry::Get(const std::string& name) const {
  // TODO: 持锁查找，不存在返回 nullptr
  (void)name;
  return nullptr;
}

void LoggerRegistry::SetDefault(std::shared_ptr<Logger> logger) {
  // TODO: 持锁保存到 default_logger_
  (void)logger;
}

std::shared_ptr<Logger> LoggerRegistry::Default() const {
  // TODO: 持锁返回 default_logger_
  return nullptr;
}

void LoggerRegistry::Clear() {
  // TODO: 持锁清空 loggers_ 并重置 default_logger_
}

}  // namespace safelog
