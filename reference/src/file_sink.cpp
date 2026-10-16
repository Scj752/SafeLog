#include "safelog/file_sink.h"

#include <stdexcept>
#include <system_error>

namespace safelog {

FileSink::FileSink(const std::filesystem::path& file_path,
                   std::size_t max_bytes,
                   std::size_t max_files)
    : file_path_(file_path),
      max_bytes_(max_bytes),
      max_files_(max_files == 0 ? 1 : max_files) {
  std::error_code ec;
  const auto parent = file_path_.parent_path();
  if (!parent.empty()) {
    std::filesystem::create_directories(parent, ec);  // 失败不致命，open 会再报错
  }
  if (std::filesystem::exists(file_path_, ec)) {
    current_size_ = static_cast<std::size_t>(std::filesystem::file_size(file_path_, ec));
  }
  stream_.open(file_path_, std::ios::binary | std::ios::app);
  if (!stream_) {
    throw std::runtime_error("FileSink: cannot open file: " + file_path_.string());
  }
}

void FileSink::Write(const LogRecord& /*record*/, const std::string& formatted) {
  std::lock_guard<std::mutex> lock(mutex_);
  RotateIfNeeded(formatted.size() + 1);  // +1 计入换行符
  stream_ << formatted << '\n';
  current_size_ += formatted.size() + 1;
}

void FileSink::Flush() {
  std::lock_guard<std::mutex> lock(mutex_);
  stream_.flush();
}

void FileSink::RotateIfNeeded(std::size_t incoming_bytes) {
  if (max_bytes_ == 0 || current_size_ == 0) return;
  if (current_size_ + incoming_bytes <= max_bytes_) return;

  stream_.close();
  std::error_code ec;
  // 历史文件依次后移：app.log.(n-1) -> app.log.n，最老的被覆盖删除。
  for (std::size_t i = max_files_ - 1; i >= 1; --i) {
    const auto src = file_path_.string() + "." + std::to_string(i);
    const auto dst = file_path_.string() + "." + std::to_string(i + 1);
    if (std::filesystem::exists(src, ec)) {
      std::filesystem::remove(dst, ec);
      std::filesystem::rename(src, dst, ec);
    }
  }
  const auto first = file_path_.string() + ".1";
  std::filesystem::remove(first, ec);
  std::filesystem::rename(file_path_, first, ec);
  current_size_ = 0;
  stream_.open(file_path_, std::ios::binary | std::ios::trunc);
  if (!stream_) {
    throw std::runtime_error("FileSink: cannot reopen file: " + file_path_.string());
  }
}

}  // namespace safelog
