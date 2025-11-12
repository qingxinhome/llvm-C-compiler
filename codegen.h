#pragma once

#include "ast.h"
#include <memory>
#include <functional>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/ADT/DenseMap.h>

class CodeGen : public Visitor, public TypeVisitor {
public:
    // 构造函数
    CodeGen(std::shared_ptr<Program> program) {
        module = std::make_unique<llvm::Module>(program->fileName, context);
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
    llvm::Value* VisitReturnStmt(ReturnStmt *stmt) override;
    llvm::Value* VisitBlockStmt(BlockStmt *blockstmt) override;
    llvm::Value* VisitDoWhileStmt(DoWhileStmt *stmt) override;
    llvm::Value* VisitSwitchStmt(SwitchStmt *stmt) override;
    llvm::Value* VisitCaseStmt(CaseStmt *stmt) override;
    llvm::Value* VisitDefaultStmt(DefaultStmt *stmt) override;

    llvm::Value* VisitVariableDeclExpr(VariableDecl *decl) override;
    llvm::Value* VisitFunctionDeclExpr(FunctionDecl *decl) override;
    llvm::Value* VisitVariableAccessExpr(VariableAccessExpr *expr) override;
    llvm::Value* VisitBinaryExpr(BinaryExpr *binaryExpr) override;
    llvm::Value* VisitThreeExpr(ThreeExpr *expr) override;
    llvm::Value* VisitCastExpr(CastExpr *expr) override;
    llvm::Value* VisitUnaryExpr(UnaryExpr *unaryExpr) override;
    llvm::Value* VisitSizeOfExpr(SizeOfExpr *sizeofExpr) override;
    llvm::Value* VisitPostIncExpr(PostIncExpr *postIncExpr) override;
    llvm::Value* VisitPostDecExpr(PostDecExpr *postDecExpr) override;
    llvm::Value* VisitPostSubscript(PostSubscript *expr) override;
    llvm::Value* VisitPostMemberDotExpr(PostMemberDotExpr *expr) override;
    llvm::Value* VisitPostMemberArrowExpr(PostMemberArrowExpr *expr) override;
    llvm::Value* VisitPostFunctionCallExpr(PostFunctionCallExpr *expr) override;
    llvm::Value* VisitNumberExpr(NumberExpr *numberExpr) override;
    llvm::Value* VisitStringExpr(StringExpr *expr) override;

    llvm::Type* VisitPrimaryType(CPrimaryType *type) override;
    llvm::Type* VisitPointType(CPointType *type) override;
    llvm::Type* VisitArrayType(CArrayType *type) override;
    llvm::Type* VisitRecordType(CRecordType *type) override;
    llvm::Type* VisitFuncType(CFuncType *type) override;

private:
    void Cast(llvm::Value *&val) ;
    /* CastValue用于将Value转换为目标类型的Value值*/
    void CastValue(llvm::Value *&val, llvm::Type *destTy);
    void AssignCast(llvm::Value *&val, llvm::Type *destTy);
    void BinaryArithCast(llvm::Value *&left, llvm::Value *&right);
    llvm::Value *BooleanCast(llvm::Value *value);

private:
    llvm::LLVMContext context;
    /*
      Module是IR的顶级容器，存储函数、全局变量、元数据等，类似一个编译单元（.c 文件）。
      添加函数（getOrInsertFunction）、全局变量（getOrInsertGlobal），并作为 IR 输出或编译的入口
    */
    std::unique_ptr<llvm::Module> module;
    // 当前Function, 注：一个Module模块里是可以有很多Function的，需要记录当前Function
    llvm::Function *curFunc{nullptr};
    llvm::IRBuilder<> irBuilder{context}; // // C++11+初始化


    /*
        llvm::DenseMap是LLVM 核心库（ADT-Abstract Data Types）中一个高性能哈希映射容器，
        专为LLVM 内部使用设计。它是 std::unordered_map的高效替代品
    */
    // 用于存储，在CodeGen过程中， 执行break后需要跳转到的block块
    llvm::DenseMap<AstNode*, llvm::BasicBlock*> breakBBs;
    // 用于存储，在CodeGen过程中， 执行continue后需要跳转到的block块
    llvm::DenseMap<AstNode*, llvm::BasicBlock*> continueBBs;
    // 使用SmallVector作为栈结构记录switch指令， 
    llvm::SmallVector<llvm::SwitchInst*> switchStack;


private:
    /*
        llvm的ir也有变量/函数符号的作用域问题
    */
    // 局部变量（函数内部的变量） 地址和类型 作用域符号存储
    llvm::SmallVector<llvm::StringMap<std::pair<llvm::Value*, llvm::Type*>>>  localVarMap;
    // 全局变量/函数的 <地址，类型> 的存储， 对于函数而言就是， 函数的入口地址和函数的类型
    llvm::StringMap<std::pair<llvm::Value*, llvm::Type*>> globalVarMap;
private:
    /* llvm IR 作用域的管理 */
    void AddLocalVarToMap(llvm::Value* addr, llvm::Type* ty, llvm::StringRef name);
    void AddGlobalVarToMap(llvm::Value* addr, llvm::Type* ty, llvm::StringRef name);
    std::pair<llvm::Value*, llvm::Type*> GetVarByName(llvm::StringRef name);

    void PushScope();
    void PopScope();
    void ClearVarScope();
};