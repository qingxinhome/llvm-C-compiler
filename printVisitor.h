#pragma once

#include "ast.h"
#include "parser.h"

class PrintVisitor : public Visitor {
public:
    // 构造函数
    PrintVisitor(std::shared_ptr<Program> program);
    llvm::Value* VisitProgram(Program *program) override;
    llvm::Value* VisitBinaryExpr(BinaryExpr *binaryExpr) override;
    llvm::Value* VisitFactorExpr(FactorExpr *factorExpr) override;
};