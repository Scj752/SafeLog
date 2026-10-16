// demo.cpp —— SafeLog 参考实现演示：工厂 + 注册表 + 异步 + 宏。
#include <string>

#include "safelog/log_macros.h"
#include "safelog/logger_factory.h"

int main() {
  using namespace safelog;

  LoggerConfig config;
  config.name = "demo";
  config.level = LogLevel::kDebug;
  config.async = true;  // 异步模式：日志先入队，工作线程落盘
  config.console = true;
  config.file_path = "logs/demo.log";
  config.max_bytes = 1024 * 1024;  // 1MB 滚动

  auto logger = LoggerFactory::Create(config);
  auto& registry = LoggerRegistry::Instance();
  registry.Register(logger);
  registry.SetDefault(logger);

  SAFELOG_INFO(logger, "SafeLog demo started");
  SAFELOG_D("default logger is ready");
  for (int i = 0; i < 5; ++i) {
    SAFELOG_I("async message " + std::to_string(i));
  }
  logger->Flush();  // 等待队列排空并刷新
  SAFELOG_INFO(logger, "demo finished, see logs/demo.log");
  logger->Flush();
  return 0;
}
