#pragma once

// blocking_queue.h —— 有界阻塞队列（并发层，模板，头文件实现）。
// 学习点：互斥锁 + 条件变量的经典生产者-消费者模型、虚假唤醒防护、
//         有界队列背压、优雅关闭（Shutdown 唤醒所有等待线程）。
//
// 线程安全约定：
//   - 支持多生产者多消费者。
//   - Push/TryPush 在队列满时分别表现为“阻塞等待”/“立即失败”。
//   - Shutdown 后：Push/TryPush 返回 false；WaitPop 取完剩余元素后返回 false。

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

  // 非阻塞入队：已满或已关闭时返回 false。
  bool TryPush(T value) {
    {
      std::lock_guard<std::mutex> lock(mutex_);
      if (shutdown_ || queue_.size() >= capacity_) return false;
      queue_.push_back(std::move(value));
    }
    cv_not_empty_.notify_one();
    return true;
  }

  // 阻塞入队：队列满时等待空位（背压）；关闭后返回 false。
  bool Push(T value) {
    {
      std::unique_lock<std::mutex> lock(mutex_);
      cv_not_full_.wait(lock, [this] { return shutdown_ || queue_.size() < capacity_; });
      if (shutdown_) return false;
      queue_.push_back(std::move(value));
    }
    cv_not_empty_.notify_one();
    return true;
  }

  // 阻塞出队：取到元素返回 true；已关闭且无剩余元素时返回 false。
  bool WaitPop(T& out) {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_not_empty_.wait(lock, [this] { return shutdown_ || !queue_.empty(); });
    if (queue_.empty()) return false;  // 关闭且已排空
    out = std::move(queue_.front());
    queue_.pop_front();
    cv_not_full_.notify_one();          // 持锁通知，避免丢失唤醒
    if (queue_.empty()) cv_empty_.notify_all();
    return true;
  }

  // 关闭队列并唤醒所有等待线程。幂等。
  void Shutdown() {
    {
      std::lock_guard<std::mutex> lock(mutex_);
      shutdown_ = true;
    }
    cv_not_empty_.notify_all();
    cv_not_full_.notify_all();
    cv_empty_.notify_all();
  }

  bool IsShutdown() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return shutdown_;
  }

  std::size_t Size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
  }

  // 等待队列变为空（用于 Flush 语义；多生产者并发写期间仅表示“瞬时为空”）。
  void WaitEmpty() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_empty_.wait(lock, [this] { return queue_.empty(); });
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
