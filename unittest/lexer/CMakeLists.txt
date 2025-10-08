enable_testing()

add_executable(
  # 构建测试命令
  lexer_test
  lexer_test.cc

  # 依赖的源文件
  ../../lexer.cc
  ../../type.cc 
  ../../diag_engine.cc
)

llvm_map_components_to_libnames(llvm_all Support Core)

#message(STATUS "wuxiliang@@@@@@@@@@@@@iiicp: ${llvm_all}")

# 链接llvm的库
target_link_libraries(
  lexer_test
  GTest::gtest_main
  ${llvm_all}
)

include(GoogleTest)
gtest_discover_tests(lexer_test)