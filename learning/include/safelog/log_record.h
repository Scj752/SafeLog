#pragma once

// log_record.h —— 单条日志记录的值类型（基础设施层）。
// 学习点：值语义、默认成员初始化器、跨线程传递的数据设计。
//
// LogRecord 是可拷贝、可移动的纯数据对象：
// 调用线程构造它时捕获当前时间与线程 id，之后可安全地跨线程传递
// （异步日志场景：入队后由工作线程消费，数据与调用线程生命周期无关）。

#include <chrono>
#include <string>
#include <thread>

#include "safelog/log_level.h"

namespace safelog {

struct LogRecord {
  LogLevel level = LogLevel::kInfo;
  std::string message;
  // 构造时刻自动捕获时间戳与线程 id（默认成员初始化器）。
  std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now();
  std::thread::id thread_id = std::this_thread::get_id();
  std::string file;  // 来源文件（由宏填充，可为空）
  int line = 0;      // 来源行号（0 表示未提供）
};

}  // namespace safelog
