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

    llvm::Value* lastValue;
    for (auto &expr : program->exprVec)
    {
        lastValue = expr->Accept(this);
    }
    irBuilder.CreateCall(printf, {irBuilder.CreateGlobalStringPtr("expr value: [%d]\n"), lastValue});

    // 在llvm IR中一切指令都是值llvm::Value, llvm::Value是一切的基类
    // CreateRet返回的类型是llvm::ReturnInst，但llvm::ReturnInst是llvm::Value的派生类
    llvm::Value* ret = irBuilder.CreateRet(irBuilder.getInt32(0));

    verifyFunction(*mFunc);
    module->print(outs(), nullptr);

    return ret;
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
    return irBuilder.getInt32(numberExpr->number);
}

llvm::Value* CodeGen::VisitVariableDeclExpr(VariableDecl *decl) {
    llvm::Type* ty = nullptr;
    if (decl->type == CType::GetIntTy()) {
        // ty = llvm::Type::getInt32Ty(context);
        ty = irBuilder.getInt32Ty();
    }
    llvm::Value* value = irBuilder.CreateAlloca(ty, nullptr, decl->name);
    varAddrMap.insert({decl->name, value});
    return value;
}

llvm::Value* CodeGen::VisitVariableAccessExpr(VariableAccessExpr *expr) {
    llvm::Value* varAddr= varAddrMap[expr->name];
    llvm::Type* ty = nullptr;
    if (expr->type = CType::GetIntTy()) {
        ty = irBuilder.getInt32Ty();
    }
    return irBuilder.CreateLoad(ty, varAddr, expr->name);
}

llvm::Value* CodeGen::VisitAssignExpr(AssignExpr *expr) {
    auto left = expr->left;
    VariableAccessExpr *varAccessExpr = (VariableAccessExpr *)left.get();
    auto leftVarAddr = varAddrMap[varAccessExpr->name];
    llvm::Value* rightValue = expr->right->Accept(this);
    return irBuilder.CreateStore(rightValue, leftVarAddr);
}


//在llvm IR中一切指令都是值llvm::Value