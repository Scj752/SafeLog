// stage4_factory_test.cpp —— 关卡 4：工厂、注册表与宏封装。
// 覆盖：LoggerFactory 三类创建、LoggerRegistry 登记/查找/默认 logger、宏的源位置捕获。

#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>

#include "safelog/log_macros.h"
#include "safelog/logger_factory.h"
#include "test_framework.h"
#include "test_sink.h"

using namespace safelog;
namespace fs = std::filesystem;

namespace {

std::string ReadAll(const fs::path& path) {
  std::ifstream in(path, std::ios::binary);
  std::ostringstream oss;
  oss << in.rdbuf();
  return oss.str();
}

fs::path TempDir(const std::string& name) {
  fs::path dir = fs::temp_directory_path() / ("safelog_" + name);
  std::error_code ec;
  fs::remove_all(dir, ec);
  fs::create_directories(dir);
  return dir;
}

std::size_t CountLines(const std::string& text) {
  std::size_t lines = 0;
  for (const char c : text) {
    if (c == '\n') ++lines;
  }
  return lines;
}

}  // namespace

TEST_CASE(Factory_CreatesConsoleLogger) {
  LoggerConfig config;
  config.name = "console";
  auto logger = LoggerFactory::Create(config);
  CHECK_TRUE(logger != nullptr);
  CHECK_EQ(logger->name(), std::string("console"));
  CHECK_TRUE(logger->ShouldLog(LogLevel::kInfo));
  CHECK_FALSE(logger->ShouldLog(LogLevel::kDebug));
}

TEST_CASE(Factory_CreatesFileLogger) {
  const fs::path dir = TempDir("factory_file");
  const fs::path file = dir / "factory.log";
  LoggerConfig config;
  config.name = "file";
  config.console = false;
  config.file_path = file.string();
  auto logger = LoggerFactory::Create(config);
  logger->Info("factory hello");
  logger->Flush();
  CHECK_TRUE(ReadAll(file).find("factory hello") != std::string::npos);
  std::error_code ec;
  fs::remove_all(dir, ec);
}

TEST_CASE(Factory_CreatesAsyncLogger) {
  const fs::path dir = TempDir("factory_async");
  const fs::path file = dir / "async.log";
  constexpr std::size_t kCount = 200;
  {
    LoggerConfig config;
    config.name = "async";
    config.async = true;
    config.console = false;
    config.file_path = file.string();
    config.queue_capacity = 256;
    auto logger = LoggerFactory::Create(config);
    for (std::size_t i = 0; i < kCount; ++i) {
      logger->Info("line-" + std::to_string(i));
    }
    logger->Flush();
  }  // 析构后文件可读
  CHECK_EQ(CountLines(ReadAll(file)), kCount);
  std::error_code ec;
  fs::remove_all(dir, ec);
}

TEST_CASE(Registry_RegisterAndGet) {
  auto& registry = LoggerRegistry::Instance();
  registry.Clear();
  auto logger = std::make_shared<Logger>("reg-test");
  CHECK_TRUE(registry.Register(logger));
  CHECK_FALSE(registry.Register(logger));  // 重名拒绝
  CHECK_EQ(registry.Get("reg-test").get(), logger.get());
  CHECK_TRUE(registry.Get("missing") == nullptr);
  registry.Clear();
}

TEST_CASE(Registry_DefaultLogger) {
  auto& registry = LoggerRegistry::Instance();
  registry.Clear();
  CHECK_TRUE(registry.Default() == nullptr);
  auto logger = std::make_shared<Logger>("default");
  registry.SetDefault(logger);
  CHECK_EQ(registry.Default().get(), logger.get());
  registry.Clear();
}

TEST_CASE(Macro_CapturesSourceLocation) {
  auto logger = std::make_shared<Logger>("macro", LogLevel::kInfo);
  auto sink = std::make_shared<TestSink>();
  logger->AddSink(sink);

  SAFELOG_INFO(logger, "macro message");
  CHECK_EQ(sink->Count(), 1u);
  CHECK_EQ(sink->At(0).message, std::string("macro message"));
  CHECK_TRUE(sink->At(0).line > 0);
  CHECK_TRUE(sink->At(0).file.find("stage4") != std::string::npos);
}

TEST_CASE(Macro_RespectsLevelBeforeEvaluating) {
  auto logger = std::make_shared<Logger>("macro-level", LogLevel::kError);
  auto sink = std::make_shared<TestSink>();
  logger->AddSink(sink);

  SAFELOG_INFO(logger, "skip");  // 低于级别，不应产生记录
  CHECK_EQ(sink->Count(), 0u);
  SAFELOG_ERROR(logger, "keep");
  CHECK_EQ(sink->Count(), 1u);
  CHECK_EQ(sink->At(0).message, std::string("keep"));
}

MINI_TEST_MAIN()
