#include <gtest/gtest.h>

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/CodeGen.h"
#include "lexer.h"
#include "parser.h"
#include "codegen.h"

#include <stdarg.h>
#include <functional>

bool TestProgramUseJit(llvm::StringRef content, int expectValue) {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    LLVMLinkInMCJIT();

    llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> buf = llvm::MemoryBuffer::getMemBuffer(content, "stdin");
     if (!buf) {
        llvm::errs() << "open file failed!!!\n";
        return false;
    }
    llvm::SourceMgr mgr;
    DiagEngine diagEngine(mgr);
    mgr.AddNewSourceBuffer(std::move(*buf), llvm::SMLoc());

    Lexer lex(mgr, diagEngine);
    Sema sema(diagEngine);
    Parser parser(lex, sema);

    auto program = parser.ParseProgram(); 
    CodeGen codegen(program);
    auto &module = codegen.GetModule();
    EXPECT_FALSE(llvm::verifyModule(*module));
    {
        /* 使用JIT执行module的main函数 */
        llvm::EngineBuilder builder(std::move(module));
        std::string error;
        auto ptr = std::make_unique<llvm::SectionMemoryManager>();
        auto ref = ptr.get();
        std::unique_ptr<llvm::ExecutionEngine> ee(
            builder.setErrorStr(&error)
                    .setEngineKind(llvm::EngineKind::JIT)
                    .setOptLevel(llvm::CodeGenOptLevel::None)
                    .setSymbolResolver(std::move(ptr))
                    .create());
        ref->finalizeMemory(&error);

        void *addr = (void *)ee->getFunctionAddress("main");
        int res = ((int (*)())addr)();
        if (res != expectValue) {
            llvm::errs() << "expected: " << expectValue << ", but got " << res << "\n";
        }
        EXPECT_EQ(res, expectValue);
    }
    return true;
}

TEST(CodeGenTest, assign) {
    bool res = TestProgramUseJit("{int a; int b = 4; a = 3; b = 5; a = b;}", 5);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, add_assign) {
    bool res = TestProgramUseJit("{int a; int b = 4; a = 3; b = 5; a += b;}", 8);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, sub_assign) {
    bool res = TestProgramUseJit("{int a; int b = 4; a = 3; b = 5; a -= b;}", -2);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, mul_assign) {
    bool res = TestProgramUseJit("{int a; int b = 4; a = 3; b = 5; a *= b;}", 15);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, div_assign) {
    bool res = TestProgramUseJit("{int a; int b = 4; a = 3; b = 5; a /= b;}", 0);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, mod_assign) {
    bool res = TestProgramUseJit("{int a; int b = 4; a = 3; b = 5; a %= b;}", 3);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, bit_or_assign) {
    bool res = TestProgramUseJit("{int a; int b = 4; a = 3; b = 5; a |= b;}", 7);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, bit_and_assign) {
    bool res = TestProgramUseJit("{int a; int b = 4; a = 3; b = 5; a &= b;}", 1);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, bit_xor_assign) {
    bool res = TestProgramUseJit("{int a; int b = 4; a = 3; b = 5; a ^= b;}", 6);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, left_shift_assign) {
    bool res = TestProgramUseJit("{int a; int b = 4; a = 3; b = 5; a <<= b;}", 96);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, right_shift_assign) {
    bool res = TestProgramUseJit("{int a; int b = 4; a = 3; b = 5; a >>= b;}", 0);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, three_op1) {
    bool res = TestProgramUseJit("{int a = 1, b = 2, ans; ans = (a == 1 ? (b == 2 ? 3 : 5) : 0);}", 3);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, three_op2) {
    bool res = TestProgramUseJit("{int a = 10, b = 20, c; c = (a < b) ? a : b;}", 10);
    ASSERT_EQ(res, true);
}


TEST(CodeGenTest, sizeof_int) {
    bool res = TestProgramUseJit("{int a = 10; sizeof(int);}", 4);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, sizeof_pointer) {
    bool res = TestProgramUseJit("{int a = 10; sizeof(int*);}", 8);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, sizeof_unary) {
    bool res = TestProgramUseJit("{int a = 10; sizeof(a) + sizeof a;}", 8);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, unary_positive) {
    bool res = TestProgramUseJit("{int a = 10; +a;}", 10);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, unary_negative) {
    bool res = TestProgramUseJit("{int a = 10; -a;}", -10);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, unary_logical_not) {
    bool res = TestProgramUseJit("{int a = 10; !a;}", 0);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, unary_bit_not) {
    bool res = TestProgramUseJit("{int a = 10; ~a;}", -11);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, unary_addr_dref) {
    bool res = TestProgramUseJit("{int a = 10; int *p = &a; *p;}", 10);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, unary_inc) {
    bool res = TestProgramUseJit("{int a = 10; ++a;}", 11);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, unary_dec) {
    bool res = TestProgramUseJit("{int a = 10; --a;}", 9);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, post_dec) {
    bool res = TestProgramUseJit("{int a = 10; a--;}", 10);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, post_inc) {
    bool res = TestProgramUseJit("{int a = 10; a++;}", 10);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, post_inc_dref) {
    bool res = TestProgramUseJit("{int a = 10, *p = &a; *p++;}", 10);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, post_dec_dref) {
    bool res = TestProgramUseJit("{int a = 10, *p = &a; *p--;}", 10);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, unary_dref_assign) {
    bool res = TestProgramUseJit("{int a = 10, b = 20, *p = &a; *p = 100; a;}", 100);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, array_subscript) {
    bool res = TestProgramUseJit("{int a[3]; a[0] = 4;a[0];}", 4);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, array_subscript2) {
    bool res = TestProgramUseJit("{int a[3][5]; a[2][4] = 4;a[2][4];}", 4);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, array_init1) {
    bool res = TestProgramUseJit("{int a[3] = {1,101}; a[1];}", 101);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, array_init2) {
    bool res = TestProgramUseJit("{int a[3][4] = {{1,101},{2,6}}; a[1][1];}", 6);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, array_init3) {
    bool res = TestProgramUseJit("{int a[3][4] = {{1,101},{2,6}}; int (*p)[3][4] = &a; (*p)[1][1];}", 6);
    ASSERT_EQ(res, true);
}

TEST(CodeGenTest, struct_1) {
    bool res = TestProgramUseJit("{struct A{int a, b; int *p;}; struct A a; 1;}", 1);
    ASSERT_EQ(res, true);
}


// TEST(CodeGenTest, unary_dec_dref) {
//     bool res = TestProgramUseJit("{int a = 10, b = 20, *p = &a; *--p;}", 20);
//     ASSERT_EQ(res, true);
// }

// TEST(CodeGenTest, unary_inc_dref) {
//     bool res = TestProgramUseJit("{int a = 10, b = 20, *p = &b; *++p;}", 10);
//     ASSERT_EQ(res, true);
// }