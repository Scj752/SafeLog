# 关卡测试：同一套用例构建两次 ——
#   ref_*    链接参考实现，必须全部通过（“标准答案”自证）
#   learn_*  链接学习项目，初始全红，完成 TODO 后逐个转绿（练习关卡）
set(STAGE_TESTS
    stage1_core_test
    stage2_format_file_test
    stage3_async_test
    stage4_factory_test)

foreach(test_name IN LISTS STAGE_TESTS)
  add_executable(ref_${test_name} ${test_name}.cpp)
  target_include_directories(ref_${test_name} PRIVATE common)
  target_link_libraries(ref_${test_name} PRIVATE safelog_ref)
  add_test(NAME ref_${test_name} COMMAND ref_${test_name})

  add_executable(learn_${test_name} ${test_name}.cpp)
  target_include_directories(learn_${test_name} PRIVATE common)
  target_link_libraries(learn_${test_name} PRIVATE safelog_learn)
  add_test(NAME learn_${test_name} COMMAND learn_${test_name})
endforeach()
