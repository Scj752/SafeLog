// 【学习项目】关卡 1 练习文件：占位实现。请根据 TODO 完成，然后运行
//   ctest -R learn_stage1_core_test
// 对照参考实现：reference/src/log_level.cpp（建议先自己实现，再对比）

#include "safelog/log_level.h"

namespace safelog {

std::string ToString(LogLevel level) {
  // TODO: 使用 switch 将每个枚举值映射为对应大写字符串
  //       （TRACE/DEBUG/INFO/WARN/ERROR/FATAL），switch 之外兜底返回 "UNKNOWN"
  (void)level;
  return {};
}

std::optional<LogLevel> LogLevelFromString(std::string_view text) {
  // TODO: 将 text 转为大写后与六个级别名逐一比较，匹配则返回对应枚举；
  //       都不匹配返回 std::nullopt
  //       （提示：std::string upper(text); std::transform + std::toupper）
  (void)text;
  return std::nullopt;
}

}  // namespace safelog
