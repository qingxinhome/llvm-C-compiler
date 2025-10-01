#include "printVisitor.h"


PrintVisitor::PrintVisitor(std::shared_ptr<Program> program) {
    VisitProgram(program.get());
}

llvm::Value* PrintVisitor::VisitProgram(Program *program) {
    for (auto expr : program->exprVec) {
        expr->Accept(this);
        llvm::outs() << "\n";
    }
    return nullptr;
}

llvm::Value* PrintVisitor::VisitBinaryExpr(BinaryExpr *binaryExpr) {
    // 采用后序遍历方式(表达式的计算都是采用后序遍历)
    binaryExpr->left->Accept(this);
    binaryExpr->right->Accept(this);
    switch (binaryExpr->op)
    {
    case OpCode::add:{
        llvm::outs() << " + ";
        break;
    }
    case OpCode::sub:{
        llvm::outs() << " - ";
        break;
    }
    case OpCode::mul:{
        llvm::outs() << " * ";
        break;
    }
    case OpCode::div:{
        llvm::outs() << " / ";
        break;
    }
    default:
        break;
    }
    return nullptr;
}

llvm::Value* PrintVisitor::VisitFactorExpr(FactorExpr *factorExpr) {
    llvm::outs() << factorExpr->number;
    return nullptr;
}