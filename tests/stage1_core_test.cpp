// stage1_core_test.cpp —— 关卡 1：同步日志核心。
// 覆盖：LogLevel 转换、Logger 级别过滤、LogRecord 字段、ConsoleSink 基本写入。

#include <memory>
#include <string>

#include "safelog/console_sink.h"
#include "safelog/log_level.h"
#include "safelog/logger.h"
#include "test_framework.h"
#include "test_sink.h"

using namespace safelog;

TEST_CASE(LogLevel_ToString_RoundTrip) {
  const LogLevel kLevels[] = {LogLevel::kTrace, LogLevel::kDebug, LogLevel::kInfo,
                              LogLevel::kWarn,  LogLevel::kError, LogLevel::kFatal};
  for (const LogLevel level : kLevels) {
    const auto parsed = LogLevelFromString(ToString(level));
    CHECK_TRUE(parsed.has_value());
    CHECK_EQ(*parsed, level);
  }
}

TEST_CASE(LogLevel_FromString_Invalid) {
  CHECK_FALSE(LogLevelFromString("NOPE").has_value());
  CHECK_FALSE(LogLevelFromString("").has_value());
}

TEST_CASE(LogLevel_FromString_CaseInsensitive) {
  const auto parsed = LogLevelFromString("info");
  CHECK_TRUE(parsed.has_value());
  CHECK_EQ(*parsed, LogLevel::kInfo);
}

TEST_CASE(Logger_FiltersByLevel) {
  auto logger = std::make_shared<Logger>("core", LogLevel::kInfo);
  auto sink = std::make_shared<TestSink>();
  logger->AddSink(sink);

  logger->Debug("hidden");
  CHECK_EQ(sink->Count(), 0u);

  logger->Info("shown");
  CHECK_EQ(sink->Count(), 1u);
  CHECK_EQ(sink->At(0).message, std::string("shown"));

  logger->SetLevel(LogLevel::kDebug);
  logger->Debug("now visible");
  CHECK_EQ(sink->Count(), 2u);
}

TEST_CASE(Logger_RecordCarriesSourceInfo) {
  auto logger = std::make_shared<Logger>("core");
  auto sink = std::make_shared<TestSink>();
  logger->AddSink(sink);

  logger->Log(LogLevel::kWarn, "with-src", "main.cpp", 42);
  const LogRecord rec = sink->At(0);
  CHECK_EQ(rec.level, LogLevel::kWarn);
  CHECK_EQ(rec.message, std::string("with-src"));
  CHECK_EQ(rec.file, std::string("main.cpp"));
  CHECK_EQ(rec.line, 42);
}

TEST_CASE(ConsoleSink_WriteDoesNotThrow) {
  ConsoleSink sink;
  LogRecord record;
  record.level = LogLevel::kInfo;
  record.message = "hello console";
  sink.Write(record, "[INFO] hello console");
  sink.Flush();
  CHECK_TRUE(true);
}

MINI_TEST_MAIN()
