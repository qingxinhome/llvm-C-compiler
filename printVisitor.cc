#include "printVisitor.h"


PrintVisitor::PrintVisitor(std::shared_ptr<Program> program) {
    VisitProgram(program.get());
}

llvm::Value* PrintVisitor::VisitProgram(Program *program) {
    for (const auto &stmtNode : program->stmtNodeVec) {
        stmtNode->Accept(this);
        llvm::outs() << "\n";
    }
    return nullptr;
}

llvm::Value* PrintVisitor::VisitDeclareStmt(DeclareStmt *declstmt) {
    for (const auto & node : declstmt->nodeVec){
        node->Accept(this);
    }
    return nullptr;
}

llvm::Value* PrintVisitor::VisitIfStmt(IfStmt *ifstmt) {
    llvm::outs() << "if(";
    ifstmt->condNode->Accept(this);
    llvm::outs() << ")\n";
    ifstmt->thenNode->Accept(this);
    if (ifstmt->elseNode != nullptr) {
        llvm::outs() << "\nelse\n";
        ifstmt->elseNode->Accept(this);
    }
    return nullptr;
}

llvm::Value* PrintVisitor::VisitBlockStmt(BlockStmt *blockstmt) {
    llvm::outs() << "{\n";
    for (const auto & node : blockstmt->nodeVec) {
        node->Accept(this);
        llvm::outs() << "\n";
    }
    llvm::outs() << "}";
    return nullptr;
}

llvm::Value* PrintVisitor::VisitVariableDeclExpr(VariableDecl *decl) {
    if (decl->type == CType::GetIntTy()) {
        llvm::StringRef text(decl->token.ptr, decl->token.len);
        llvm::outs() << "int " << text << ";";
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
    llvm::outs() << llvm::StringRef(expr->token.ptr, expr->token.len);
    return nullptr;
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
    case OpCode::equal_equal:{
        llvm::outs() << " == ";
        break;
    }
    case OpCode::not_equal:{
        llvm::outs() << " != ";
        break;
    }
    case OpCode::less:{
        llvm::outs() << " < ";
        break;
    }
    case OpCode::less_equal:{
        llvm::outs() << " <= ";
        break;
    }
    case OpCode::greater:{
        llvm::outs() << " > ";
        break;
    }
    case OpCode::greater_equal:{
        llvm::outs() << " >= ";
        break;
    }
    default:
        break;
    }
    binaryExpr->right->Accept(this);
    return nullptr;
}

llvm::Value* PrintVisitor::VisitNumberExpr(NumberExpr *expr) {
    llvm::outs() << llvm::StringRef(expr->token.ptr, expr->token.len);
    return nullptr;
}

