// 【学习项目】关卡 3 练习文件：占位实现。请根据 TODO 完成，然后运行
//   ctest -R learn_stage3_async_test
// 前置：需先完成 learning/include/safelog/blocking_queue.h 中的 TODO。
// 对照参考实现：reference/src/async_logger.cpp
//
// 注意：完成本文件前请先完成关卡 1（logger.cpp），否则基类逻辑不可用。

#include "safelog/async_logger.h"

#include <utility>

namespace safelog {

AsyncLogger::AsyncLogger(std::string name,
                         LogLevel level,
                         std::shared_ptr<IFormatter> formatter,
                         std::size_t queue_capacity)
    : Logger(std::move(name), level, std::move(formatter)),
      queue_(queue_capacity) {
  // TODO: 在成员初始化之后启动工作线程：worker_ = std::thread([this] { WorkerMain(); });
  //       （可改为在初始化列表中初始化 worker_，思考：为什么此时使用 this 是安全的？）
}

AsyncLogger::~AsyncLogger() {
  // TODO: RAII 停止序列（顺序很重要，想清楚每一步为什么）：
  //       1) queue_.Shutdown()  —— 拒绝新写入、唤醒工作线程
  //       2) join worker_（先判断 joinable）—— 等待队列排空
  //       3) Logger::Flush()    —— 刷新所有 sink
}

void AsyncLogger::Dispatch(const LogRecord& record) {
  // TODO: queue_.Push(record)（满则阻塞形成背压，不丢日志）
  (void)record;
}

void AsyncLogger::Flush() {
  // TODO: 1) queue_.WaitEmpty()            —— 等队列排空
  //       2) 加锁 write_mutex_             —— 等“在途写入”完成
  //       3) Logger::Flush()
}

void AsyncLogger::WorkerMain() {
  // TODO: 循环 WaitPop 直到返回 false：
  //         LogRecord record;
  //         while (queue_.WaitPop(record)) {
  //           try { 持 write_mutex_ 锁调用 WriteToSinks(record); }
  //           catch (const std::exception& e) { 写 std::cerr，继续循环； }
  //           catch (...) { 同上； }
  //         }
  //       思考：为什么必须 catch 住异常？（提示：线程退出后队列里的日志会怎样？）
}

}  // namespace safelog
