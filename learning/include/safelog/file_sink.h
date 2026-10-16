#pragma once

// file_sink.h —— 文件输出端，支持按大小滚动（实现层）。
// 学习点：RAII 管理文件流、std::filesystem + std::error_code 的无异常错误处理。

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <mutex>

#include "safelog/sink.h"

namespace safelog {

// 写入 file_path；max_bytes > 0 时超出大小即滚动：
//   app.log -> app.log.1 -> app.log.2 ... 最多保留 max_files 个历史文件。
class FileSink final : public ISink {
public:
  explicit FileSink(const std::filesystem::path& file_path,
                    std::size_t max_bytes = 0,
                    std::size_t max_files = 3);
  ~FileSink() override = default;  // ofstream 由 RAII 自动关闭

  FileSink(const FileSink&) = delete;
  FileSink& operator=(const FileSink&) = delete;

  void Write(const LogRecord& record, const std::string& formatted) override;
  void Flush() override;

  const std::filesystem::path& file_path() const noexcept { return file_path_; }

private:
  // 调用前必须持有 mutex_。
  void RotateIfNeeded(std::size_t incoming_bytes);

  std::filesystem::path file_path_;
  std::size_t max_bytes_;
  std::size_t max_files_;
  std::size_t current_size_ = 0;
  std::ofstream stream_;
  std::mutex mutex_;
};

}  // namespace safelog
