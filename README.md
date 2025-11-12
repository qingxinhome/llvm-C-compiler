# LLVM-C-Compiler

<p align="center">
  <img src="https://img.shields.io/badge/LLVM-18.1.8-blue?style=for-the-badge&logo=llvm" alt="LLVM 18.1.8">
  <img src="https://img.shields.io/badge/C-C99-green?style=for-the-badge" alt="C99">
  <img src="https://img.shields.io/badge/Target-RISCV32i-orange?style=for-the-badge" alt="RISCV32i">
  <img src="https://img.shields.io/badge/JIT-Supported-success?style=for-the-badge" alt="JIT">
  <img src="https://img.shields.io/badge/Build-CMake-blueviolet?style=for-the-badge" alt="CMake">
  <img src="https://img.shields.io/github/license/qingxinhome/llvm-C-compiler?color=yellow&style=for-the-badge" alt="MIT License">
</p>

---

## 项目简介

**LLVM-C-Compiler** 是一个基于 **LLVM 18.1.8** 框架实现的 **轻量级 C99 编译器**，专为 **RHEL 9.5** 环境优化设计。该项目充分利用 LLVM 的强大中间表示（IR）基础设施，从 C 源代码解析到 RISCV32i 机器码生成的全流程编译，支持静态编译和 JIT 即时执行。

### 为什么选择 LLVM？
LLVM 作为现代编译器框架的核心，提供高效的 IR 生成、优化和代码生成能力。本项目突出 LLVM 的使用：
- **前端**：利用 Clang/LLVM 解析 C99 语法，生成高质量 IR。
- **后端**：集成 RISCV32i Target，自动生成 RV32I 指令集汇编。
- **优化**：支持 LLVM Pass 管道，实现跨函数优化和寄存器分配。
- **JIT**：通过 ExecutionEngine 实现动态加载和执行，适合嵌入式开发。

项目处于**开发早期阶段**，当前支持 C99 基础语法（变量、表达式、控制流、函数），目标是完整 C99 标准覆盖。欢迎贡献！

---

## 核心特性

| 特性 | 描述 | LLVM 集成方式 |
|------|------|---------------|
| **C99 前端** | 支持变量声明、算术/逻辑表达式、if/while/for 控制流、函数定义/调用 | 使用 LLVM IRBuilder 生成 IR，支持 Clang 风格的 AST 解析 |
| **LLVM IR 生成** | 自动生成优化 IR，支持类型转换（如 int → float）和内存管理 | 核心依赖 IRBuilder、Value/Type 系统，实现 BinaryArithCast 等 |
| **RISCV32i 后端** | 生成 RV32I 汇编/ELF，支持交叉编译 | 利用 LLVM TargetMachine 和 SelectionDAG 生成 RISCV 指令 |
| **JIT 即时执行** | 直接运行 C 程序，无需文件生成 | ExecutionEngine + SectionMemoryManager，实现 finalizeObject() 动态加载 |
| **优化支持** | -O0 到 -O3 级别 | LLVM PassManager 运行 ScalarOptimizer 等 Pass |
| **模块化设计** | 易扩展新语法或目标 | 自定义 LLVM Pass，兼容多架构（如 x86_64 扩展） |

> **测试环境**：RHEL 9.5 + LLVM 18.1.8，确保 RISCV 后端稳定。

---

## 环境要求

本项目针对 **RHEL 9.5** 环境开发和测试，LLVM 版本固定为 **18.1.8**（支持完整 RISCV 工具链）。

### 系统依赖
- **操作系统**：RHEL 9.5 或兼容 Linux 发行版
- **编译工具**：GCC 11+ 或 Clang 14+
- **LLVM/Clang**：18.1.8（推荐源码编译以启用 RISCV）

### 安装依赖（RHEL 9.5 示例）
```bash
# 更新系统
sudo dnf update

# 基本工具
sudo dnf install git gcc gcc-c++ cmake make ninja-build

# LLVM 18.1.8（如果未安装，从源码编译）
sudo dnf install llvm llvm-devel clang clang-devel  # 或手动编译

# RISC-V 工具链
sudo dnf install gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu

# QEMU 模拟器（用于运行 RISCV 程序）
sudo dnf install qemu-system-riscv

# 可选：Spike 模拟器（从 GitHub 克隆编译）
git clone https://github.com/riscv-software-src/riscv-isa-sim.git
cd riscv-isa-sim && mkdir build && cd build
../configure --prefix=$HOME/riscv-tools
make -j$(nproc) && make install
```

> **LLVM 源码编译指南**（推荐，确保 RISCV 支持）：
> 1. 下载 LLVM 18.1.8：`git clone --branch llvmorg-18.1.8 https://github.com/llvm/llvm-project.git`
> 2. `cd llvm-project && mkdir build && cd build`
> 3. `cmake .. -DLLVM_ENABLE_PROJECTS="clang;lld" -DCMAKE_BUILD_TYPE=Release -DLLVM_TARGETS_TO_BUILD=RISCV`
> 4. `make -j$(nproc) && sudo make install`

---

## 构建和安装

### 1. 克隆仓库
```bash
git clone https://github.com/qingxinhome/llvm-C-compiler.git
cd llvm-C-compiler
```

### 2. 创建构建目录
```bash
mkdir build && cd build
```

### 3. 配置 CMake
```bash
cmake .. \
  -DLLVM_DIR=/usr/lib64/cmake/llvm \  # 调整为你的 LLVM 路径
  -DCMAKE_BUILD_TYPE=Release \
  -DRISCV_TARGET=ON \
  -DLLVM_ENABLE_PROJECTS=clang  # 如果需要 Clang 支持
```

> **常见 CMake 选项**：
> - `-DCMAKE_BUILD_TYPE=Debug`：调试模式
> - `-DLLVM_TARGETS_TO_BUILD=RISCV`：仅构建 RISCV 后端
> - `-DUSE_JIT=ON`：启用 JIT 支持

### 4. 编译
```bash
make -j$(nproc)  # 或 ninja -j$(nproc)
```

### 5. 安装（可选）
```bash
sudo make install
```

> **清理构建**：`make clean` 或 `rm -rf build`

构建完成后，可执行文件位于 `build/bin/llvm-c-compiler`。

---

## 使用指南

### 基本编译流程
编译器支持从 C 源代码到 RISCV32i ELF 的完整流程。

1. **编译 C 源代码**（默认生成 RISCV32i ELF）：
   ```bash
   ./bin/llvm-c-compiler input.c -o output
   ```

2. **指定目标架构**：
   ```bash
   ./bin/llvm-c-compiler input.c -march=riscv32 -o output.s  # 生成汇编
   ./bin/llvm-c-compiler input.c -march=riscv32 -O2 -o output  # 优化级别 2
   ```

3. **运行生成的程序**（使用 QEMU）：
   ```bash
   qemu-riscv32 output
   ```

### 高级选项
| 选项 | 描述 |
|------|------|
| `-emit-llvm` | 输出 `.ll` 文件（LLVM IR） |
| `-dump-ir` | 打印优化前/后 IR |
| `-v` | 详细日志模式 |
| `-S` | 输出汇编代码 |
| `--jit` | 即时执行（JIT 模式） |
| `-O0` / `-O1` / `-O2` / `-O3` | 优化级别 |

### 示例：编译并运行 `hello.c`
```c
// examples/hello.c
#include <stdio.h>

int main() {
    printf("Hello, RISCV from LLVM-C-Compiler!\n");
    return 0;
}
```

```bash
# 编译
./bin/llvm-c-compiler examples/hello.c -o hello

# 运行
qemu-riscv32 hello
```

**预期输出**：
```
Hello, RISCV from LLVM-C-Compiler!
```

### JIT 模式示例
```bash
./bin/llvm-c-compiler examples/hello.c --jit
```

> JIT 内部使用 LLVM ExecutionEngine 动态加载 IR，调用 `finalizeObject()` 完成内存布局。

---

## LLVM 集成详解

本项目 **深度集成 LLVM 18.1.8**，从前端解析到后端代码生成，全程依赖 LLVM 的核心组件。以下是详细说明，突出 LLVM 在各阶段的使用。

### 1. **前端：C 解析 → LLVM IR 生成**
- **LLVM 组件**：`IRBuilder<>`、`LLVMContext`、`Module`、`Function`、`BasicBlock`。
- **流程**：
  1. 使用自定义 Clang-like 解析器（`src/frontend/`）构建 AST。
  2. 通过 `IRBuilder` 生成 IR：
     - 变量声明：`CreateAlloca` 分配栈空间。
     - 表达式：`BinaryArithCast` 处理类型提升（如 int + float → `CreateFAdd`）。
     - 控制流：`CreateBr`、`CreateCondBr` 生成基本块。
  3. 示例（加法 IR 生成）：
     ```cpp
     // src/frontend/expr.cpp 示例
     if (leftTy->isFloatingPointTy()) {
         return irBuilder.CreateFAdd(left, right);  // 浮点加法
     } else {
         return irBuilder.CreateNSWAdd(left, right);  // 整数加法（nsw 优化）
     }
     ```
- **优势**：LLVM IR 确保类型安全（如 `isIntegerTy()` 检查），支持跨平台。

### 2. **中端：IR 优化**
- **LLVM 组件**：`PassManager`、`FunctionPassManager`、`ScalarOptimizer`。
- **流程**：
  1. 运行 LLVM 优化管道：`-O2` 启用 DeadCodeElimination、ConstantPropagation 等 Pass。
  2. 自定义 Pass（`src/backend/optimize.cpp`）：如常量折叠、死代码消除。
- **示例**：
  ```cpp
  // 优化示例
  legacy::PassManager PM;
  PM.add(createPromoteMemoryToRegisterPass());  // 寄存器提升
  PM.run(*module);
  ```

### 3. **后端：IR → RISCV32i 机器码**
- **LLVM 组件**：`TargetMachine`、`SelectionDAG`、`CodeGen`。
- **流程**：
  1. 配置 RISCV Target：`llvm::TargetRegistry::lookupTarget("riscv32")`。
  2. 使用 `SelectionDAGBuilder` 将 IR 转换为 DAG 节点。
  3. 生成汇编：`llc -march=riscv32` 或自定义 TargetDesc。
- **RISCV 特定**：支持 RV32I 基础指令（如 `add`、`lw`、`sw`），通过 `InBoundsGEP` 处理指针偏移。
- **示例（指针加法）**：
  ```cpp
  // src/backend/pointer.cpp 示例
  if (leftTy->isPointerTy()) {
      auto *ElementTy = cast<PointerType>(leftTy)->getElementType();
      return irBuilder.CreateInBoundsGEP(ElementTy, left, {right});  // GEP 偏移
  }
  ```

### 4. **JIT 执行支持**
- **LLVM 组件**：`ExecutionEngine`、`EngineBuilder`、`SectionMemoryManager`。
- **流程**：
  1. `EngineBuilder` 配置 JIT 引擎。
  2. `finalizeObject()` 完成内存布局。
  3. `getFunctionAddress("main")` 获取入口点。
- **示例**：
  ```cpp
  // src/jit/main.cpp 示例
  std::unique_ptr<ExecutionEngine> ee = EngineBuilder(std::move(module)).create();
  ee->finalizeObject();
  int (*main)() = (int(*)())ee->getFunctionAddress("main");
  main();  // 直接执行
  ```
- **优势**：在 RHEL 9.5 环境下，JIT 支持快速原型验证，无需 QEMU。

> **LLVM 版本兼容**：专为 18.1.8 优化，确保 RISCV 后端稳定。升级时需检查 API 变更。

---

## 项目结构

```
llvm-C-compiler/
├── src/                        # 源代码核心
│   ├── frontend/               # C 解析器（AST 构建 + IR 生成）
│   │   ├── parser.cpp          # 语法解析
│   │   └── codegen.cpp         # IRBuilder 使用示例
│   ├── backend/                # RISCV32i 代码生成
│   │   ├── riscv_target.cpp    # TargetMachine 配置
│   │   └── optimize.cpp        # 自定义 Pass
│   └── main.cpp                # 编译器入口
├── include/                    # 头文件
│   ├── ast.h                   # AST 节点定义
│   └── llvm_utils.h            # LLVM 工具封装
├── examples/                   # 示例 C 程序
│   ├── hello.c                 # Hello World
│   └── sum.c                   # 函数调用示例
├── tests/                      # 测试用例
│   ├── unit_tests/             # 单元测试
│   └── integration/            # 端到端测试
├── cmake/                      # CMake 模块
│   └── FindLLVM.cmake          # LLVM 查找脚本
├── docs/                       # 文档
│   ├── llvm_integration.md     # LLVM 详解
│   └── riscv_guide.md          # RISCV 后端指南
├── build/                      # 构建输出（.gitignore）
├── CMakeLists.txt              # 顶级构建配置
├── README.md                   # 本文档
└── LICENSE                     # MIT 许可
```

---

## 贡献指南

1. **Fork 仓库** 并克隆你的 Fork。
2. **创建分支**：`git checkout -b feature/your-feature`（e.g., `feature/add-float-support`）。
3. **开发与测试**：确保代码通过 `make test`，添加新测试用例。
4. **提交更改**：`git commit -m "feat: add float support with LLVM FAdd" -s`（signed-off）。
5. **推送并 PR**：`git push origin feature/your-feature`，在 GitHub 创建 Pull Request。

### 代码规范
- **C++ 标准**：C++17
- **LLVM 风格**：遵循 [LLVM Coding Standards](https://llvm.org/docs/CodingStandards.html)
- **提交信息**：使用 Conventional Commits（e.g., `feat: add JIT support`）
- **测试覆盖**：所有新功能需添加 tests/

> **常见贡献领域**：扩展 C99 语法、优化 RISCV 后端、集成更多 LLVM Pass。

---

## 常见问题 (FAQ)

### Q1: 构建时提示 "LLVM_DIR not found"？
**A**：运行 `llvm-config --cmakedir` 获取路径，然后设置 `-DLLVM_DIR=...`。在 RHEL 9.5 上，通常为 `/usr/lib64/cmake/llvm`。

### Q2: `getFunctionAddress("main")` 返回 0？
**A**：确保函数使用 `ExternalLinkage`，并调用 `ee->finalizeObject()`。检查 IR：`module.print(errs())` 确认 "main" 存在。

### Q3: 浮点运算生成错误 IR（如使用 `CreateAdd`）？
**A**：检查 `BinaryArithCast`：浮点用 `CreateFAdd`，整数用 `CreateNSWAdd`。添加类型检查 `isFloatingPointTy()`。

### Q4: RISCV 汇编无效指令？
**A**：验证工具链：`riscv64-linux-gnu-gcc --version`。使用 `llc -march=riscv32 -verify-machineinstrs` 检查。

### Q5: JIT 执行崩溃？
**A**：确认 `SectionMemoryManager` 正确转移所有权，避免手动 `finalizeMemory`。添加 `if (!addr) { errs() << "Function not found"; }`。

---

## 许可证

[MIT License](LICENSE) © 2025 qingxinhome

---

## 联系与支持

- **仓库**：https://github.com/qingxinhome/llvm-C-compiler
- **作者**：qingxinhome
- **Issues**：报告 Bug 或功能请求
- **讨论**：欢迎 PR 和反馈

**最后更新**：November 12, 2025

---

<p align="center">
  <em>Powered by LLVM 18.1.8 on RHEL 9.5</em><br>
  <sub>Building the future of C compilation with RISCV</sub>
</p>
```

---