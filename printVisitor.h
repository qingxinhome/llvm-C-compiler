#pragma once

#include "ast.h"
#include "parser.h"

/*
llvm::raw_ostream 是 LLVM 提供的一个抽象输出流类，位于 llvm/Support/raw_ostream.h 头文件中。
它设计用于高效地输出数据（比如字符串、数字或 LLVM IR），支持多种输出目标，如标准输出（stdout）、
标准错误（stderr）、文件或内存缓冲区。
*/

class PrintVisitor : public Visitor, public TypeVisitor {
private:
    llvm::raw_ostream *out;
public:
    // 构造函数
    PrintVisitor(std::shared_ptr<Program> program, llvm::raw_ostream *out = &llvm::outs());
    llvm::Value* VisitProgram(Program *program) override;
    llvm::Value* VisitDeclareStmt(DeclareStmt *declstmt) override;
    llvm::Value* VisitIfStmt(IfStmt *ifstmt) override;
    llvm::Value* VisitForStmt(ForStmt *ifstmt) override;
    llvm::Value* VisitBreakStmt(BreakStmt *breakstmt) override;
    llvm::Value* VisitContinueStmt(ContinueStmt *continuestmt) override;
    llvm::Value* VisitBlockStmt(BlockStmt *blockstmt) override;
    llvm::Value* VisitVariableDeclExpr(VariableDecl *decl) override;
    llvm::Value* VisitVariableAccessExpr(VariableAccessExpr *expr) override;
    llvm::Value* VisitBinaryExpr(BinaryExpr *binaryExpr) override;
    llvm::Value* VisitThreeExpr(ThreeExpr *threeExpr) override;
    llvm::Value* VisitUnaryExpr(UnaryExpr *unaryExpr) override;
    llvm::Value* VisitSizeOfExpr(SizeOfExpr *sizeofExpr) override;
    llvm::Value* VisitPostIncExpr(PostIncExpr *postIncExpr) override;
    llvm::Value* VisitPostDecExpr(PostDecExpr *postDecExpr) override;
    llvm::Value* VisitNumberExpr(NumberExpr *numberExpr) override;

    llvm::Type* VisitPrimaryType(CPrimaryType *type) override;
    llvm::Type* VisitPointType(CPointType *type) override;
};