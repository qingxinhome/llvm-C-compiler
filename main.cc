#include "lexer.h"
#include "parser.h"
#include "printVisitor.h"
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/ErrorOr.h>
#include "codegen.h"
#include "diag_engine.h"

#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/TargetSelect.h>

#define JIT_TEST

int main(int argc, char* argv[]) {
#ifdef JIT_TEST
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    LLVMLinkInMCJIT();
#endif

    if (argc < 2) {
        printf("please input filename\n");
        return -1;
    }

    const char* filename = argv[1];
    llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> buf = llvm::MemoryBuffer::getFile(filename);
    if (!buf) {
        llvm::errs() << "Error: can't open file '" << filename << "': "
                << buf.getError().message() << "\n";
        return -1;
    }

    llvm::SourceMgr mgr;
    // 诊断引擎
    DiagEngine diagEngine(mgr);

    mgr.AddNewSourceBuffer(std::move(*buf), llvm::SMLoc());

    Lexer lexer(mgr, diagEngine);
    Sema sema(diagEngine);
    Parser parser(lexer, sema);
    auto program = parser.ParseProgram();
    PrintVisitor printVisitor(program);
    
    CodeGen codegen(program);    /*将AST转换为LLVM IR*/
    auto &module = codegen.GetModule();
    assert(!llvm::verifyModule(*module));
    module->print(llvm::outs(), nullptr);
#ifdef JIT_TEST
    {   
        // 创建llvm::EngineBuilder 对象,用于配置和构建 llvm::ExecutionEngine
        llvm::EngineBuilder builder(std::move(module));  /* std::move(module) 将之前生成的llvm::Module的所有权转移给EngineBuilder */
        std::string error;
        /* SectionMemoryManager 内存管理器 -> 管理内存分配和符号解析*/
        auto ptr = std::make_unique<llvm::SectionMemoryManager>();
        auto ref = ptr.get();
        std::unique_ptr<llvm::ExecutionEngine> ee(
            builder.setErrorStr(&error)
                .setEngineKind(llvm::EngineKind::JIT)     /* 指定使用 JIT 引擎（MCJIT）*/
                .setOptLevel(llvm::CodeGenOptLevel::None) /* 禁用优化，生成未经优化的机器码 */
                .setSymbolResolver(std::move(ptr))        /* 将SectionMemoryManager的所有权转移给执行引擎，用于内存管理和符号解析*/
                .create()
        );

        ee->finalizeObject();
        // 错误！ptr 所有权已转移！所有权转移后，别碰旧指针！让引擎自己 finalize
        // ref->finalizeMemory(&error);                        /* finalizeMemory方法用于完成JIT代码的内存布局*/
        void *addr = (void *)ee->getFunctionAddress("main");   /* 通过ExecutionEngine的getFunctionAddress 方法获取main 函数的内存地址*/
        int res = ((int (*)())addr)();                         /* 将addr强制转换为函数指针类型int (*)()（无参，返回 int）*/
        llvm::errs() << "result:" << res << "\n";
    }
#endif

    return 0;
}


/*
(1) llvm::InitializeNativeTarget()
作用：
    初始化 LLVM 的本机目标（Native Target），为当前硬件架构（如 x86_64 或 ARM）注册目标相关的元数据和代码生成支持。
包括设置指令集体系结构（ISA）、数据布局（如指针大小、字节序）等。
逻辑重要性：
    这是 JIT 或 AOT（提前编译）代码生成的前提。没有此初始化，LLVM 无法识别目标架构，导致后续 JIT 失败。

(2) llvm::InitializeNativeTargetAsmPrinter()
作用：
    初始化 LLVM 的本机目标汇编打印器（ASM Printer），用于将 LLVM IR 转换为目标架构的汇编代码或机器码。
注册目标特定的汇编生成逻辑，包括指令编码和汇编格式（如 Intel/AT&T 语法）。
逻辑重要性：
    JIT 执行需要将IR转换为可执行机器码，汇编打印器是关键组件，确保生成的代码符合目标架构的调用约定。

(3) LLVMLinkInMCJIT()
作用：
    链接并启用LLVM的 MCJIT（Machine Code JIT）引擎，注册 MCJIT 实现，使ExecutionEngine能使用 MCJIT 作为后端。
MCJIT 将 LLVM IR 动态编译为本机机器码并执行。
逻辑重要性：
    这是 JIT 执行的核心，允许程序在运行时编译和运行生成的代码。

*/