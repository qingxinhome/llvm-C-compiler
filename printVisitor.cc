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

llvm::Value* PrintVisitor::VisitVariableDeclExpr(VariableDecl *decl) {
    if (decl->type == CType::GetIntTy()) {
        llvm::outs() << "int " << decl->name << ";";
    }
    return nullptr;
}


llvm::Value* PrintVisitor::VisitAssignExpr(AssignExpr *expr) {
    expr->left->Accept(this);
    llvm::outs() << " = ";
    expr->right->Accept(this);
    return nullptr;
}

llvm::Value* PrintVisitor::VisitVariableAccessExpr(VariableAccessExpr *expr) {
    llvm::outs() << expr->name;
}


llvm::Value* PrintVisitor::VisitBinaryExpr(BinaryExpr *binaryExpr) {
    binaryExpr->left->Accept(this);
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
    binaryExpr->right->Accept(this);
    return nullptr;
}

llvm::Value* PrintVisitor::VisitNumberExpr(NumberExpr *numberExpr) {
    llvm::outs() << numberExpr->number;
    return nullptr;
}

