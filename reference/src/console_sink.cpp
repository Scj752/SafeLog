#include "safelog/console_sink.h"

#include <iostream>

namespace safelog {

void ConsoleSink::Write(const LogRecord& record, const std::string& formatted) {
  std::lock_guard<std::mutex> lock(mutex_);
  std::ostream& os = (record.level >= LogLevel::kWarn) ? std::cerr : std::cout;
  os << formatted << '\n';
}

void ConsoleSink::Flush() {
  std::lock_guard<std::mutex> lock(mutex_);
  std::cout.flush();
  std::cerr.flush();
}

}  // namespace safelog
