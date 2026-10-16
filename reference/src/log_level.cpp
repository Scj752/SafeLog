#include "safelog/log_level.h"

#include <algorithm>
#include <cctype>

namespace safelog {

std::string ToString(LogLevel level) {
  switch (level) {
    case LogLevel::kTrace: return "TRACE";
    case LogLevel::kDebug: return "DEBUG";
    case LogLevel::kInfo:  return "INFO";
    case LogLevel::kWarn:  return "WARN";
    case LogLevel::kError: return "ERROR";
    case LogLevel::kFatal: return "FATAL";
  }
  return "UNKNOWN";
}

std::optional<LogLevel> LogLevelFromString(std::string_view text) {
  std::string upper(text);
  std::transform(upper.begin(), upper.end(), upper.begin(),
                 [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
  if (upper == "TRACE") return LogLevel::kTrace;
  if (upper == "DEBUG") return LogLevel::kDebug;
  if (upper == "INFO")  return LogLevel::kInfo;
  if (upper == "WARN")  return LogLevel::kWarn;
  if (upper == "ERROR") return LogLevel::kError;
  if (upper == "FATAL") return LogLevel::kFatal;
  return std::nullopt;
}

}  // namespace safelog
