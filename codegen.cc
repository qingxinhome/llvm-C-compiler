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

llvm::Value* CodeGen::VisitProgram(Program *program) {
    for (const auto &decl : program->externalDecls)
    {
        decl->Accept(this);
    }
    return nullptr;
}
    


#if 0
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

    llvm::Value* lastValue = program->node->Accept(this);
    // if (lastValue != nullptr) {
    //     irBuilder.CreateCall(printf, {irBuilder.CreateGlobalStringPtr("expr value: [%d]\n"), lastValue});
    // } else {
    //     irBuilder.CreateCall(printf, {irBuilder.CreateGlobalStringPtr("last inst is not expr\n")});
    // }

    /* 
      在llvm IR中一切指令都是值llvm::Value, llvm::Value是一切的基类
      CreateRet返回的类型是llvm::ReturnInst，但llvm::ReturnInst是llvm::Value的派生类 
    */
    // llvm::Value* ret = irBuilder.CreateRet(irBuilder.getInt32(0));
    llvm::Value* ret = irBuilder.CreateRet(lastValue);

    verifyFunction(*mFunc);

    // 注：verifyModule函数：如果检查module有错误则返回true。 检查module正确则返回false
    if (verifyModule(*module, &llvm::outs())) {
        module->print(outs(), nullptr);
    }
    

    if (lastValue != nullptr) {
        irBuilder.CreateCall(printf, {irBuilder.CreateGlobalStringPtr("expr value: [%d]\n"), lastValue});
    } else {
        irBuilder.CreateCall(printf, {irBuilder.CreateGlobalStringPtr("last inst is not expr\n")});
    }
    /* 
      在llvm IR中一切指令都是值llvm::Value, llvm::Value是一切的基类
      CreateRet返回的类型是llvm::ReturnInst，但llvm::ReturnInst是llvm::Value的派生类 
    */
    llvm::Value* ret = irBuilder.CreateRet(irBuilder.getInt32(0));

    verifyFunction(*mFunc);
    module->print(outs(), nullptr);
    return ret;
}
#endif

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

llvm::Value* CodeGen::VisitForStmt(ForStmt *forStmt) {
    llvm::BasicBlock *initBB = llvm::BasicBlock::Create(context, "for.init", curFunc);
    llvm::BasicBlock *condBB = llvm::BasicBlock::Create(context, "for.cond");
    llvm::BasicBlock *incBB = llvm::BasicBlock::Create(context, "for.inc");
    llvm::BasicBlock *bodyBB = llvm::BasicBlock::Create(context, "for.body");
    llvm::BasicBlock *lastBB = llvm::BasicBlock::Create(context, "for.last");

    // 将for循环语句中可以被break跳转到的block块记录在breakBBs Map中
    breakBBs.insert({forStmt, lastBB});
    // 将for循环语句中可以被continue跳转到的block块记录在breakBBs Map中
    continueBBs.insert({forStmt, incBB});

    // 1. 先无条件跳转到init块中
    irBuilder.CreateBr(initBB);
    irBuilder.SetInsertPoint(initBB);
    if (forStmt->initNode != nullptr) {
        forStmt->initNode->Accept(this);
    }
    // 由初始块无条件跳转至条件块
    irBuilder.CreateBr(condBB);

    // 将block块插入到当前函数
    condBB->insertInto(curFunc);
    irBuilder.SetInsertPoint(condBB);
    if (forStmt->condNode != nullptr) {
        llvm::Value *val = forStmt->condNode->Accept(this);
        llvm::Value *condVal = irBuilder.CreateICmpNE(val, irBuilder.getInt32(0));
        irBuilder.CreateCondBr(condVal, bodyBB, lastBB);
    } else {
         irBuilder.CreateBr(bodyBB);
    }


    bodyBB->insertInto(curFunc);
    irBuilder.SetInsertPoint(bodyBB);
    if (forStmt->bodyNode != nullptr) {
        forStmt->bodyNode->Accept(this);
    }
    irBuilder.CreateBr(incBB);


    incBB->insertInto(curFunc);
    irBuilder.SetInsertPoint(incBB);
    if (forStmt->incNode != nullptr) {
        forStmt->incNode->Accept(this);
    }
    irBuilder.CreateBr(condBB);


    lastBB->insertInto(curFunc);
    irBuilder.SetInsertPoint(lastBB);

    breakBBs.erase(forStmt);
    continueBBs.erase(forStmt);

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


llvm::Value* CodeGen::VisitReturnStmt(ReturnStmt *stmt) {
    // return语句就是产生一条IR的return 指令
    if (stmt->expr != nullptr) {
        llvm::Value *val= stmt->expr->Accept(this);
        return irBuilder.CreateRet(val);
    } else {
        //如果没有返回值表达式， 返回一个空的return指令
        return irBuilder.CreateRetVoid();
    }
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
    llvm::Value* left = nullptr;
    llvm::Value* right = nullptr;
    if (binaryExpr->op != BinaryOp::logic_and && binaryExpr->op != BinaryOp::logic_or) {
        left = binaryExpr->left->Accept(this);
        right = binaryExpr->right->Accept(this);
    }
    switch (binaryExpr->op)
    {
    case BinaryOp::add:{
        /* 加法和减法需要考虑指针类型 */
        llvm::Type *ty = binaryExpr->left->type->Accept(this);
        if (ty->isPointerTy()) {
            // GEP指令用于计算指针的偏移地址
            llvm::Value* newVal = irBuilder.CreateInBoundsGEP(ty, left, {right});
            return newVal;
        } else {
            return irBuilder.CreateNSWAdd(left, right);
        }
    }
    case BinaryOp::sub:{
        /* 加法和减法需要考虑指针类型 */
        llvm::Type *ty = binaryExpr->left->type->Accept(this);
        if (ty->isPointerTy()) {
            // GEP指令用于计算指针的偏移地址
            llvm::Value* newVal = irBuilder.CreateInBoundsGEP(ty, left, {irBuilder.CreateNeg(right)});
            return newVal;
        } else {
            return irBuilder.CreateNSWSub(left, right);
        }
    }
    case BinaryOp::mul:{
        return irBuilder.CreateNSWMul(left, right);
    }
    case BinaryOp::div:{
        return irBuilder.CreateSDiv(left, right);
    }
    case BinaryOp::mod:{
        return irBuilder.CreateSRem(left, right);
    }
    case BinaryOp::bitwise_or: {
        return irBuilder.CreateOr(left, right);
    }
    case BinaryOp::bitwise_and: {
        return irBuilder.CreateAnd(left, right);
    }
    case BinaryOp::bitwise_xor: {
        return irBuilder.CreateXor(left, right);
    }
    case BinaryOp::left_shift: {
        return irBuilder.CreateShl(left, right);
    }
    case BinaryOp::right_shift: {
        return irBuilder.CreateAShr(left, right);
    }
    case BinaryOp::equal:{
        // 比较运算返回的结果是一个1byte整形值， 返回后无法与后来的int32表达式做运算, 需要做类型转换
        llvm::Value *value = irBuilder.CreateICmpEQ(left, right);
        return irBuilder.CreateIntCast(value, irBuilder.getInt32Ty(), true);
    }
    case BinaryOp::not_equal:{
        llvm::Value *value = irBuilder.CreateICmpNE(left, right);
        return irBuilder.CreateIntCast(value, irBuilder.getInt32Ty(), true);
    }
    case BinaryOp::less:{
        llvm::Value *value = irBuilder.CreateICmpSLT(left, right);
        return irBuilder.CreateIntCast(value, irBuilder.getInt32Ty(), true);
    }
    case BinaryOp::less_equal:{
        llvm::Value *value = irBuilder.CreateICmpSLE(left, right);
        return irBuilder.CreateIntCast(value, irBuilder.getInt32Ty(), true);
    }
    case BinaryOp::greater:{
        llvm::Value *value = irBuilder.CreateICmpSGT(left, right);
        return irBuilder.CreateIntCast(value, irBuilder.getInt32Ty(), true);
    }
    case BinaryOp::greater_equal:{
        llvm::Value *value = irBuilder.CreateICmpSGE(left, right);
        return irBuilder.CreateIntCast(value, irBuilder.getInt32Ty(), true);
    }
    case BinaryOp::logic_and:{
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
    case BinaryOp::logic_or:{
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
    
    case BinaryOp::assign:{
        // 赋值运算的左表达式必须是一个load指令， 先做强转
        llvm::LoadInst *load = llvm::dyn_cast<llvm::LoadInst>(left);
        assert(load);   // 保证不为空

        
        /* 
         注：load指令相当于对指针解引用后的值， 而load->getPointerOperand()，相当于对变量取地址
         LoadInst.getPointerOperand(): 返回load 指令的指针操作数(operand)，即从中加载值的内存地址 
        */
        // load 指令对应的是一个右值， 需要获取其原始边变量地址;
        irBuilder.CreateStore(right, load->getPointerOperand());
        
        // 赋值操作返回的结果是右表达式的值
        return right;
    }
    case BinaryOp::add_assign:{
        // 同样的，赋值运算的左表达式必须是一个load指令， 先做强转
        llvm::LoadInst *load = llvm::dyn_cast<llvm::LoadInst>(left);
        assert(load);

        llvm::Type *ty = binaryExpr->left->type->Accept(this);
        if (ty->isPointerTy()) {
            // GEP指令用于计算指针的偏移地址
            llvm::Value* newVal = irBuilder.CreateInBoundsGEP(ty, left, {right});
            irBuilder.CreateStore(newVal, load->getPointerOperand());
            return newVal;
        } else {
            // a+=3  => a = a + 3
            llvm::Value *tmp = irBuilder.CreateAdd(left, right);
            // load指令是一个右值， 需要先获取指针操作数
            irBuilder.CreateStore(tmp, load->getPointerOperand());
            return tmp;
        }
    }
    case BinaryOp::sub_assign:{
        llvm::LoadInst *load = llvm::dyn_cast<llvm::LoadInst>(left);
        assert(load);

        llvm::Type *ty = binaryExpr->left->type->Accept(this);
        if (ty->isPointerTy()) {
            // GEP指令用于计算指针的偏移地址
            llvm::Value* newVal = irBuilder.CreateInBoundsGEP(ty, left, {irBuilder.CreateNeg(right)});
            irBuilder.CreateStore(newVal, load->getPointerOperand());
            return newVal;
        } else {
            // a-=3  => a = a - 3
            llvm::Value *tmp = irBuilder.CreateSub(left, right);
            // load指令是一个右值， 需要先获取指针操作数
            irBuilder.CreateStore(tmp, load->getPointerOperand());
            return tmp;
        }
    }
    case BinaryOp::mul_assign:{
        llvm::LoadInst *load = llvm::dyn_cast<llvm::LoadInst>(left);
        assert(load);

        // a*=3  => a = a * 3
        llvm::Value *tmp = irBuilder.CreateMul(left, right);
        // load指令是一个右值， 需要先获取指针操作数
        irBuilder.CreateStore(tmp, load->getPointerOperand());
        return tmp;
    }
    case BinaryOp::div_assign:{
        llvm::LoadInst *load = llvm::dyn_cast<llvm::LoadInst>(left);
        assert(load);

        // a/=3  => a = a / 3
        llvm::Value *tmp = irBuilder.CreateSDiv(left, right);
        // load指令是一个右值， 需要先获取指针操作数
        irBuilder.CreateStore(tmp, load->getPointerOperand());
        return tmp;
    }
    case BinaryOp::mod_assign:{
        llvm::LoadInst *load = llvm::dyn_cast<llvm::LoadInst>(left);
        assert(load);

        // a%=3  => a = a % 3
        llvm::Value *tmp = irBuilder.CreateSRem(left, right);
        // load指令是一个右值， 需要先获取指针操作数
        irBuilder.CreateStore(tmp, load->getPointerOperand());
        return tmp;
    }
    case BinaryOp::bitwise_and_assign:{
        llvm::LoadInst *load = llvm::dyn_cast<llvm::LoadInst>(left);
        assert(load);

        // a &=3  => a = a & 3
        llvm::Value *tmp = irBuilder.CreateAnd(left, right);
        // load指令是一个右值， 需要先获取指针操作数
        irBuilder.CreateStore(tmp, load->getPointerOperand());
        return tmp;
    }
    case BinaryOp::bitwise_or_assign:{
        llvm::LoadInst *load = llvm::dyn_cast<llvm::LoadInst>(left);
        assert(load);

        // a |=3  => a = a | 3
        llvm::Value *tmp = irBuilder.CreateOr(left, right);
        // load指令是一个右值， 需要先获取指针操作数
        irBuilder.CreateStore(tmp, load->getPointerOperand());
        return tmp;
    }
    case BinaryOp::bitwise_xor_assign:{
        llvm::LoadInst *load = llvm::dyn_cast<llvm::LoadInst>(left);
        assert(load);

        // a ^=3  => a = a ^ 3
        llvm::Value *tmp = irBuilder.CreateXor(left, right);
        // load指令是一个右值， 需要先获取指针操作数
        irBuilder.CreateStore(tmp, load->getPointerOperand());
        return tmp;
    }
    case BinaryOp::left_shift_assign:{
        llvm::LoadInst *load = llvm::dyn_cast<llvm::LoadInst>(left);
        assert(load);

        // a ^=3  => a = a ^ 3
        llvm::Value *tmp = irBuilder.CreateShl(left, right);
        // load指令是一个右值， 需要先获取指针操作数
        irBuilder.CreateStore(tmp, load->getPointerOperand());
        return tmp;
    }
    case BinaryOp::right_shift_assign:{
        llvm::LoadInst *load = llvm::dyn_cast<llvm::LoadInst>(left);
        assert(load);

        // a ^=3  => a = a ^ 3
        llvm::Value *tmp = irBuilder.CreateAShr(left, right);
        // load指令是一个右值， 需要先获取指针操作数
        irBuilder.CreateStore(tmp, load->getPointerOperand());
        return tmp;
    }
    default:
        break;
    }
    return nullptr;
}

llvm::Value* CodeGen::VisitThreeExpr(ThreeExpr *expr) {
    // 三目运算其实就是 if else 
    llvm::BasicBlock *thenBB = llvm::BasicBlock::Create(context, "then", curFunc);
    llvm::BasicBlock *elseBB = llvm::BasicBlock::Create(context, "els");
    llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(context, "merge");

    // 1. 计算条件表达式
    llvm::Value *val = expr->cond->Accept(this);
    llvm::Value *cond = irBuilder.CreateICmpNE(val, irBuilder.getInt32(0));
    irBuilder.CreateCondBr(cond, thenBB, elseBB);

    // 2. 计算then表达式
    irBuilder.SetInsertPoint(thenBB);
    llvm::Value *thenVal = expr->then->Accept(this);
    /*对三目运算的then表达式执行完codegen之后，当前的block块可能已经不是thenBB了，需要更新当前的block块*/
    thenBB = irBuilder.GetInsertBlock();
    irBuilder.CreateBr(mergeBB);


    // 3. 计算else表达式
    elseBB->insertInto(curFunc);          /* 将当前的block块插入到目标函数中*/
    irBuilder.SetInsertPoint(elseBB);
    llvm::Value *elseVal = expr->els->Accept(this);
    // 更新当前插入指令的基本块
    elseBB = irBuilder.GetInsertBlock();
    irBuilder.CreateBr(mergeBB);

    // 4. 操作合并后的block块
    mergeBB->insertInto(curFunc);         /* 将当前的block块插入到目标函数中*/
    irBuilder.SetInsertPoint(mergeBB);
    
    // phi指令代表有多种可能性的情况
    llvm::PHINode* phi = irBuilder.CreatePHI(expr->then->type->Accept(this), 2);
    phi->addIncoming(thenVal, thenBB);
    phi->addIncoming(elseVal, elseBB);
    return phi;
}

llvm::Value* CodeGen::VisitUnaryExpr(UnaryExpr *unaryExpr) {
    llvm::Value *val = unaryExpr->node->Accept(this);
    llvm::Type *ty = unaryExpr->node->type->Accept(this);

    switch (unaryExpr->op) {
    case UnaryOp::positive:
        return val;
    case UnaryOp::negative:
        return irBuilder.CreateNeg(val);  /* 数学取反指令*/
    case UnaryOp::logic_not:{
        llvm::Value *tmp = irBuilder.CreateICmpNE(val, irBuilder.getInt32(0));
        // 比较运算的返回值类型是 int1 (1 bit), 需要扩展为int32
        return irBuilder.CreateZExt(irBuilder.CreateNot(tmp), irBuilder.getInt32Ty());
    }
    case UnaryOp::bitwise_not:{
        return irBuilder.CreateNot(val);
    }
    case UnaryOp::addr:{
        return llvm::dyn_cast<LoadInst>(val)->getPointerOperand();
    }
    case UnaryOp::deref:{
        // 指针的解引用操作， 需要先获取当前这个一元表达式的类型
        // 如： int *p = &a;  *p = a; 那么一元表达式*p的类型就是int
        llvm:Type *nodeType = unaryExpr->type->Accept(this);
        return irBuilder.CreateLoad(nodeType, val);
    }
    case UnaryOp::inc:{
        // ++p;  ==> p = p + 1
        if (ty->isPointerTy()) {
            // GEP指令用于计算指针的偏移地址，也即是说 val其实就是指针的地址
            llvm::Value * newVal = irBuilder.CreateInBoundsGEP(ty, val, {irBuilder.getInt32(1)});
            irBuilder.CreateStore(newVal, llvm::dyn_cast<llvm::LoadInst>(val)->getPointerOperand());
            return newVal;
        } else if (ty->isIntegerTy()) {
            // 如果要获取原始变量的地址(指针操作数):llvm::dyn_cast<llvm::LoadInst>(val)->getPointerOperand()
            llvm::Value *newVal = irBuilder.CreateAdd(val, irBuilder.getInt32(1));
            irBuilder.CreateStore(newVal, llvm::dyn_cast<llvm::LoadInst>(val)->getPointerOperand());
            return newVal;
        } else {
            assert(0);
            return nullptr;
        }
    }
    case UnaryOp::dec:{
        // --p;  ==> p = p - 1
        if (ty->isPointerTy()) {
            // GEP指令用于计算指针的偏移地址，也即是说 val其实就是指针的地址
            llvm::Value * newVal = irBuilder.CreateInBoundsGEP(ty, val, {irBuilder.getInt32(-1)});
            irBuilder.CreateStore(newVal, llvm::dyn_cast<llvm::LoadInst>(val)->getPointerOperand());
            return newVal;
        } else if (ty->isIntegerTy()) {
            // 如果要获取原始变量的地址(指针操作数):llvm::dyn_cast<llvm::LoadInst>(val)->getPointerOperand()
            llvm::Value *newVal = irBuilder.CreateSub(val, irBuilder.getInt32(1));
            irBuilder.CreateStore(newVal, llvm::dyn_cast<llvm::LoadInst>(val)->getPointerOperand());
            return newVal;
        } else {
            assert(0);
            return nullptr;
        }
    }
    default:
        break;
    }
    return nullptr;
}

llvm::Value* CodeGen::VisitSizeOfExpr(SizeOfExpr *sizeofExpr) {
    llvm::Type *ty = nullptr;
    if (sizeofExpr->ty != nullptr) {
        ty = sizeofExpr->ty->Accept(this);
    } else {
        // sizeof expr； 需要获取表达式的类型
        ty = sizeofExpr->node->type->Accept(this);
    }
    if (ty->isPointerTy()) {
        return irBuilder.getInt32(8);    /* 指针变量的长度为8 */
    } else if (ty->isIntegerTy()) {
        return irBuilder.getInt32(4);    /* 整形变量的长度为4 */
    } else {
        assert(0);
        return nullptr;
    }
}

llvm::Value* CodeGen::VisitPostIncExpr(PostIncExpr *postIncExpr) {
    // p++; => p = p + 1;
    llvm::Value *val = postIncExpr->left->Accept(this);
    llvm::Type *ty = postIncExpr->left->type->Accept(this);

    if (ty->isPointerTy()) {
        // int *p; p++;  ==> p = p + 1
        // CreateInBoundsGEP(Type *Ty, Value *Ptr, ArrayRef<Value*> IdxList);Ty是基类型，表示指针Ptr指向的对象的数据类型
        // GEP指令用于计算指针的偏移地址，也即是说 val其实就是指针的地址
        llvm::Value * newVal = irBuilder.CreateInBoundsGEP(ty, val, {irBuilder.getInt32(1)});
        irBuilder.CreateStore(newVal, llvm::dyn_cast<llvm::LoadInst>(val)->getPointerOperand());
        return val;
    } else if (ty->isIntegerTy()) {
        // 注意： 这里的val其实并不是原始的变量， 而是通过load指令从原始变量中取出值的临时变量, 
        // 如果要获取原始变量的地址(指针操作数)，需要执行：llvm::dyn_cast<llvm::LoadInst>(val)->getPointerOperand()
        llvm::Value *newVal = irBuilder.CreateAdd(val, irBuilder.getInt32(1));
        irBuilder.CreateStore(newVal, llvm::dyn_cast<llvm::LoadInst>(val)->getPointerOperand());
        return val;
    } else {
        assert(0);
        return nullptr;
    }
}


llvm::Value* CodeGen::VisitPostDecExpr(PostDecExpr *postDecExpr) {
    // p--; => p = p - 1;
    llvm::Value *val = postDecExpr->left->Accept(this);
    llvm::Type *ty = postDecExpr->left->type->Accept(this);

    if (ty->isPointerTy()) {
        // int *p; p--;  ==> p = p - 1
        llvm::Value * newVal = irBuilder.CreateInBoundsGEP(ty, val, {irBuilder.getInt32(-1)});
        irBuilder.CreateStore(newVal, llvm::dyn_cast<llvm::LoadInst>(val)->getPointerOperand());
        return val;
    } else if (ty->isIntegerTy()) {
        llvm::Value *newVal = irBuilder.CreateSub(val, irBuilder.getInt32(1));
        irBuilder.CreateStore(newVal, llvm::dyn_cast<llvm::LoadInst>(val)->getPointerOperand());
        return val;
    } else {
        assert(0);
        return nullptr;
    }
}


llvm::Value* CodeGen::VisitNumberExpr(NumberExpr *numberExpr){
    return irBuilder.getInt32(numberExpr->token.value);
}


// 数组元素访问表达式的codegen本质上就是对数组元素地址的计算
// a[5] = 10;
llvm::Value* CodeGen::VisitPostSubscript(PostSubscript *expr) {
    llvm::Type * ty = expr->type->Accept(this);
    llvm::Value *left = expr->left->Accept(this);    /* 数组基地址*/
    llvm::Value *offset = expr->node->Accept(this);  /* 偏移地址*/

    /* 
    left对应的是load指令(被取值了)，load指令是对地址被解引用后的值， 可以通过LoadInst.getPointerrOperand()获取原始地址
    CreateInBoundsGEP(llvm::Type *Ty, llvm::Value *Ptr, llvm::ArrayRef<llvm::Value *> IdxList)
    如： int *p；ty就是int，   Ptr就是p（指针）
    */
    llvm::Value *addr = irBuilder.CreateInBoundsGEP(ty, llvm::dyn_cast<LoadInst>(left)->getPointerOperand(), {offset});
    return irBuilder.CreateLoad(ty, addr);
}

// 其实就是求结构体成员的地址
// a.b , a -> T (a 类型就是T)
llvm::Value* CodeGen::VisitPostMemberDotExpr(PostMemberDotExpr *expr) {
    // leftValue就是结构体变量本身，注意：它不是结构体变量的地址， 因为它是被load出来的，相当于做了解引用， T *a -> T
    llvm::Value *leftValue = expr->left->Accept(this); 
    llvm::Type *leftType = expr->left->type->Accept(this);  // 结构体的类型
    
    // 获取结构体的C语言类型
    CRecordType *cRecordType = llvm::dyn_cast<CRecordType>(expr->left->type.get());
    TagKind tagKind = cRecordType->GetTagKind();
    
    // 计算结构体元素的类型
    llvm::Type* memType = expr->member.ty->Accept(this);
    if (tagKind == TagKind::KStruct) {
        llvm::Value *val0 = irBuilder.getInt32(0);
        llvm::Value *next = irBuilder.getInt32(expr->member.elemIdx);

        // llvm::Value *CreateInBoundsGEP(llvm::Type *Ty, llvm::Value *Ptr, ...)
        // CreateInBoundsGEP的ty参数是结构体的类型， Ptr是结构体变量的指针
        llvm::Value* memAddr = irBuilder.CreateInBoundsGEP(leftType, llvm::dyn_cast<LoadInst>(leftValue)->getPointerOperand(), {val0, next}); 
        return irBuilder.CreateLoad(memType, memAddr);
    } else {
        llvm::Value *val0 = irBuilder.getInt32(0);
        llvm::Value* memAddr = irBuilder.CreateInBoundsGEP(leftType, llvm::dyn_cast<LoadInst>(leftValue)->getPointerOperand(), {val0, val0}); 
        
        // BitCast的作用是：将一个值的类型由源类型转换为另一个类型，而不改变数据的二进制表示，只改变编译器对该数据值的类型解释。
        llvm::Value *cast = irBuilder.CreateBitCast(memAddr, llvm::PointerType::getUnqual(memType));
        return irBuilder.CreateLoad(memType, cast);
    }
}

// a->b
// ptr* -> ptr
llvm::Value* CodeGen::VisitPostMemberArrowExpr(PostMemberArrowExpr *expr) {
    // 由于是指针选择选择运算符(->), 因此leftValue就是结构体变量的地址
    llvm::Value *leftValue = expr->left->Accept(this); 

    CPointType *cRecordTypePtr = llvm::dyn_cast<CPointType>(expr->left->type.get());
    llvm::Type *leftType = cRecordTypePtr->GetBaseType()->Accept(this);  // 由结构体指针获取结构体的类型
    
    // 获取结构体的C语言类型
    CRecordType *cRecordType = llvm::dyn_cast<CRecordType>(cRecordTypePtr->GetBaseType().get());
    TagKind tagKind = cRecordType->GetTagKind();
    
    // 计算结构体元素的类型
    llvm::Type* memType = expr->member.ty->Accept(this);
    if (tagKind == TagKind::KStruct) {
        llvm::Value *val0 = irBuilder.getInt32(0);
        llvm::Value *next = irBuilder.getInt32(expr->member.elemIdx);

        // llvm::Value *CreateInBoundsGEP(llvm::Type *Ty, llvm::Value *Ptr, ...)
        // CreateInBoundsGEP的ty参数是结构体的类型， Ptr是结构体变量的指针
        llvm::Value* memAddr = irBuilder.CreateInBoundsGEP(leftType, leftValue, {val0, next}); 
        return irBuilder.CreateLoad(memType, memAddr);
    } else {
        llvm::Value *val0 = irBuilder.getInt32(0);
        llvm::Value* memAddr = irBuilder.CreateInBoundsGEP(leftType, leftValue, {val0, val0}); 
        
        // BitCast的作用是：将一个值的类型由源类型转换为另一个类型，而不改变数据的二进制表示，只改变编译器对该数据值的类型解释。
        llvm::Value *cast = irBuilder.CreateBitCast(memAddr, llvm::PointerType::getUnqual(memType));
        return irBuilder.CreateLoad(memType, cast);
    }
}

 llvm::Value* CodeGen::VisitPostFunctionCallExpr(PostFunctionCallExpr *expr) {
    // 计算函数表达式， 求出函数的地址
    llvm::Value *funcAddr = expr->left->Accept(this);

    // 获取函数类型
    llvm::FunctionType *funcTy = llvm::dyn_cast<llvm::FunctionType>(expr->left->type->Accept(this));

    // 对函数调用表达式的实参做Codegen
    llvm::SmallVector<llvm::Value*> args;
    for (auto &arg : expr->args) {
        args.push_back(arg->Accept(this));
    }
    return irBuilder.CreateCall(funcTy, funcAddr, args);
 }

// 变量的声明要区分全局变量和局部变量
llvm::Value* CodeGen::VisitVariableDeclExpr(VariableDecl *decl) {
    llvm::Type* ty = decl->type->Accept(this);
    llvm::StringRef varName(decl->token.ptr, decl->token.len);

    if (decl->isGlobal) {
        // 定义一个lambda表达式， 用于获取全局变量定义时 给的初始值
        auto GetInitValueByOffset = [&](std::vector<int> &offset) -> std::shared_ptr<VariableDecl::InitValue>{
            const auto &initVals = decl->initValues;
            for (auto &n : initVals) {
                if (n->offsetList.size() != offset.size()) {
                    continue;
                }

                bool find = true;
                for (int i = 0; i < offset.size(); i++) {
                    if (offset[i] != n->offsetList[i]) {
                        find = false;
                        break;
                    }
                }

                if (find) {
                    return n;
                }
            }
            return nullptr;
        };

        
        // 定义一个lambda表达式(相当于一个递归函数), 用于计算全局变量的初始值（初始值都是常量），采用递归回溯法
        auto GetInitialValue = [&](llvm::Type* ty, auto &&initFunc, std::vector<int> offset)->llvm::Constant * {
            if (ty->isIntegerTy()) {
                // 全局变量为整数类型(递归)

                // 如果初始值不为空，则返回初始值的常量
                std::shared_ptr<VariableDecl::InitValue> initVal = GetInitValueByOffset(offset);
                if (initVal != nullptr) {
                    return llvm::dyn_cast<llvm::Constant>(initVal->value->Accept(this));
                }
                // 如果没有初始值，则它的默认初始值为0
                return irBuilder.getInt32(0);
            } else if (ty->isPointerTy()) {
                // 全局变量为指针类型，则它的默认初始值为空的常量指针（全局指针变量的初始值只能是空值）
                return llvm::ConstantPointerNull::get(llvm::dyn_cast<llvm::PointerType>(ty));
            } else if (ty->isStructTy()) {
                // 全局变量为结构体类型(递归)， 
                llvm::StructType *structTy = llvm::dyn_cast<llvm::StructType>(ty);

                // elemsConstantValue 用于存储结构体成员的常量初始值
                llvm::SmallVector<llvm::Constant *> elemsConstantVal;  
                int size = structTy->getStructNumElements();     /* 通过llvm::StructType获取结构体元素个数 */
                for (int i = 0; i < size; i++)
                {
                    offset.push_back(i);
                    // 对结构体成员进行递归初始化
                    auto elemConstVal = initFunc(structTy->getStructElementType(i), initFunc, offset);
                    elemsConstantVal.push_back(elemConstVal);
                    offset.pop_back();
                }
                
                // 结构体的初始值， 是一个llvm结构体常量llvm::ConstantStruct
                return llvm::ConstantStruct::get(structTy, elemsConstantVal);
            } else if (ty->isArrayTy()) {
                // 全局变量为数组类型(递归)
                llvm::ArrayType *arrayTy = llvm::dyn_cast<llvm::ArrayType>(ty);

                llvm::SmallVector<llvm::Constant*> elemsConstantVal;
                int size = arrayTy->getArrayNumElements();    /* 获取数组类型的元素个数 */
                for (int i = 0; i < size; i++)
                {
                    offset.push_back(i);
                    // 对数组元素进行递归初始化
                    auto elemConstVal = initFunc(arrayTy->getArrayElementType(), initFunc, offset);
                    elemsConstantVal.push_back(elemConstVal);
                    offset.pop_back();
                }

                // 数组的初始值， 是一个llvm数组常量llvm::ConstantStruct
                return llvm::ConstantArray::get(arrayTy, elemsConstantVal);
            } else {
                // TODO: 不支持的类型， 如果未来类型扩展在此处补充
            }
        };


        // 在llvm api中全局变量是由单独的指令去创建， new GlobalVariable(...), 原型如下：
        // GlobalVariable(Module &M,    全局变量所属的module
        //         Type *Ty,            全局变量的类型
        //         bool isConstant,     是否为常量
        //         LinkageTypes Linkage,  链接属性， 可以设置为外部链接属性
        //         Constant *Initializer, const Twine &Name = "",  全局变量的初始值和名字
        //         GlobalVariable *InsertBefore = nullptr,      
        //         ThreadLocalMode = NotThreadLocal,
        //         std::optional<unsigned> AddressSpace = std::nullopt,
        //         bool isExternallyInitialized = false);
        llvm::GlobalVariable *globalVar = new llvm::GlobalVariable(*module, ty, false, llvm::GlobalValue::ExternalLinkage, nullptr);
        globalVar->setAlignment(llvm::Align(decl->type->GetAlign()));          /* 设置变量内存对齐 */ 
        globalVar->setInitializer(GetInitialValue(ty, GetInitialValue, {0}));  /* 设置全局变量初始值 */ 

        AddGlobalVarToMap(globalVar, ty, varName);
        return globalVar;
    } else {
        /* 
        在LLVM IR中，alloca指令必须放在函数的entry block(入口基本块)中:
        ┌─────────────────────────┐
        │     entry block         │ ← 所有 alloca 必须在这里！
        │  %x = alloca i32        │
        │  %y = alloca [10 x i32] │
        └─────────────────────────┘
                ↓
        其他基本块（不能 alloca）
        */
        // curFunc->getEntryBlock() 获取函数的入口基本快
        // curFunc->getEntryBlock().begin() 函数的入口基本块的插入位置迭代器
        // 创建一个临时irBuilder
        llvm::IRBuilder<> tmpIrBuilder(&curFunc->getEntryBlock(), curFunc->getEntryBlock().begin());

        llvm::AllocaInst* alloc = irBuilder.CreateAlloca(ty, nullptr, varName);    /* 为变量申请内存地址 */
        alloc->setAlignment(llvm::Align(decl->type->GetAlign()));                  /* 设置变量内存对齐 */

        // 存储变量的 '地址'-'类型' 的KV
        AddLocalVarToMap(alloc, ty, varName);

        if (decl->initValues.size() > 0) {
            if (decl->initValues.size() == 1) {
                llvm::Value *initValue = decl->initValues[0]->value->Accept(this);
                irBuilder.CreateStore(initValue, alloc);
            } else {
                // 处理数组声明的初始化
                if (llvm::ArrayType *arrType = llvm::dyn_cast<llvm::ArrayType>(ty)) {
                    for (const auto &initValue : decl->initValues) {
                        llvm::SmallVector<llvm::Value*> vec;
                        for (auto &offset : initValue->offsetList) {
                            vec.push_back(irBuilder.getInt32(offset));
                        }
                        llvm::Value *addr = irBuilder.CreateInBoundsGEP(ty, alloc, vec);
                        llvm::Value *val = initValue->value->Accept(this);
                        irBuilder.CreateStore(val, addr);
                    }
                } 
                // 处理struct/union声明的初始化
                else if (llvm::StructType *structType = llvm::dyn_cast<StructType>(ty)) {
                    // 获取对应C语言类型信息
                    CRecordType *cStructType = llvm::dyn_cast<CRecordType>(decl->type.get());
                    TagKind tagKind = cStructType->GetTagKind();
                    if (tagKind == TagKind::KStruct) {
                        for (const auto &initValue : decl->initValues) {
                            llvm::SmallVector<llvm::Value*> vec;
                            for (auto &offset : initValue->offsetList) {
                                vec.push_back(irBuilder.getInt32(offset));
                            }
                            llvm::Value *addr = irBuilder.CreateInBoundsGEP(ty, alloc, vec);
                            llvm::Value *val = initValue->value->Accept(this);
                            irBuilder.CreateStore(val, addr);
                        }
                    } else {
                        // union
                        assert(decl->initValues.size() == 1);

                        llvm::SmallVector<llvm::Value*> vec;
                        const auto& initValue = decl->initValues[0];
                        for (auto &offset : initValue->offsetList) {
                            vec.push_back(irBuilder.getInt32(offset));
                        }
                        llvm::Value *addr = irBuilder.CreateInBoundsGEP(ty, alloc, vec);
                        llvm::Value *val = initValue->value->Accept(this);
                        irBuilder.CreateStore(val, addr);
                    }
                } else {
                    assert(0);
                }
            }
        }
        return alloc;
    }
    // if (decl->init != nullptr) {
    //     llvm::Value *initValue = decl->init->Accept(this);
    //     irBuilder.CreateStore(initValue, varValue);
    // }
    // return value;
}

llvm::Value* CodeGen::VisitFunctionDeclExpr(FunctionDecl *decl) {
    ClearVarScope();  /* 初始化函数的局部变量作用域 */ 

    // 获取函数的C语言语法树类型
    CFuncType *cFuncTy = llvm::dyn_cast<CFuncType>(decl->type.get());

    FunctionType *funcTy = llvm::dyn_cast<llvm::FunctionType>(decl->type->Accept(this));
    // ExternalLinkage 外部链接属性可以被连接器找到
    Function *mFunc = Function::Create(funcTy, GlobalValue::LinkageTypes::ExternalLinkage, cFuncTy->GetName(), module.get());
    // 将函数的地址和类型添加到全局作用域中
    AddGlobalVarToMap(mFunc, funcTy, cFuncTy->GetName());

    const auto &params = cFuncTy->GetParams();
    int i = 0;
    // 设置 llvm::Function的参数名
    for (auto &arg : mFunc->args()) {
        arg.setName(params[i].name);
        i++;
    }

    if (decl->blockStmt == nullptr) {
        return nullptr;
    }

    // 创建函数的入口块
    /* 函数的入口块entry要用于存放变量内存的分配 */
    BasicBlock *entryBB = BasicBlock::Create(context, "entry", mFunc);
    irBuilder.SetInsertPoint(entryBB);

    // 记录当前的Function函数
    curFunc = mFunc;

    // >>>> 进入一个函数，就要开辟一个新的作用域
    PushScope();

    i = 0;
    for (auto &arg : mFunc->args()) {
        // 在入口块entry中为函数参数申请一个变量
        auto* alloc = irBuilder.CreateAlloca(arg.getType(), nullptr, params[i].name);
        alloc->setAlignment(llvm::Align(params[i].type->GetAlign()));
        // 将函数的传入的参数存入到alloc变量中
        irBuilder.CreateStore(&arg, alloc);

        // 将参数存入局部变量作用域中
        AddLocalVarToMap(alloc, arg.getType(), params[i].name);
        i++;
    }
    // 函数体的codegen
    decl->blockStmt->Accept(this);
    // <<<< 离开一个函数时，就要退出当前作用域
    PopScope();

    verifyFunction(*mFunc);
    // 注：verifyModule函数：如果检查module有错误则返回true。 检查module正确则返回false
    if (verifyModule(*module, &llvm::outs())) {
        module->print(outs(), nullptr);
    }
    return nullptr;
}

// '变量访问' 返回的应该是一个右值
// CreateLoad返回的是一个右值
// 变量访问节点，就是通过变量的指针解引用，获取变量的值，所以返回的结果不是变量的指针，而是变量的值本身
// alloc T -> T *
// load T* -> T
llvm::Value* CodeGen::VisitVariableAccessExpr(VariableAccessExpr *expr) {
    llvm::StringRef varName(expr->token.ptr, expr->token.len);

    const auto &[addr, ty] = GetVarByName(varName);
    if (ty->isFunctionTy()) {
        // 如果是函数类型，则直接返回函数地址
        return addr;
    }

    // 变量访问 对应一个load指令
    return irBuilder.CreateLoad(ty, addr, varName);
}


llvm::Type* CodeGen::VisitPrimaryType(CPrimaryType *type) {
    if (type->GetKind() == CType::TY_Int) {
        return irBuilder.getInt32Ty();
    }
    assert(0);
    return nullptr;
}

llvm::Type* CodeGen::VisitPointType(CPointType *type) {
    llvm::Type *baseType = type->GetBaseType()->Accept(this);
    // 创建指向指定元素类型的指针类型（PointerType），地址空间固定为默认值 0（通用地址空间）
    return llvm::PointerType::getUnqual(baseType);
}

llvm::Type* CodeGen::VisitArrayType(CArrayType *type) {
    llvm::Type *elemType = type->GetElementType()->Accept(this);
    return llvm::ArrayType::get(elemType, type->GetElementCount());
}


llvm::Type* CodeGen::VisitRecordType(CRecordType *type) {
    // 判断llvm IR 的结构体类型是在其符号表中否已经存在
    llvm::StructType *structType = llvm::StructType::getTypeByName(context, type->GetName());
    if (structType != nullptr) {
        return structType;
    }
    structType = llvm::StructType::create(context, type->GetName());

    TagKind tagKind = type->GetTagKind();
    if (tagKind == TagKind::KStruct) {
        llvm::SmallVector<llvm::Type*> vec;
        for (auto &mbr : type->GetMembers()) {
            vec.push_back(mbr.ty->Accept(this));
        }
        structType->setBody(vec);
    } else {
        llvm::SmallVector<llvm::Type*> vec;
        const auto &memebers = type->GetMembers();
        int idx = type->GetMaxElementIdx();

        vec.push_back(memebers[idx].ty->Accept(this));
        structType->setBody(vec);
    }

    // 打印类型
    // structType->print(llvm::outs());
    return structType;
}


llvm::Type* CodeGen::VisitFuncType(CFuncType *type) {
    llvm::Type *retTy = type->GetRetType()->Accept(this);

    llvm::SmallVector<llvm::Type*> paramsType;
    for(const auto &param : type->GetParams()) {
        paramsType.push_back(param.type->Accept(this));
    }
    // C函数类型就返回一个llvm的FunctionType
    return llvm::FunctionType::get(retTy, paramsType, false);
}



void CodeGen::AddLocalVarToMap(llvm::Value* addr, llvm::Type* ty, llvm::StringRef name) {
    localVarMap.back().insert(std::make_pair(name, std::make_pair(addr, ty)));
}

void CodeGen::AddGlobalVarToMap(llvm::Value* addr, llvm::Type* ty, llvm::StringRef name) {
    globalVarMap.insert(std::make_pair(name, std::make_pair(addr, ty)));
}

std::pair<llvm::Value*, llvm::Type*> CodeGen::GetVarByName(llvm::StringRef name) {
    // 先从局部变量作用域的栈底查找
    for(auto it = localVarMap.rbegin(); it != localVarMap.rend(); it++) {
        // if (it->find(name) != it->end()) {
        //     return (*it)[name];
        // }
        auto ret = it->find(name);
        if (ret != it->end()) {
            return ret->second;
        }
    }
    // 从全局变量作用域中查找
    assert(globalVarMap.find(name) != globalVarMap.end());
    return globalVarMap[name];
}

void CodeGen::PushScope() {
    localVarMap.emplace_back();
}

void CodeGen::PopScope() {
    localVarMap.pop_back();
}

void CodeGen::ClearVarScope() {
    // 每次进入一个llvm IR Function， 都需要将作用域（localVarMap）清理一次
    localVarMap.clear();
}


/*
注：
1. 在llvm IR中一切指令都是值llvm::Value
2. 在llvm中类型的基类是 llvm::Type, llvm::Type中有一个print函数，可用于打印当前类型的ir
3. 在 LLVM IR 中，alloca 指令必须放在函数的 entry basic block（入口基本块）中。这是 LLVM 语言
   规范（Language Reference）明确规定的规则，不能违反，否则 IR 将被视为 ill-formed（格式错误），后续的 opt、llc 或 clang 等工
   具可能会报错或产生未定义行为。
*/