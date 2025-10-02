cmake_minimum_required(VERSION 3.18)
project(expr)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

# 查找已安装的 LLVM 库，REQUIRED 表示如果找不到则报错，CONFIG 表示优先使用 LLVM 的配置文件（如 LLVMConfig.cmake）。
find_package(LLVM REQUIRED CONFIG)

list(APPEND CMAKE_MODULE_PATH ${LLVM_CMAKE_DIR})

#包含 LLVM 提供的 AddLLVM 模块。
include(AddLLVM)
include_directories("${LLVM_BINARY_DIR}/include" "${LLVM_INCLUDE_DIR}")
add_definitions(${LLVM_DEFINITIONS})

# 检查 LLVM 是否启用 RTTI（运行时类型信息）。如果禁用（LLVM_ENABLE_RTTI 为 FALSE），则添加 -fno-rtti 编译标志。
if (NOT ${LLVM_ENABLE_RTTI})
    # For non-MSVC compilers
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()


# 指定要链接的 LLVM 组件：Support（基础工具类）和 Core（核心 IR 数据结构）。
set(LLVM_LINK_COMPONENTS Support Core)

# 使用 LLVM 的 add_llvm_executable 宏创建可执行文件，名称为 llvm_test（来自 project(llvm_test)），基于源文件 main.cc。
add_llvm_executable(${PROJECT_NAME} main.cc lexer.cc parser.cc printVisitor.cc codegen.cc type.cc)