# 学习项目：与参考实现同接口的练习框架（src 中为 TODO 占位实现）
file(GLOB SAFELOG_LEARN_SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_library(safelog_learn STATIC ${SAFELOG_LEARN_SOURCES})
target_include_directories(safelog_learn PUBLIC include)

find_package(Threads REQUIRED)
target_link_libraries(safelog_learn PUBLIC Threads::Threads)
