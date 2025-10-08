#include "codegen.h"
#include <llvm/IR/Verifier.h>

using namespace llvm;

/*
; ModuleID = 'expr'
source_filename = "expr"

@0 = private unnamed_addr constant [18 x i8] c"expr value: [%d]\0A\00", align 1
@1 = private unnamed_addr constant [18 x i8] c"expr value: [%d]\0A\00", align 1
@2 = private unnamed_addr constant [18 x i8] c"expr value: [%d]\0A\00", align 1

declare i32 @printf(ptr, ...)

// llvm IR 会做常量折叠
define i32 @main() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @0, i32 4)
  %1 = call i32 (ptr, ...) @printf(ptr @1, i32 10)
  %2 = call i32 (ptr, ...) @printf(ptr @2, i32 -1)
  ret i32 0
}
*/

llvm::Value* CodeGen::VisitProgram(Program *program){
    // 声明printf函数: int printf(const char *format, ...);
    FunctionType *printfType = FunctionType::get(irBuilder.getInt32Ty(), {irBuilder.getPtrTy()}, true);
    Function* printf = Function::Create(printfType, GlobalValue::LinkageTypes::ExternalLinkage, "printf", module.get());

    // main
    FunctionType *mFuncType = FunctionType::get(irBuilder.getInt32Ty(), false);
    // ExternalLinkage 外部链接属性可以被连接器找到
    Function *mFunc = Function::Create(mFuncType, GlobalValue::LinkageTypes::ExternalLinkage, "main", module.get());

    BasicBlock *entryBB = BasicBlock::Create(context, "entry", mFunc);
    irBuilder.SetInsertPoint(entryBB);
    // 记录当前的Function函数
    curFunc = mFunc;

    llvm::Value* lastValue;
    for (auto &stmtNode : program->stmtNodeVec){
        lastValue = stmtNode->Accept(this);
    }

    if (lastValue != nullptr) {
        irBuilder.CreateCall(printf, {irBuilder.CreateGlobalStringPtr("expr value: [%d]\n"), lastValue});
    } else {
        irBuilder.CreateCall(printf, {irBuilder.CreateGlobalStringPtr("last inst is not expr\n")});
    }
    

    // 在llvm IR中一切指令都是值llvm::Value, llvm::Value是一切的基类
    // CreateRet返回的类型是llvm::ReturnInst，但llvm::ReturnInst是llvm::Value的派生类
    llvm::Value* ret = irBuilder.CreateRet(irBuilder.getInt32(0));

    verifyFunction(*mFunc);
    module->print(outs(), nullptr);

    return ret;
}

llvm::Value* CodeGen::VisitDeclareStmt(DeclareStmt *declstmt) {
    llvm::Value* lastValue;
    for (const auto &node : declstmt->nodeVec) {
        lastValue = node->Accept(this);
    }
    return lastValue;
}

///
llvm::Value* CodeGen::VisitIfStmt(IfStmt *ifstmt) {
    llvm::BasicBlock *condBB = llvm::BasicBlock::Create(context, "cond", curFunc);
    llvm::BasicBlock *thenBB = llvm::BasicBlock::Create(context, "then", curFunc);
    llvm::BasicBlock *elseBB = nullptr;
    if (ifstmt->elseNode != nullptr) {
        elseBB = llvm::BasicBlock::Create(context, "else", curFunc);
    }
    llvm::BasicBlock *lastBB = llvm::BasicBlock::Create(context, "last", curFunc);

    // 将指令执行流无条件跳转至 if 条件块
    irBuilder.CreateBr(condBB);

    // handl if condBB
    irBuilder.SetInsertPoint(condBB);
    llvm::Value* val = ifstmt->condNode->Accept(this);
    // int32 类型的比较指令ne ：不等于
    llvm::Value* condval =irBuilder.CreateICmpNE(val, irBuilder.getInt32(0));
    
    if (ifstmt->elseNode != nullptr) {
        // 条件跳转
        irBuilder.CreateCondBr(condval, thenBB, elseBB);

        // handle then block
        irBuilder.SetInsertPoint(thenBB);
        ifstmt->thenNode->Accept(this);
        irBuilder.CreateBr(lastBB);// 无条件跳转

        // handle else block
        irBuilder.SetInsertPoint(elseBB);
        ifstmt->elseNode->Accept(this);
        irBuilder.CreateBr(lastBB); // 无条件跳转
    } else  {
        // 条件跳转
        irBuilder.CreateCondBr(condval, thenBB, lastBB);

        // then block
        irBuilder.SetInsertPoint(thenBB);
        ifstmt->thenNode->Accept(this);
        irBuilder.CreateBr(lastBB);// 无条件跳转
    }
    irBuilder.SetInsertPoint(lastBB);
    //  因为if语句不是表达式，if 语句没有值，不需要返回具体的llvm:Value
    return nullptr;
}


llvm::Value* CodeGen::VisitBinaryExpr(BinaryExpr *binaryExpr){
    llvm::Value* left = binaryExpr->left->Accept(this);
    llvm::Value* right = binaryExpr->right->Accept(this);

    switch (binaryExpr->op)
    {
    case OpCode::add:{
        return irBuilder.CreateNSWAdd(left, right);
    }
    case OpCode::sub:{
        return irBuilder.CreateNSWSub(left, right);
    }
    case OpCode::mul:{
        return irBuilder.CreateNSWMul(left, right);
    }
    case OpCode::div:{
        return irBuilder.CreateSDiv(left, right);
    }
    default:
        break;
    }
    return nullptr;
}

llvm::Value* CodeGen::VisitNumberExpr(NumberExpr *numberExpr){
    return irBuilder.getInt32(numberExpr->token.value);
}

llvm::Value* CodeGen::VisitVariableDeclExpr(VariableDecl *decl) {
    llvm::Type* ty = nullptr;
    if (decl->type == CType::GetIntTy()) {
        // ty = llvm::Type::getInt32Ty(context);
        ty = irBuilder.getInt32Ty();
    }

    llvm::StringRef varName(decl->token.ptr, decl->token.len);
    llvm::Value* value = irBuilder.CreateAlloca(ty, nullptr, varName);
    
    varAddrTypeMap.insert({varName, {value, ty}});
    return value;
}

// 变量访问返回的应该是一个右值
//  CreateLoad返回的是一个右值
llvm::Value* CodeGen::VisitVariableAccessExpr(VariableAccessExpr *expr) {
    llvm::StringRef varName(expr->token.ptr, expr->token.len);
    std::pair pair = varAddrTypeMap[varName];
    llvm::Value* addr = pair.first;
    llvm::Type* ty = pair.second;
    return irBuilder.CreateLoad(ty, addr, varName);
}

// a = 3; => rvalue 这个表达式的值一个右值（这里说的是表达式不是变量）
// CreateStore返回的是一个地址（地址对应的是左值）
//  CreateLoad返回的是一个右值
llvm::Value* CodeGen::VisitAssignExpr(AssignExpr *expr) {
    auto left = expr->left;
    VariableAccessExpr *varAccessExpr = (VariableAccessExpr *)left.get();

    llvm::StringRef varName(varAccessExpr->token.ptr, varAccessExpr->token.len);
    std::pair pair = varAddrTypeMap[varName];
    llvm::Value *leftAddr = pair.first;
    llvm::Type *leftTy = pair.second;

    llvm::Value* rightValue = expr->right->Accept(this);

    irBuilder.CreateStore(rightValue, leftAddr);

    // CreateLoad返回的是一个右值
    return irBuilder.CreateLoad(leftTy, leftAddr, varName);
}


//在llvm IR中一切指令都是值llvm::Value