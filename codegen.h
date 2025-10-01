#pragma once

#include "ast.h"
#include <memory>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

class CodeGen : public Visitor {
public:
    // 构造函数
    CodeGen(std::shared_ptr<Program> program) {
        module = std::make_shared<llvm::Module>("expr", context);
        VisitProgram(program.get());
    }
private:
    llvm::Value* VisitProgram(Program *program) override;
    llvm::Value* VisitBinaryExpr(BinaryExpr *binaryExpr) override;
    llvm::Value* VisitFactorExpr(FactorExpr *factorExpr) override;
private:
    llvm::LLVMContext context;
    std::shared_ptr<llvm::Module> module;
    llvm::IRBuilder<> irBuilder{context}; // // C++11+初始化
};