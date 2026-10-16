#pragma once

// blocking_queue.h —— 有界阻塞队列（并发层，模板，头文件实现）。
// 【学习项目】本文件为练习框架：所有方法为占位实现，请根据 TODO 提示完成。
// 完成标准：learn_stage3_async_test 中 BlockingQueue_* 用例全部通过。
//
// 实现提示：
//   - 所有共享状态（queue_/shutdown_）访问必须持有 mutex_。
//   - cv 等待一律使用带谓词的 wait(lock, pred) 重载，防虚假唤醒。
//   - 关闭语义：Shutdown 后 Push/TryPush 返回 false；WaitPop 取完剩余元素后返回 false。

#include <condition_variable>
#include <cstddef>
#include <deque>
#include <mutex>
#include <utility>

namespace safelog {

template <typename T>
class BlockingQueue {
public:
  explicit BlockingQueue(std::size_t capacity)
      : capacity_(capacity == 0 ? 1 : capacity) {}

  BlockingQueue(const BlockingQueue&) = delete;
  BlockingQueue& operator=(const BlockingQueue&) = delete;

  // 非阻塞入队：已满或已关闭时返回 false，否则入队并唤醒一个消费者。
  bool TryPush(T value) {
    // TODO: 加锁检查 shutdown_ 与容量；满足条件时 push_back(std::move(value)) 并
    //       notify_one(cv_not_empty_)，返回 true；否则返回 false
    (void)value;
    return false;
  }

  // 阻塞入队：队列满时在 cv_not_full_ 上等待空位（背压）；关闭后返回 false。
  bool Push(T value) {
    // TODO: unique_lock + cv_not_full_.wait(lock, 谓词: shutdown_ || 未满)；
    //       若已关闭返回 false；否则入队、notify_one(cv_not_empty_)、返回 true
    (void)value;
    return false;
  }

  // 阻塞出队：在 cv_not_empty_ 上等待直到有元素或关闭；
  // 取到元素返回 true（记得 notify cv_not_full_，空队列时 notify cv_empty_）；
  // 已关闭且无剩余元素时返回 false。
  bool WaitPop(T& out) {
    // TODO: unique_lock + cv_not_empty_.wait(lock, 谓词: shutdown_ || 非空)；
    //       队列仍为空则返回 false；否则移动队首到 out、pop_front、
    //       notify_one(cv_not_full_)、若空了 notify_all(cv_empty_)、返回 true
    (void)out;
    return false;
  }

  // 关闭队列并唤醒所有等待线程（三个条件变量都要 notify_all）。幂等。
  void Shutdown() {
    // TODO: 持锁置 shutdown_ = true，然后对三个条件变量 notify_all
  }

  bool IsShutdown() const {
    // TODO: 持锁返回 shutdown_
    return false;
  }

  std::size_t Size() const {
    // TODO: 持锁返回 queue_.size()
    return 0;
  }

  // 等待队列变为空（Flush 语义）。
  void WaitEmpty() {
    // TODO: unique_lock + cv_empty_.wait(lock, 谓词: queue_.empty())
  }

private:
  const std::size_t capacity_;
  std::deque<T> queue_;
  mutable std::mutex mutex_;
  std::condition_variable cv_not_empty_;
  std::condition_variable cv_not_full_;
  std::condition_variable cv_empty_;
  bool shutdown_ = false;
};

}  // namespace safelog
