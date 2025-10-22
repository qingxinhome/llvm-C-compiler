#pragma once

#include "ast.h"
#include <memory>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/ADT/DenseMap.h>

class CodeGen : public Visitor, public TypeVisitor {
public:
    // 构造函数
    CodeGen(std::shared_ptr<Program> program) {
        module = std::make_unique<llvm::Module>("expr", context);
        VisitProgram(program.get());
    }

    // 返回一个引用
    std::unique_ptr<llvm::Module> & GetModule() {
        return module;
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
    llvm::Value* VisitThreeExpr(ThreeExpr *expr) override;
    llvm::Value* VisitUnaryExpr(UnaryExpr *unaryExpr) override;
    llvm::Value* VisitSizeOfExpr(SizeOfExpr *sizeofExpr) override;
    llvm::Value* VisitPostIncExpr(PostIncExpr *postIncExpr) override;
    llvm::Value* VisitPostDecExpr(PostDecExpr *postDecExpr) override;
    llvm::Value* VisitPostSubscript(PostSubscript *expr) override;
    llvm::Value* VisitNumberExpr(NumberExpr *numberExpr) override;

    llvm::Type* VisitPrimaryType(CPrimaryType *type) override;
    llvm::Type* VisitPointType(CPointType *type) override;
    llvm::Type* VisitArrayType(CArrayType *type) override;

private:
    llvm::LLVMContext context;
    std::unique_ptr<llvm::Module> module;
    // 当前Function, 注：一个Module模块里是可以有很多Function的，需要记录当前Function
    llvm::Function *curFunc{nullptr};
    llvm::IRBuilder<> irBuilder{context}; // // C++11+初始化

    // StringMap存储变量的地址和类型
    llvm::StringMap<std::pair<llvm::Value*, llvm::Type*>> varAddrTypeMap;

    // 存储在CodeGen过程中， 执行break后需要跳转到的block块
    llvm::DenseMap<AstNode*, llvm::BasicBlock*> breakBBs;
    // 存储在CodeGen过程中， 执行continue后需要跳转到的block块
    llvm::DenseMap<AstNode*, llvm::BasicBlock*> continueBBs;
};