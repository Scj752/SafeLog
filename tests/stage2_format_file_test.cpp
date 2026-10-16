// stage2_format_file_test.cpp —— 关卡 2：格式化与文件输出。
// 覆盖：PatternFormatter 占位符、FileSink 写入、按大小滚动。

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include "safelog/file_sink.h"
#include "safelog/formatter.h"
#include "test_framework.h"

using namespace safelog;
namespace fs = std::filesystem;

namespace {

std::string ReadAll(const fs::path& path) {
  std::ifstream in(path, std::ios::binary);
  std::ostringstream oss;
  oss << in.rdbuf();
  return oss.str();
}

fs::path TempDir(const std::string& name) {
  fs::path dir = fs::temp_directory_path() / ("safelog_" + name);
  std::error_code ec;
  fs::remove_all(dir, ec);
  fs::create_directories(dir);
  return dir;
}

LogRecord MakeRecord(LogLevel level, const std::string& msg, const std::string& file,
                     int line) {
  LogRecord record;
  record.level = level;
  record.message = msg;
  record.file = file;
  record.line = line;
  return record;
}

}  // namespace

TEST_CASE(PatternFormatter_FormatsFixedTokens) {
  PatternFormatter formatter("[%l][%s:%#] %m");
  const auto record = MakeRecord(LogLevel::kInfo, "hello", "app.cpp", 7);
  CHECK_EQ(formatter.Format(record), std::string("[INFO][app.cpp:7] hello"));
}

TEST_CASE(PatternFormatter_TimeTokensShape) {
  PatternFormatter formatter("%d %t");
  const std::string out = formatter.Format(MakeRecord(LogLevel::kInfo, "x", "", 0));
  CHECK_EQ(out.size(), 19u);  // "YYYY-MM-DD HH:MM:SS"
  CHECK_EQ(out[4], '-');
  CHECK_EQ(out[7], '-');
  CHECK_EQ(out[10], ' ');
  CHECK_EQ(out[13], ':');
}

TEST_CASE(PatternFormatter_EscapesPercent) {
  PatternFormatter formatter("100%% %m");
  CHECK_EQ(formatter.Format(MakeRecord(LogLevel::kInfo, "done", "", 0)),
           std::string("100% done"));
}

TEST_CASE(FileSink_WritesFormattedLine) {
  const fs::path dir = TempDir("write");
  const fs::path file = dir / "app.log";
  {
    FileSink sink(file);
    const auto record = MakeRecord(LogLevel::kInfo, "hello file", "", 0);
    sink.Write(record, "[INFO] hello file");
    sink.Flush();
  }
  const std::string content = ReadAll(file);
  CHECK_TRUE(content.find("[INFO] hello file") != std::string::npos);
  std::error_code ec;
  fs::remove_all(dir, ec);
}

TEST_CASE(FileSink_RotatesWhenFull) {
  const fs::path dir = TempDir("rotate");
  const fs::path file = dir / "rotate.log";
  {
    FileSink sink(file, /*max_bytes=*/64, /*max_files=*/2);
    const auto record = MakeRecord(LogLevel::kInfo, "x", "", 0);
    const std::string line(40, 'x');
    for (int i = 0; i < 6; ++i) {
      sink.Write(record, line);
    }
    sink.Flush();
  }
  CHECK_TRUE(fs::exists(file.string() + ".1"));  // 产生了滚动历史文件
  CHECK_TRUE(fs::file_size(file) <= 64 + 41);    // 当前文件不超过阈值+单条大小
  std::error_code ec;
  fs::remove_all(dir, ec);
}

MINI_TEST_MAIN()
