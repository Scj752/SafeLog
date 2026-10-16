// stage3_async_test.cpp —— 关卡 3：异步日志。
// 覆盖：BlockingQueue 的 FIFO/背压/关闭语义，AsyncLogger 的可靠投递与有序性。

#include <atomic>
#include <memory>
#include <string>
#include <thread>

#include "safelog/async_logger.h"
#include "safelog/blocking_queue.h"
#include "test_framework.h"
#include "test_sink.h"

using namespace safelog;

TEST_CASE(BlockingQueue_FifoAndBackpressure) {
  BlockingQueue<int> queue(2);
  CHECK_TRUE(queue.Push(1));
  CHECK_TRUE(queue.Push(2));
  CHECK_FALSE(queue.TryPush(3));  // 已满：非阻塞入队失败
  CHECK_EQ(queue.Size(), 2u);

  int out = 0;
  CHECK_TRUE(queue.WaitPop(out));
  CHECK_EQ(out, 1);
  CHECK_TRUE(queue.WaitPop(out));
  CHECK_EQ(out, 2);
  CHECK_EQ(queue.Size(), 0u);
}

TEST_CASE(BlockingQueue_ShutdownUnblocksConsumer) {
  BlockingQueue<int> queue(1);
  std::atomic<bool> popped{true};
  std::thread consumer([&] {
    int out = 0;
    popped = queue.WaitPop(out);  // 空队列上阻塞，Shutdown 后应返回 false
  });
  queue.Shutdown();
  consumer.join();
  CHECK_FALSE(popped.load());
}

TEST_CASE(BlockingQueue_PushFailsAfterShutdown) {
  BlockingQueue<int> queue(1);
  queue.Shutdown();
  CHECK_FALSE(queue.Push(1));
  CHECK_FALSE(queue.TryPush(1));
  CHECK_TRUE(queue.IsShutdown());
}

TEST_CASE(AsyncLogger_DeliversAllMessages) {
  auto sink = std::make_shared<TestSink>();
  constexpr std::size_t kCount = 500;
  {
    AsyncLogger logger("async", LogLevel::kTrace, nullptr, 1024);
    logger.AddSink(sink);
    for (std::size_t i = 0; i < kCount; ++i) {
      logger.Info("msg-" + std::to_string(i));
    }
    logger.Flush();
    CHECK_EQ(sink->Count(), kCount);
  }  // 析构后剩余日志也必须落盘
  CHECK_EQ(sink->Count(), kCount);
}

TEST_CASE(AsyncLogger_PreservesOrderSingleProducer) {
  auto sink = std::make_shared<TestSink>();
  AsyncLogger logger("order", LogLevel::kTrace);
  logger.AddSink(sink);
  for (int i = 0; i < 100; ++i) {
    logger.Info("m" + std::to_string(i));
  }
  logger.Flush();
  CHECK_EQ(sink->Count(), 100u);
  for (int i = 0; i < 100; ++i) {
    CHECK_EQ(sink->At(static_cast<std::size_t>(i)).message,
             "m" + std::to_string(i));
  }
}

TEST_CASE(AsyncLogger_FiltersBeforeEnqueue) {
  auto sink = std::make_shared<TestSink>();
  AsyncLogger logger("filter", LogLevel::kWarn);
  logger.AddSink(sink);
  logger.Info("dropped");
  logger.Warn("kept");
  logger.Flush();
  CHECK_EQ(sink->Count(), 1u);
  CHECK_EQ(sink->At(0).message, std::string("kept"));
}

MINI_TEST_MAIN()
