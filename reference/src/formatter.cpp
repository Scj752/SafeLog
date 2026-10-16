#include "safelog/formatter.h"

#include <chrono>
#include <cstdio>
#include <ctime>
#include <sstream>
#include <utility>

#include "safelog/log_level.h"

namespace safelog {
namespace {

// 线程安全的本地时间转换（避免 std::localtime 的静态缓冲竞争）。
std::tm LocalTime(std::time_t t) {
  std::tm tm{};
#ifdef _WIN32
  localtime_s(&tm, &t);
#else
  localtime_r(&t, &tm);
#endif
  return tm;
}

std::string FormatTime(const std::chrono::system_clock::time_point& tp) {
  const std::time_t t = std::chrono::system_clock::to_time_t(tp);
  const std::tm tm = LocalTime(t);
  char buf[16];
  std::snprintf(buf, sizeof(buf), "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
  return buf;
}

std::string FormatDate(const std::chrono::system_clock::time_point& tp) {
  const std::time_t t = std::chrono::system_clock::to_time_t(tp);
  const std::tm tm = LocalTime(t);
  char buf[16];
  std::snprintf(buf, sizeof(buf), "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1,
                tm.tm_mday);
  return buf;
}

}  // namespace

PatternFormatter::PatternFormatter(std::string pattern) : pattern_(std::move(pattern)) {}

std::string PatternFormatter::Format(const LogRecord& record) const {
  std::string out;
  out.reserve(pattern_.size() + record.message.size() + 32);
  for (std::size_t i = 0; i < pattern_.size(); ++i) {
    const char c = pattern_[i];
    if (c != '%' || i + 1 >= pattern_.size()) {
      out.push_back(c);
      continue;
    }
    const char token = pattern_[++i];
    switch (token) {
      case 'l': out += ToString(record.level); break;
      case 'm': out += record.message; break;
      case 't': out += FormatTime(record.timestamp); break;
      case 'd': out += FormatDate(record.timestamp); break;
      case 'T': {
        std::ostringstream oss;
        oss << record.thread_id;
        out += oss.str();
        break;
      }
      case 's': out += record.file; break;
      case '#': out += std::to_string(record.line); break;
      case '%': out.push_back('%'); break;
      default:  // 未知占位符原样保留，便于发现模式串笔误
        out.push_back('%');
        out.push_back(token);
        break;
    }
  }
  return out;
}

}  // namespace safelog
