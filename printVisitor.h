#pragma once

#include "ast.h"
#include "parser.h"

class PrintVisitor : public Visitor {
public:
    // 构造函数
    PrintVisitor(std::shared_ptr<Program> program);
    llvm::Value* VisitProgram(Program *program) override;
    llvm::Value* VisitDeclareStmt(DeclareStmt *declstmt) override;
    llvm::Value* VisitIfStmt(IfStmt *ifstmt) override;
    llvm::Value* VisitForStmt(ForStmt *ifstmt) override;
    llvm::Value* VisitBreakStmt(BreakStmt *breakstmt) override;
    llvm::Value* VisitContinueStmt(ContinueStmt *continuestmt) override;
    llvm::Value* VisitBlockStmt(BlockStmt *blockstmt) override;
    llvm::Value* VisitVariableDeclExpr(VariableDecl *decl) override;
    llvm::Value* VisitVariableAccessExpr(VariableAccessExpr *expr) override;
    llvm::Value* VisitAssignExpr(AssignExpr *expr) override;
    llvm::Value* VisitBinaryExpr(BinaryExpr *binaryExpr) override;
    llvm::Value* VisitNumberExpr(NumberExpr *numberExpr) override;
};