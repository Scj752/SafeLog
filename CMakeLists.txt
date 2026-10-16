cmake_minimum_required(VERSION 3.16)
project(SafeLog LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
  add_compile_options(/W4 /permissive- /utf-8)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

enable_testing()

add_subdirectory(reference)  # 参考实现：完整可运行
add_subdirectory(learning)   # 学习项目：TODO 练习框架
add_subdirectory(tests)      # 关卡测试：同一套用例分别链接两套实现
add_subdirectory(examples)   # 演示程序（链接参考实现）
