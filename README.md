基于LLVM实现C语言编译器，同时实现RISCV32i后端

# LLVM-C-Compiler

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

## 项目概述

LLVM-C-Compiler 是一个基于 LLVM 框架实现的 C 语言编译器项目，同时集成了 RISCV32i 后端支持。该项目旨在提供一个简洁、高效的 C 语言前端解析器和代码生成器，利用 LLVM 的强大 IR（Intermediate Representation）基础设施，实现从 C 源代码到 RISCV32i 机器码的完整编译流程。

### 核心特性
- **C 语言前端**：支持 C99 标准的基本语法解析，包括变量声明、表达式、控制流等。
- **LLVM 集成**：使用 Clang/LLVM 作为后端，进行优化和代码生成。
- **RISCV32i 后端**：针对 RISCV32i 指令集架构（RV32I 基础指令集）生成汇编和二进制代码，支持交叉编译。
- **模块化设计**：易于扩展，支持自定义 Pass 和优化。

当前项目处于开发早期阶段，欢迎贡献代码、测试用例或优化建议！

## 环境要求

项目开发和构建环境基于 RHEL 9.5（或其他兼容的 Linux 发行版）。以下是所需依赖：

### 系统依赖
- **操作系统**：RHEL 9.5 或更高版本
- **编译工具**：GCC 11+ 或 Clang 14+
- **LLVM/Clang**：版本 15+（推荐从源代码编译以支持 RISCV 后端）

### 安装依赖（RHEL 9.5 示例）
使用 DNF 安装基本包：
```bash
sudo dnf update
sudo dnf install git gcc gcc-c++ cmake make llvm llvm-devel clang clang-devel
```

### RISCV 工具链
为支持 RISCV32i 后端，安装 RISC-V GCC 工具链：
```bash
sudo dnf install gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu
```

### 可选工具
- **QEMU**：用于 RISCV 模拟测试
  ```bash
  sudo dnf install qemu-system-riscv
  ```
- **Spike**：RISCV ISA 模拟器（可选，从 GitHub 克隆编译）

## 构建和安装

### 克隆仓库
```bash
git clone https://github.com/qingxinhome/llvm-C-compiler.git
cd llvm-C-compiler
```

### 配置和构建
使用 CMake 进行 out-of-source 构建（推荐）：
```bash
mkdir build
cd build
cmake .. -DLLVM_DIR=/usr/lib64/cmake/llvm  # 根据你的 LLVM 安装路径调整
make -j$(nproc)
sudo make install  # 可选，安装到系统路径
```

- **CMake 选项**：
  - `-DLLVM_ENABLE_PROJECTS=clang`：启用 Clang 支持（如果未预装）。
  - `-DCMAKE_BUILD_TYPE=Release`：优化构建。
  - `-DRISCV_TARGET=ON`：启用 RISCV32i 后端（默认开启）。

构建完成后，可执行文件将位于 `build/bin/` 目录下。

### 清理构建
```bash
cd build
make clean
# 或删除整个构建目录
cd ..
rm -rf build
```

## 使用指南

### 基本编译流程
编译器支持从 C 源代码生成 RISCV32i 可执行文件。假设可执行文件名为 `llvm-c-compiler`。

1. **编译 C 源代码**：
   ```bash
   ./build/bin/llvm-c-compiler input.c -o output  # 默认生成 RISCV32i ELF
   ```

2. **指定目标**：
   ```bash
   ./build/bin/llvm-c-compiler input.c -march=riscv32 -o output.s  # 生成汇编
   ./build/bin/llvm-c-compiler input.c -march=riscv32 -O2 -o output  # 优化级别 2
   ```

3. **运行生成的代码（使用 QEMU）**：
   ```bash
   qemu-riscv32 output  # 模拟执行
   ```

### 示例
假设有一个简单的 `hello.c`：
```c
#include <stdio.h>

int main() {
    printf("Hello, RISCV!\n");
    return 0;
}
```
编译并运行：
```bash
./build/bin/llvm-c-compiler hello.c -o hello
qemu-riscv32 hello
```

### 调试
- 使用 `llc` 检查 LLVM IR：
  ```bash
  ./build/bin/llvm-c-compiler hello.c -emit-llvm -o hello.ll
  llc hello.ll -march=riscv32 -o hello.s
  ```
- GDB 支持：`riscv64-unknown-elf-gdb output`

## 项目结构

```
llvm-C-compiler/
├── src/                    # 源代码目录
│   ├── frontend/           # C 语言解析器（使用 Clang 或自定义）
│   ├── backend/            # RISCV32i 后端实现
│   └── main.cpp            # 入口文件
├── include/                # 头文件
├── tests/                  # 测试用例（C 源代码和预期输出）
├── cmake/                  # CMake 模块
├── docs/                   # 文档（设计说明、API 参考）
├── .gitignore              # Git 忽略文件
├── CMakeLists.txt          # 顶级 CMake 配置
└── README.md               # 本文件
```

## 贡献指南

1. **Fork 仓库** 并克隆你的 Fork。
2. **创建分支**：`git checkout -b feature/your-feature`。
3. **提交更改**：确保代码通过测试，并添加单元测试。
4. **推送并 PR**：`git push origin feature/your-feature`，然后在 GitHub 创建 Pull Request。
5. **代码规范**：使用 C++17 标准，遵循 LLVM 编码风格（见 [LLVM Coding Standards](https://llvm.org/docs/CodingStandards.html)）。

请在 Issues 中报告 Bug 或提出功能请求。

## 许可证

本项目采用 MIT 许可证。详见 [LICENSE](LICENSE) 文件。

## 联系与支持

- **作者**：Qingxin Home
- **仓库**：https://github.com/qingxinhome/llvm-C-compiler
- **Issues**：https://github.com/qingxinhome/llvm-C-compiler/issues

如果您有问题或建议，欢迎通过 GitHub Issues 联系！

---

*最后更新：2025 年 10 月 25 日*