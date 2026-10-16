# SafeLog —— 线程安全异步日志库（C++ 学习项目）

一个零依赖、C++17 的日志库，包含**参考实现**与**关卡制学习练习框架**两部分。

## 快速开始

```powershell
cmake -S . -B build -G "MinGW Makefiles"   # MSVC 用户省略 -G
cmake --build build -j
ctest --test-dir build -R "ref_" --output-on-failure   # 参考实现测试：4/4 通过
.\build\examples\safelog_demo.exe                       # 运行演示
```

## 30 秒上手（API 预览）

```cpp
#include "safelog/log_macros.h"
#include "safelog/logger_factory.h"

using namespace safelog;

LoggerConfig config;
config.name = "app";
config.async = true;                 // 异步模式
config.file_path = "logs/app.log";   // 同时写文件
config.max_bytes = 1024 * 1024;      // 1MB 滚动

auto logger = LoggerFactory::Create(config);
LoggerRegistry::Instance().SetDefault(logger);

SAFELOG_INFO(logger, "hello world");   // 显式 logger，自动携带源文件/行号
SAFELOG_I("via default logger");       // 使用注册表中的默认 logger
logger->Flush();
```

## 仓库结构

| 目录 | 说明 |
| --- | --- |
| `reference/` | 完整可运行的参考实现（库目标 `safelog_ref`） |
| `learning/` | 练习框架：同接口 + `// TODO:` 占位（库目标 `safelog_learn`） |
| `tests/` | 关卡测试：同一套用例链接两套实现（`ref_*` 常绿，`learn_*` 红→绿） |
| `docs/` | 总设计文档、4 个阶段文档、学习引导 |
| `examples/` | 演示程序 |

## 文档

- 设计与计划：[docs/00-总设计文档.md](docs/00-总设计文档.md)
- 练习路径：[docs/学习引导.md](docs/学习引导.md)
- 阶段文档：[阶段一](docs/01-阶段一-同步日志核心.md) · [阶段二](docs/02-阶段二-格式化与文件输出.md) · [阶段三](docs/03-阶段三-异步日志.md) · [阶段四](docs/04-阶段四-工厂与宏封装.md)
