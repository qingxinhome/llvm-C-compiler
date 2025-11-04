#include "sema.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Casting.h"

std::shared_ptr<VariableDecl> Sema::semaVariableDeclNode(Token token, std::shared_ptr<CType> type, bool isGloabl) {
    // 1. 检查在当前作用域内是否存在重名
    llvm::StringRef text(token.ptr, token.len);
    auto symbol = scope.FindObjSymbolInCurScope(text);
    if (symbol && (mode == Mode::Normal)) {
        // llvm::errs() << "re define variable name " << text << "\n";
        diagEngine.Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_redefined, text);
    }

    if (this->mode == Mode::Normal) {
        // 2. 添加到符号表中
        scope.AddObjSymbol(type, text);
    }

    std::shared_ptr<VariableDecl> variableDecl = std::make_shared<VariableDecl>();
    variableDecl->token = token;
    variableDecl->type = type;
    // 指针和普通类型的变量都是左值
    variableDecl->isLValue = true;
    variableDecl->isGlobal = isGloabl;
    return variableDecl;
}


std::shared_ptr<VariableAccessExpr> Sema::semaVariableAccessNode(Token token) {
    // 从符号表栈的所有符号表中检查符号是否存在
    llvm::StringRef text(token.ptr, token.len);
    std::shared_ptr<Symbol> symbol = scope.FindObjSymbol(text);
    if(symbol == nullptr && (this->mode == Mode::Normal)) {
        // llvm::errs() << "use undefined symbol " << text << "\n";
        diagEngine.Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_undefined, text);
    }
    std::shared_ptr<VariableAccessExpr> expr = std::make_shared<VariableAccessExpr>();
    expr->token = token;
    expr->type = symbol->GetType();
    expr->isLValue = true;
    return expr;
}


std::shared_ptr<BinaryExpr> Sema::semaBinaryExprNode(std::shared_ptr<AstNode> left, std::shared_ptr<AstNode> right, BinaryOp op){
    auto binaryExpr = std::make_shared<BinaryExpr>();
    binaryExpr->op = op;
    binaryExpr->left = left;
    binaryExpr->right = right;

    // 二元表达式运算完后就是右值
    // binaryExpr->isLValue = false;

    // 正常情况下， 二元表达式的类型就是左节点或有节点的类型
    binaryExpr->type = left->type;

    // 指针类型能够进行 + 、- 、+=、-= 运算
    if (op == BinaryOp::add || op == BinaryOp::sub || op == BinaryOp::add_assign || op == BinaryOp::sub_assign) {
        // int a = 5; int *p = &a, 3+p; 这种情况要取右节点的类型
        if ((left->type->GetKind() == CType::TY_Int) && (right->type->GetKind() == CType::TY_Point)) {
            binaryExpr->type = right->type;
        }
    }

    return binaryExpr;
}


std::shared_ptr<UnaryExpr> Sema::semaUnaryExprNode(std::shared_ptr<AstNode> unary, UnaryOp op, Token token) {
    auto unaryExpr = std::make_shared<UnaryExpr>();
    unaryExpr->op = op;
    unaryExpr->node = unary;

    // 一般情况下：一元运算结束后，会变成右值（解引用运算除外）
    switch (op)
    {
    case UnaryOp::positive:
    case UnaryOp::negative:
    case UnaryOp::logic_not:
    case UnaryOp::bitwise_not:
    {
        // 一元加，一元减，逻辑非，按位取反都只能针对数值类型,因此当前节点的类型就是子节点的类型
        if (unary->type->GetKind() != CType::TY_Int && (this->mode == Mode::Normal)) {
            // 一元加，一元减，逻辑非，按位取反 只能针对基本类型
            diagEngine.Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_expected_type, "int type");
        }
        unaryExpr->type = unary->type;
        break;
    } 
    case UnaryOp::addr:
    {
        // int a;  int *p = &a;
        // if (!unary->isLValue && (this->mode == Mode::Normal)) {
        //     // 要求只能针对左值进行取地址
        //     diagEngine.Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_expected_lvalue);
        // }
        unaryExpr->type = std::make_shared<CPointType>(unary->type);
        break;
    }
    case UnaryOp::deref:
    {
        // int *p = a;  *p = 100;
        // 解引用只能针对指针类型， 需要进行语义检查
        if (unary->type->GetKind() != CType::TY_Point && (this->mode == Mode::Normal)) {
            diagEngine.Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_expected_type, "pointer type");
        }
        // if (!unary->isLValue) {
        //     // 要求只能针对左值进行解引用？ 未必，比如：int *p = &a; *(p+1)
        //     diagEngine.Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_expected_lvalue);
        // }

        CPointType *pty = llvm::dyn_cast<CPointType>(unary->type.get());
        unaryExpr->type = pty->GetBaseType();
        unaryExpr->isLValue = true;
        break;
    }
    case UnaryOp::inc:
    case UnaryOp::dec:
    {
        if (!unary->isLValue && (this->mode == Mode::Normal)) {
            diagEngine.Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_expected_lvalue);
        }
        // 指针和变量都可以 ++、--
        unaryExpr->type = unary->type;
        break;
    }
    default:
        break;
    }
    return unaryExpr;
}

std::shared_ptr<ThreeExpr> Sema::semaThreeExprNode(std::shared_ptr<AstNode> cond, std::shared_ptr<AstNode> then, std::shared_ptr<AstNode> els, Token token) {
    std::shared_ptr<ThreeExpr> threeExpr = std::make_shared<ThreeExpr>();
    threeExpr->cond = cond;
    threeExpr->then = then;
    threeExpr->els = els;

    if (then->type->GetKind() != els->type->GetKind() && (this->mode == Mode::Normal)) {
        diagEngine.Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_same_type);
    }

    threeExpr->type = then->type;
    return threeExpr;
}

std::shared_ptr<SizeOfExpr> Sema::semaSizeOfExprNode(std::shared_ptr<AstNode> unary, std::shared_ptr<CType> type) {
    auto sizeOfExpr = std::make_shared<SizeOfExpr>();
    sizeOfExpr->node = unary;
    sizeOfExpr->ty = type;
    // sizeof的类型是int
    sizeOfExpr->type = CType::IntType;
    return sizeOfExpr;
}

// eg: a++
std::shared_ptr<PostIncExpr> Sema::semaPostIncExprNode(std::shared_ptr<AstNode> left, Token token) {
    if (!left->isLValue && (this->mode == Mode::Normal)) {
        diagEngine.Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_expected_lvalue);
    }
    auto node = std::make_shared<PostIncExpr>();
    node->left = left;
    node->type = left->type;
    return node;
}

// eg: a--
std::shared_ptr<PostDecExpr> Sema::semaPostDecExprNode(std::shared_ptr<AstNode> left, Token token) {
    if (!left->isLValue && (this->mode == Mode::Normal)) {
        diagEngine.Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_expected_lvalue);
    }
    auto node = std::make_shared<PostDecExpr>();
    node->left = left;
    node->type = left->type;
    return node;
}

// eg: a[3]
// a[3]; -> *(a + offset(3 * elementSize))
std::shared_ptr<PostSubscript> Sema::semaPostSubscriptNode(std::shared_ptr<AstNode> left, std::shared_ptr<AstNode> node, Token curtoken) {
    if (left->type->GetKind() != CType::TY_Array && left->type->GetKind() != CType::TY_Point && (this->mode == Mode::Normal)) {
        diagEngine.Report(llvm::SMLoc::getFromPointer(curtoken.ptr), diag::err_expected_type, "array or point");
    }

    auto postSubscriptNode = std::make_shared<PostSubscript>();
    postSubscriptNode->left = left;
    postSubscriptNode->node = node;

    if (left->type->GetKind() == CType::TY_Array) {
        // 如果 a[3]中a是数组类型，那么数组访问表达式的类型就是数组元素类型
        CArrayType *arrayType = llvm::dyn_cast<CArrayType>(left->type.get());
        postSubscriptNode->type = arrayType->GetElementType();
    } else if (left->type->GetKind() == CType::TY_Point) {
        // 如果 p[3]中p是指针类型，如 int *p，那么数组访问表达式的类型就是指针的基类型
        CPointType *pointType = llvm::dyn_cast<CPointType>(left->type.get());
        postSubscriptNode->type = pointType->GetBaseType();
    }
    return postSubscriptNode;
}

std::shared_ptr<PostMemberDotExpr> Sema::semaPostMemberDotNode(std::shared_ptr<AstNode> left, Token identoken, Token dotToekn) {
    if (left->type->GetKind() != CType::TY_Record && (mode == Mode::Normal)) {
        diagEngine.Report(llvm::SMLoc::getFromPointer(dotToekn.ptr), diag::err_expected_type, "struct or union type");
    }

    CRecordType *recordType = llvm::dyn_cast<CRecordType>(left->type.get());
    
    bool found = false;
    Member curMember;
    for(const auto &member : recordType->GetMembers()) {
        if (member.name == llvm::StringRef(identoken.ptr, identoken.len)) {
            found = true;
            curMember = member;
            break;
        }
    }
    
    if (!found) {
        diagEngine.Report(llvm::SMLoc::getFromPointer(identoken.ptr), diag::err_miss, "struct or union miss field");
    }

    auto node = std::make_shared<PostMemberDotExpr>();
    node->token = dotToekn;
    node->type = curMember.ty;
    node->left = left;
    node->member = curMember;
    return node;
}

std::shared_ptr<PostMemberArrowExpr> Sema::semaPostMemberArrowNode(std::shared_ptr<AstNode> left, Token identoken, Token arrowToken) {
    if (left->type->GetKind() != CType::TY_Point) {
        diagEngine.Report(llvm::SMLoc::getFromPointer(arrowToken.ptr), diag::err_expected_type, "pointer type");
    }
    CPointType *pointerType = llvm::dyn_cast<CPointType>(left->type.get());

    auto baseType = pointerType->GetBaseType();
    if (baseType->GetKind() != CType::TY_Record) {
        diagEngine.Report(llvm::SMLoc::getFromPointer(arrowToken.ptr), diag::err_expected_type, "struct or union pointer type");
    }
    CRecordType *recordType = llvm::dyn_cast<CRecordType>(baseType.get());
    
    bool found = false;
    Member curMember;
    for(const auto &member : recordType->GetMembers()) {
        if (member.name == llvm::StringRef(identoken.ptr, identoken.len)) {
            found = true;
            curMember = member;
            break;
        }
    }
    
    if (!found) {
        diagEngine.Report(llvm::SMLoc::getFromPointer(identoken.ptr), diag::err_miss, "struct or union miss field");
    }

    auto node = std::make_shared<PostMemberArrowExpr>();
    node->token = arrowToken;
    node->type = curMember.ty;
    node->left = left;
    node->member = curMember;
    return node;
}

// semaDeclInitValue 对变量声明语句的初始值做语义检查，检查声明的类型和初始化值的类型是否匹配
std::shared_ptr<VariableDecl::InitValue> Sema::semaDeclInitValue(std::shared_ptr<CType> declType, std::shared_ptr<AstNode> value, std::vector<int> &offsetList, Token token) {
    /*
    暂时注释掉类型检查，用于支持类型强转
        // 检查声明的类型和初始化值的类型是否匹配
        if (declType->GetKind() != value->type->GetKind() && (this->mode == Mode::Normal)) {
            diagEngine.Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_miss, "same type");
        }
    */
      
    auto initValue = std::make_shared<VariableDecl::InitValue>();
    initValue->decType = declType;
    initValue->value = value;
    initValue->offsetList = offsetList;
    return initValue;
}



std::shared_ptr<NumberExpr> Sema::semaNumberExprNode(Token token, std::shared_ptr<CType> type) {
    std::shared_ptr<NumberExpr> expr = std::make_shared<NumberExpr>();
    expr->token = token;
    expr->type = type;
    return expr;
}


// IfStmt 节点没有类型， 因为它不能被赋值， 也不能赋值给其他人
std::shared_ptr<IfStmt> Sema::semaIfStmtNode(std::shared_ptr<AstNode> condNode, std::shared_ptr<AstNode> thenNode, std::shared_ptr<AstNode> elseNode) {
    std::shared_ptr<IfStmt> ifStmt = std::make_shared<IfStmt>();
    ifStmt->condNode = condNode;
    ifStmt->thenNode = thenNode;
    ifStmt->elseNode = elseNode;
    return ifStmt;
}


void Sema::EnterScope() {
    scope.EnterScope();
}

void Sema::ExitScope() {
    scope.ExitScope();
}

void Sema::SetMode(Mode mode) {
    this->mode = mode;
}




// 从类型符号表栈中查找 结构体名是否存在，如果存在就返回对应结构体类型
std::shared_ptr<CType> Sema::semaTagAccess(Token token) {
    // 从符号表栈的所有符号表中检查符号是否存在
    llvm::StringRef text(token.ptr, token.len);
    std::shared_ptr<Symbol> symbol = scope.FindTagSymbol(text);
    if (symbol != nullptr) {
        return symbol->GetType();
    } else {
        return nullptr;
    }

    // if(symbol == nullptr && (this->mode == Mode::Normal)) {
    //     diagEngine.Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_undefined, text);
    // }
    // return symbol->GetType();
}

std::shared_ptr<CType> Sema::semaTagDeclare(Token token, const std::vector<Member> &members, TagKind tagKind) {
    // 1. 检查类型名tag在当前作用域内是否存在重名
    llvm::StringRef text(token.ptr, token.len);
    auto symbol = scope.FindTagSymbolInCurScope(text);
    if (symbol != nullptr) {
        diagEngine.Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_redefined, text);
    }

    auto recordType = std::make_shared<CRecordType>(text, members, tagKind);
    if (this->mode == Mode::Normal) {
        // 2. 添加到符号表中
        scope.AddTagSymbol(recordType, text);
    }
    return recordType;
}


std::shared_ptr<CType> Sema::semaTagDeclare(Token token, std::shared_ptr<CType> type) {
    // 检查类型名tag在当前作用域内是否存在重名
    llvm::StringRef text(token.ptr, token.len);
    auto symbol = scope.FindTagSymbolInCurScope(text);
    if (symbol != nullptr) {
        diagEngine.Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_redefined, text);
    }

    // 2. 添加到tag符号表中
    if (this->mode == Mode::Normal) {
        scope.AddTagSymbol(type, text);
    }
    return type;
}


std::shared_ptr<CType> Sema::semaAnonyTagDeclare(const std::vector<Member> &members, TagKind tagKind) {
    llvm::StringRef text = CType::GenAnonyRecordName(tagKind);
    auto recordType = std::make_shared<CRecordType>(text, members, tagKind);
    if (this->mode == Mode::Normal) {
        // 2. 添加到符号表中
        scope.AddTagSymbol(recordType, text);
    }
    return recordType;
}


std::shared_ptr<FunctionDecl> Sema::semaFunctionDecl(Token token, std::shared_ptr<CType> type, std::shared_ptr<AstNode> blockStmt) {
    CFuncType *funcType = llvm::dyn_cast<CFuncType>(type.get());
    funcType->hasBody = blockStmt ? true : false;
    
    // 检查函数是否出现冲定义
    llvm::StringRef text(token.ptr, token.len);
    std::shared_ptr<Symbol> symbol = scope.FindObjSymbolInCurScope(text);

    if (symbol != nullptr) {
        auto symbolType = symbol->GetType();
        if (symbolType->GetKind() != CType::TY_Func && mode == Mode::Normal) {
            // 函数名和全局变量名重名，报错
            diagEngine.Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_redefined, text);
        }

        // 函数可以重复声明， 但不能重复定义
        if (symbolType->GetKind() == CType::TY_Func) {
            auto symFuncType = llvm::dyn_cast<CFuncType>(symbolType.get());
            if (symFuncType->hasBody && funcType->hasBody && mode == Mode::Normal) {
                diagEngine.Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_redefined, text);
            }
        }
    }

    if((symbol == nullptr || funcType->hasBody) && mode == Mode::Normal) {
        // 添加到符号表中
        scope.AddObjSymbol(type, text);
    }
    
    std::shared_ptr<FunctionDecl> funcDecl = std::make_shared<FunctionDecl>();
    funcDecl->token = token;     // 记录函数名对应的token
    funcDecl->blockStmt = blockStmt;
    funcDecl->type = type;
    return funcDecl;
}


std::shared_ptr<PostFunctionCallExpr> Sema::semaFuncCallExprNode(std::shared_ptr<AstNode> left, std::vector<std::shared_ptr<AstNode>> &args) {
    Token ident = left->token;
    if (left->type->GetKind() != CType::TY_Func && (mode == Mode::Normal)) {
        // 符号不是函数类型
        diagEngine.Report(llvm::SMLoc::getFromPointer(ident.ptr), diag::err_expected, "function type");
    }
    CFuncType *funcType = llvm::dyn_cast<CFuncType>(left->type.get());

    // 检查函数实参数个数与形参个数是否匹配
    if (funcType->GetParams().size() != args.size() && (mode == Mode::Normal)) {
        diagEngine.Report(llvm::SMLoc::getFromPointer(ident.ptr), diag::err_miss, "argument count not match");
    }

    auto funcCall = std::make_shared<PostFunctionCallExpr>();
    funcCall->type = funcType->GetRetType();
    funcCall->left = left;
    funcCall->args = args;
    funcCall->token = left->token;
    return funcCall;
}
