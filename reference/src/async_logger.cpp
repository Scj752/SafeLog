#include "safelog/async_logger.h"

#include <exception>
#include <iostream>
#include <utility>

namespace safelog {

AsyncLogger::AsyncLogger(std::string name,
                         LogLevel level,
                         std::shared_ptr<IFormatter> formatter,
                         std::size_t queue_capacity)
    : Logger(std::move(name), level, std::move(formatter)),
      queue_(queue_capacity),
      worker_([this] { WorkerMain(); }) {}

AsyncLogger::~AsyncLogger() {
  queue_.Shutdown();                  // 1. 拒绝新写入并唤醒工作线程
  if (worker_.joinable()) {
    worker_.join();                   // 2. 等待队列排空（WaitPop 取完才返回 false）
  }
  Logger::Flush();                    // 3. 刷新所有 sink
}

void AsyncLogger::Dispatch(const LogRecord& record) {
  queue_.Push(record);  // 队列满时阻塞（背压），保证不丢日志
}

void AsyncLogger::Flush() {
  queue_.WaitEmpty();                          // 等队列排空
  std::lock_guard<std::mutex> lock(write_mutex_);  // 等“在途写入”完成
  Logger::Flush();
}

void AsyncLogger::WorkerMain() {
  LogRecord record;
  while (queue_.WaitPop(record)) {
    try {
      std::lock_guard<std::mutex> lock(write_mutex_);
      WriteToSinks(record);
    } catch (const std::exception& e) {
      // 隔离异常，保证工作线程不退出（否则后续日志全部静默丢失）。
      std::cerr << "AsyncLogger: write failed: " << e.what() << '\n';
    } catch (...) {
      std::cerr << "AsyncLogger: write failed with unknown error\n";
    }
  }
}

}  // namespace safelog
