#pragma once

// async_logger.h —— 异步日志器（并发层）。
// 学习点：继承重写模板方法钩子、工作线程的 RAII 启停、跨线程异常隔离。
//
// 设计：AsyncLogger 继承 Logger，仅把 Dispatch 从“立即写 sink”改为
// “投递到阻塞队列”；工作线程弹出记录后调用基类 WriteToSinks 落盘。
// 级别过滤、格式化、sink 管理全部复用基类，零重复代码。

#include <cstddef>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "safelog/blocking_queue.h"
#include "safelog/logger.h"

namespace safelog {

class AsyncLogger final : public Logger {
public:
  AsyncLogger(std::string name,
              LogLevel level = LogLevel::kInfo,
              std::shared_ptr<IFormatter> formatter = nullptr,
              std::size_t queue_capacity = 8192);
  ~AsyncLogger() override;  // 关闭队列 -> 等待排空并 join 线程 -> 刷新 sink

  AsyncLogger(const AsyncLogger&) = delete;
  AsyncLogger& operator=(const AsyncLogger&) = delete;

  // 等待已入队日志全部写完并刷新 sink。
  // 注意：多线程并发写期间调用仅保证“调用时刻之前入队”的日志完成。
  void Flush() override;

protected:
  void Dispatch(const LogRecord& record) override;  // 入队（满则阻塞，背压）

private:
  void WorkerMain();

  BlockingQueue<LogRecord> queue_;
  std::mutex write_mutex_;  // 与 Flush 配合，保证“在途写入”完成
  std::thread worker_;
};

}  // namespace safelog
