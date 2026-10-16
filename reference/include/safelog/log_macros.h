#pragma once

// log_macros.h —— 日志宏封装（门面层）。
// 学习点：宏的 do-while(0) 惯用法、__FILE__/__LINE__ 源位置捕获、
//         “先过滤再求值”避免无意义的开销。
//
// 使用：
//   SAFELOG_INFO(logger, "hello");        // 显式指定 logger
//   SAFELOG_I("hello");                    // 使用注册表中的默认 logger

#include "safelog/log_level.h"
#include "safelog/logger.h"
#include "safelog/logger_factory.h"

#define SAFELOG_LOG(logger_ptr, level, message)                          \
  do {                                                                   \
    const auto& _safelog_logger = (logger_ptr);                          \
    if (_safelog_logger && _safelog_logger->ShouldLog(level)) {          \
      _safelog_logger->Log(level, (message), __FILE__, __LINE__);        \
    }                                                                    \
  } while (0)

#define SAFELOG_TRACE(logger, msg) SAFELOG_LOG(logger, ::safelog::LogLevel::kTrace, msg)
#define SAFELOG_DEBUG(logger, msg) SAFELOG_LOG(logger, ::safelog::LogLevel::kDebug, msg)
#define SAFELOG_INFO(logger, msg)  SAFELOG_LOG(logger, ::safelog::LogLevel::kInfo, msg)
#define SAFELOG_WARN(logger, msg)  SAFELOG_LOG(logger, ::safelog::LogLevel::kWarn, msg)
#define SAFELOG_ERROR(logger, msg) SAFELOG_LOG(logger, ::safelog::LogLevel::kError, msg)
#define SAFELOG_FATAL(logger, msg) SAFELOG_LOG(logger, ::safelog::LogLevel::kFatal, msg)

#define SAFELOG_DEFAULT() ::safelog::LoggerRegistry::Instance().Default()

#define SAFELOG_T(msg) SAFELOG_LOG(SAFELOG_DEFAULT(), ::safelog::LogLevel::kTrace, msg)
#define SAFELOG_D(msg) SAFELOG_LOG(SAFELOG_DEFAULT(), ::safelog::LogLevel::kDebug, msg)
#define SAFELOG_I(msg) SAFELOG_LOG(SAFELOG_DEFAULT(), ::safelog::LogLevel::kInfo, msg)
#define SAFELOG_W(msg) SAFELOG_LOG(SAFELOG_DEFAULT(), ::safelog::LogLevel::kWarn, msg)
#define SAFELOG_E(msg) SAFELOG_LOG(SAFELOG_DEFAULT(), ::safelog::LogLevel::kError, msg)
#define SAFELOG_F(msg) SAFELOG_LOG(SAFELOG_DEFAULT(), ::safelog::LogLevel::kFatal, msg)
