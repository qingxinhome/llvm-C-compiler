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

    // 注：verifyModule函数： 如果检查module有错误则返回true。 检查module正确则返回false
    if (verifyModule(*module, &llvm::outs())) {
        module->print(outs(), nullptr);
    }
    
#if 0
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
#endif

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

llvm::Value* CodeGen::VisitVariableDeclExpr(VariableDecl *decl) {
    llvm::Type* ty = decl->type->Accept(this);

    llvm::StringRef varName(decl->token.ptr, decl->token.len);
    llvm::Value* value = irBuilder.CreateAlloca(ty, nullptr, varName);
    varAddrTypeMap.insert({varName, {value, ty}});

    if (decl->init != nullptr) {
        llvm::Value *initValue = decl->init->Accept(this);
        irBuilder.CreateStore(initValue, value);
    }
    return value;
}

// 变量访问返回的应该是一个右值
// CreateLoad返回的是一个右值
llvm::Value* CodeGen::VisitVariableAccessExpr(VariableAccessExpr *expr) {
    llvm::StringRef varName(expr->token.ptr, expr->token.len);
    std::pair pair = varAddrTypeMap[varName];
    llvm::Value* addr = pair.first;
    llvm::Type* ty = pair.second;
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



/*
注：
1. 在llvm IR中一切指令都是值llvm::Value
2. 
*/