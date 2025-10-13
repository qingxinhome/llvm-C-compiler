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

llvm::Value* CodeGen::VisitBlockStmt(BlockStmt *blockstmt) {
    // block块中最后的语句有可能是一个表达式
    llvm::Value* lastValue;
    for (const auto &node : blockstmt->nodeVec) {
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

llvm::Value* CodeGen::VisitForStmt(ForStmt *forstmt) {
    llvm::BasicBlock *initBB = llvm::BasicBlock::Create(context, "for.init", curFunc);
    llvm::BasicBlock *condBB = llvm::BasicBlock::Create(context, "for.cond", curFunc);
    llvm::BasicBlock *incBB = llvm::BasicBlock::Create(context, "for.inc", curFunc);
    llvm::BasicBlock *bodyBB = llvm::BasicBlock::Create(context, "for.body", curFunc);
    llvm::BasicBlock *lastBB = llvm::BasicBlock::Create(context, "for.last", curFunc);

    // 将for循环语句中可以被break跳转到的block块记录在breakBBs Map中
    breakBBs.insert({forstmt, lastBB});
    // 将for循环语句中可以被continue跳转到的block块记录在breakBBs Map中
    continueBBs.insert({forstmt, incBB});

    // 1. 先无条件跳转到init块中
    irBuilder.CreateBr(initBB);
    irBuilder.SetInsertPoint(initBB);
    if (forstmt->initNode != nullptr) {
        forstmt->initNode->Accept(this);
    }
    // 由初始快无条件跳转至条件块
    irBuilder.CreateBr(condBB);

    irBuilder.SetInsertPoint(condBB);
    if (forstmt->condNode != nullptr) {
        llvm::Value *val = forstmt->condNode->Accept(this);
        llvm::Value *condVal = irBuilder.CreateICmpNE(val, irBuilder.getInt32(0));
        irBuilder.CreateCondBr(condVal, bodyBB, lastBB);
    } else {
         irBuilder.CreateBr(bodyBB);
    }

    irBuilder.SetInsertPoint(bodyBB);
    if (forstmt->bodyNode != nullptr) {
        forstmt->bodyNode->Accept(this);
    }
    irBuilder.CreateBr(incBB);

    irBuilder.SetInsertPoint(incBB);
    if (forstmt->incNode != nullptr) {
        forstmt->incNode->Accept(this);
    }
    irBuilder.CreateBr(condBB);

    irBuilder.SetInsertPoint(lastBB);

    breakBBs.erase(forstmt);
    continueBBs.erase(forstmt);

    return nullptr;
}


llvm::Value* CodeGen::VisitContinueStmt(ContinueStmt *continuestmt) {
    /// jump incBB;
    llvm::BasicBlock *targetBB = continueBBs[continuestmt->targetNode.get()];
    irBuilder.CreateBr(targetBB);

    llvm::BasicBlock *out = llvm::BasicBlock::Create(context, "for.continue.death", curFunc);
    irBuilder.SetInsertPoint(out);
    return nullptr;
}

llvm::Value* CodeGen::VisitBreakStmt(BreakStmt *breakstmt) {
    /// jump lastBB;
    llvm::BasicBlock* targetBB = breakBBs[breakstmt->targetNode.get()];
    irBuilder.CreateBr(targetBB);

    llvm::BasicBlock *out = llvm::BasicBlock::Create(context, "for.break.death", curFunc);
    irBuilder.SetInsertPoint(out);
    return nullptr;
}


llvm::Value* CodeGen::VisitBinaryExpr(BinaryExpr *binaryExpr){
    switch (binaryExpr->op)
    {
    case OpCode::add:{
        llvm::Value* left = binaryExpr->left->Accept(this);
        llvm::Value* right = binaryExpr->right->Accept(this);
        return irBuilder.CreateNSWAdd(left, right);
    }
    case OpCode::sub:{
        llvm::Value* left = binaryExpr->left->Accept(this);
        llvm::Value* right = binaryExpr->right->Accept(this);
        return irBuilder.CreateNSWSub(left, right);
    }
    case OpCode::mul:{
        llvm::Value* left = binaryExpr->left->Accept(this);
        llvm::Value* right = binaryExpr->right->Accept(this);
        return irBuilder.CreateNSWMul(left, right);
    }
    case OpCode::div:{
        llvm::Value* left = binaryExpr->left->Accept(this);
        llvm::Value* right = binaryExpr->right->Accept(this);
        return irBuilder.CreateSDiv(left, right);
    }
    case OpCode::mod:{
        llvm::Value* left = binaryExpr->left->Accept(this);
        llvm::Value* right = binaryExpr->right->Accept(this);
        return irBuilder.CreateSRem(left, right);
    }
    case OpCode::bitOr: {
        llvm::Value* left = binaryExpr->left->Accept(this);
        llvm::Value* right = binaryExpr->right->Accept(this);
        return irBuilder.CreateOr(left, right);
    }
    case OpCode::bitAnd: {
        llvm::Value* left = binaryExpr->left->Accept(this);
        llvm::Value* right = binaryExpr->right->Accept(this);
        return irBuilder.CreateAnd(left, right);
    }
    case OpCode::bitXor: {
        llvm::Value* left = binaryExpr->left->Accept(this);
        llvm::Value* right = binaryExpr->right->Accept(this);
        return irBuilder.CreateXor(left, right);
    }
    case OpCode::leftShift: {
        llvm::Value* left = binaryExpr->left->Accept(this);
        llvm::Value* right = binaryExpr->right->Accept(this);
        return irBuilder.CreateShl(left, right);
    }
    case OpCode::rightShift: {
        llvm::Value* left = binaryExpr->left->Accept(this);
        llvm::Value* right = binaryExpr->right->Accept(this);
        return irBuilder.CreateAShr(left, right);
    }
    case OpCode::equal_equal:{
        llvm::Value* left = binaryExpr->left->Accept(this);
        llvm::Value* right = binaryExpr->right->Accept(this);
        // 比较运算返回的结果是一个1byte整形值， 返回后无法与后来的int32表达式做运算, 需要做类型转换
        llvm::Value *value = irBuilder.CreateICmpEQ(left, right);
        return irBuilder.CreateIntCast(value, irBuilder.getInt32Ty(), true);
    }
    case OpCode::not_equal:{
        llvm::Value* left = binaryExpr->left->Accept(this);
        llvm::Value* right = binaryExpr->right->Accept(this);
        llvm::Value *value = irBuilder.CreateICmpNE(left, right);
        return irBuilder.CreateIntCast(value, irBuilder.getInt32Ty(), true);
    }
    case OpCode::less:{
        llvm::Value* left = binaryExpr->left->Accept(this);
        llvm::Value* right = binaryExpr->right->Accept(this);
        llvm::Value *value = irBuilder.CreateICmpSLT(left, right);
        return irBuilder.CreateIntCast(value, irBuilder.getInt32Ty(), true);
    }
    case OpCode::less_equal:{
        llvm::Value* left = binaryExpr->left->Accept(this);
        llvm::Value* right = binaryExpr->right->Accept(this);
        llvm::Value *value = irBuilder.CreateICmpSLE(left, right);
        return irBuilder.CreateIntCast(value, irBuilder.getInt32Ty(), true);
    }
    case OpCode::greater:{
        llvm::Value* left = binaryExpr->left->Accept(this);
        llvm::Value* right = binaryExpr->right->Accept(this);
        llvm::Value *value = irBuilder.CreateICmpSGT(left, right);
        return irBuilder.CreateIntCast(value, irBuilder.getInt32Ty(), true);
    }
    case OpCode::greater_equal:{
        llvm::Value* left = binaryExpr->left->Accept(this);
        llvm::Value* right = binaryExpr->right->Accept(this);
        llvm::Value *value = irBuilder.CreateICmpSGE(left, right);
        return irBuilder.CreateIntCast(value, irBuilder.getInt32Ty(), true);
    }
    case OpCode::logAnd:{
        // A && B
        llvm::BasicBlock* nextBB = llvm::BasicBlock::Create(context, "nextBB", curFunc);
        llvm::BasicBlock* falseBB = llvm::BasicBlock::Create(context, "falseBB", curFunc);
        llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(context, "mergeBB", curFunc);

        llvm::Value* left = binaryExpr->left->Accept(this);
        llvm::Value* lval = irBuilder.CreateICmpNE(left, irBuilder.getInt32(0));
        irBuilder.CreateCondBr(lval, nextBB, falseBB);

        //-----------------------------------------------------------------------------------------
        irBuilder.SetInsertPoint(nextBB);
        /// 右子树内部也生成了基本块
        llvm::Value* right = binaryExpr->right->Accept(this);
        // LLVM IR的比较指令（icmp）返回结果是 1 位（i1 类型），而不是 32 位。
        right = irBuilder.CreateICmpNE(right, irBuilder.getInt32(0));
        /*
        IRBuilder::CreateZExt函数用于生成IR的zext零扩展指令，零扩展是指将较小位宽的整数
        类型（如i1或i8）扩展为较大位宽的整数类型（如i32或 i64），通过在高位填充 0 来保持值的无符号语义
        */
        right = irBuilder.CreateZExt(right, irBuilder.getInt32Ty());
        irBuilder.CreateBr(mergeBB);

        // 拿到当前插入的block,对nextBB指针重新赋值， 建立一个值和基本块的关系 {right, nextBB}
        nextBB = irBuilder.GetInsertBlock();


        //-----------------------------------------------------------------------------------------
        irBuilder.SetInsertPoint(falseBB);
        irBuilder.CreateBr(mergeBB);

        //-----------------------------------------------------------------------------------------
        irBuilder.SetInsertPoint(mergeBB);
        /*
        PHI指令用于在控制流图CFG中处理值合并。它出现在基本块Basic Block的开头，当一个基本块有多个前驱
        predecessor基本块时，PHI节点从每个前驱选择合适的值，确保 SSA形式：每个变量只有一次赋值。
        */
        llvm::PHINode *phi = irBuilder.CreatePHI(irBuilder.getInt32Ty(), 2);
        phi->addIncoming(right, nextBB);
        phi->addIncoming(irBuilder.getInt32(0), falseBB);
        return phi;
    }
    case OpCode::logOr:{
        // A || B
        llvm::BasicBlock* nextBB = llvm::BasicBlock::Create(context, "nextBB", curFunc);
        llvm::BasicBlock* trueBB = llvm::BasicBlock::Create(context, "trueBB", curFunc);
        llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(context, "mergeBB", curFunc);

        llvm::Value* left = binaryExpr->left->Accept(this);
        llvm::Value* lval = irBuilder.CreateICmpNE(left, irBuilder.getInt32(0));
        irBuilder.CreateCondBr(lval, trueBB, nextBB);

        //-----------------------------------------------------------------------------------------
        irBuilder.SetInsertPoint(nextBB);
        /// 右子树内部也生成了基本块
        llvm::Value* right = binaryExpr->right->Accept(this);
        right = irBuilder.CreateICmpNE(right, irBuilder.getInt32(0));
        right = irBuilder.CreateZExt(right, irBuilder.getInt32Ty());
        irBuilder.CreateBr(mergeBB);
        /// right 这个值，所在的基本块，并不一定是 之前的nextBB了.
        /// 原因是：binaryExpr->right->Accept(this) 内部会生成新的基本块

        /// 拿到当前插入的block, 建立一个值和基本块的关系 {right, nextBB}
        nextBB = irBuilder.GetInsertBlock();

        //-----------------------------------------------------------------------------------------
        irBuilder.SetInsertPoint(trueBB);
        irBuilder.CreateBr(mergeBB);

        //-----------------------------------------------------------------------------------------
        irBuilder.SetInsertPoint(mergeBB);
        llvm::PHINode *phi = irBuilder.CreatePHI(irBuilder.getInt32Ty(), 2);
        phi->addIncoming(right, nextBB);
        phi->addIncoming(irBuilder.getInt32(1), trueBB);
        return phi;
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