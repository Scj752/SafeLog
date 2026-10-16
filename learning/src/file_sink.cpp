// 【学习项目】关卡 2 练习文件：占位实现。请根据 TODO 完成，然后运行
//   ctest -R learn_stage2_format_file_test
// 对照参考实现：reference/src/file_sink.cpp

#include "safelog/file_sink.h"

namespace safelog {

FileSink::FileSink(const std::filesystem::path& file_path,
                   std::size_t max_bytes,
                   std::size_t max_files)
    : file_path_(file_path),
      max_bytes_(max_bytes),
      max_files_(max_files == 0 ? 1 : max_files) {
  // TODO: 1) 用 std::filesystem::create_directories 创建父目录（配 std::error_code）
  //       2) 若文件已存在，把 current_size_ 初始化为现有文件大小
  //       3) 以 std::ios::binary | std::ios::app 打开 stream_，失败抛 std::runtime_error
}

void FileSink::Write(const LogRecord& record, const std::string& formatted) {
  // TODO: 持锁 -> RotateIfNeeded(formatted.size() + 1)
  //       -> 写入 formatted 与换行 -> current_size_ 累加（含换行的 1 字节）
  (void)record;
  (void)formatted;
}

void FileSink::Flush() {
  // TODO: 持锁刷新 stream_
}

void FileSink::RotateIfNeeded(std::size_t incoming_bytes) {
  // TODO: 仅在 max_bytes_ > 0、current_size_ > 0 且将超限时滚动：
  //       1) 关闭流
  //       2) 历史文件依次后移：.1->.2 ... .(max_files_-1)->.max_files_（先删目标再 rename）
  //       3) 当前文件改名为 .1，current_size_ 归零
  //       4) 以 trunc 模式重新打开，失败抛 std::runtime_error
  //       文件操作统一使用带 std::error_code 的重载，避免异常
  (void)incoming_bytes;
}

}  // namespace safelog
