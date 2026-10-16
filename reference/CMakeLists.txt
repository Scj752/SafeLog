# 参考实现：完整可运行的 SafeLog 静态库
file(GLOB SAFELOG_REF_SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_library(safelog_ref STATIC ${SAFELOG_REF_SOURCES})
target_include_directories(safelog_ref PUBLIC include)

find_package(Threads REQUIRED)
target_link_libraries(safelog_ref PUBLIC Threads::Threads)
