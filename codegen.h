#pragma once

#include "ast.h"
#include <memory>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/ADT/DenseMap.h>

class CodeGen : public Visitor {
public:
    // 构造函数
    CodeGen(std::shared_ptr<Program> program) {
        module = std::make_shared<llvm::Module>("expr", context);
        VisitProgram(program.get());
    }
private:
    llvm::Value* VisitProgram(Program *program) override;
    llvm::Value* VisitDeclareStmt(DeclareStmt *declstmt) override;
    llvm::Value* VisitIfStmt(IfStmt *ifstmt) override;
    llvm::Value* VisitForStmt(ForStmt *forstmt) override;
    llvm::Value* VisitBreakStmt(BreakStmt *breakstmt) override;
    llvm::Value* VisitContinueStmt(ContinueStmt *continuestmt) override;
    llvm::Value* VisitBlockStmt(BlockStmt *blockstmt) override;
    llvm::Value* VisitVariableDeclExpr(VariableDecl *decl) override;
    llvm::Value* VisitVariableAccessExpr(VariableAccessExpr *expr) override;
    llvm::Value* VisitBinaryExpr(BinaryExpr *binaryExpr) override;
    llvm::Value* VisitNumberExpr(NumberExpr *numberExpr) override;
private:
    llvm::LLVMContext context;
    std::shared_ptr<llvm::Module> module;
    // 当前Function, 注：一个模块里是可以有很多Function的，需要记录当前Function
    llvm::Function *curFunc{nullptr};
    llvm::IRBuilder<> irBuilder{context}; // // C++11+初始化

    // StringMap存储变量的地址和类型
    llvm::StringMap<std::pair<llvm::Value*, llvm::Type*>> varAddrTypeMap;

    // 存储在CodeGen过程中， 执行break后需要跳转到的block块
    llvm::DenseMap<AstNode*, llvm::BasicBlock*> breakBBs;
    // 存储在CodeGen过程中， 执行continue后需要跳转到的block块
    llvm::DenseMap<AstNode*, llvm::BasicBlock*> continueBBs;
};