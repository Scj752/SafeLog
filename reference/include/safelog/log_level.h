#pragma once

// log_level.h —— 日志级别定义与字符串转换（基础设施层，零依赖）。
// 学习点：enum class 类型安全、std::optional 表达“可能失败”的返回值。

#include <optional>
#include <string>
#include <string_view>

namespace safelog {

// 日志级别。使用 scoped enum 避免隐式转换为 int，保证类型安全。
enum class LogLevel {
  kTrace = 0,
  kDebug,
  kInfo,
  kWarn,
  kError,
  kFatal,
};

// 将级别转换为固定大写字符串（如 "INFO"）；未知值返回 "UNKNOWN"。
std::string ToString(LogLevel level);

// 将字符串（忽略大小写）解析为级别；无法识别时返回 std::nullopt。
std::optional<LogLevel> LogLevelFromString(std::string_view text);

}  // namespace safelog
