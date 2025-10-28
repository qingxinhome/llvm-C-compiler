#include "parser.h"

/*
prog : stmt*
stmt : decl-stmt | expr-stmt | null-stmt | if-stmt | block-stmt
null-stmt : ";"
decl-stmt : "int" identifier ("," identifier (= expr)?)* ";"
if-stmt : "if" "(" expr ")" stmt ( "else" stmt )?
block-stmt: "{" stmt* "}"
expr-stmt : expr ";"
expr : assign-expr | add-expr
assign-expr: identifier "=" expr
add-expr : mult-expr (("+" | "-") mult-expr)* 
mult-expr : primary-expr (("*" | "/") primary-expr)* 
primary-expr : identifier | number | "(" expr ")" 
number: ([0-9])+ 
identifier : (a-zA-Z_)(a-zA-Z0-9_)*
*/
std::shared_ptr<Program> Parser::ParseProgram(){
    // while (true) .... exit ?
    // token -> eof
    // std::vector<std::shared_ptr<AstNode>> nodeVec;
    // while(token.tokenType != TokenType::eof) {
    //     auto stmt = ParseStmt();
    //     if (stmt == nullptr) {
    //         continue;
    //     }
    //     nodeVec.push_back(stmt);
    // }

    auto program = std::make_shared<Program>();
    if (token.tokenType != TokenType::eof) {
        auto stmt = ParseBlockStmt();
        program->node = stmt;
    }
    Expect(TokenType::eof);
    return program;
}

std::shared_ptr<AstNode> Parser::ParseStmt() {
    // null statement
    if (token.tokenType == TokenType::semi) {
        NextToken();
        return nullptr;
    }
    // declare statment
    else if (IsTypeName(token.tokenType)) {
        return ParseDeclareStmt();
    }
    // if statement
    else if (token.tokenType == TokenType::kw_if) {
        return ParseIfStmt();
    }
    // block statment
    else if (token.tokenType == TokenType::l_brace) {
        return ParseBlockStmt();
    }
    // for statement
    else if (token.tokenType == TokenType::kw_for) {
        return ParseForStmt();
    }
    // break statement
    else if (token.tokenType == TokenType::kw_break) {
        return ParseBreakStmt();
    }
    // continue statement
    else if (token.tokenType == TokenType::kw_continue) {
        return ParseContinueStmt();
    }
    // expr statement
    else {
        return ParseExprStmt();
    }   
}

// 处理声明语句中的基础类型，如：int *p; 中的int
std::shared_ptr<CType> Parser::ParseDeclSpec() {
    if (token.tokenType == TokenType::kw_int) {
        Consume(TokenType::kw_int);
        return CType::IntType;
    } else if (token.tokenType == TokenType::kw_struct 
        || token.tokenType == TokenType::kw_union) {
        return ParseStructOrUnionSpec();
    }
    GetDiagEngine().Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_type);
    return nullptr;
}

/*
decl-spec  ::= "int" | struct-or-union-specifier
struct-or-union-specifier ::= struct-or-union identifier "{" (decl-spec declarator(, declarator)* ";")+ "}"
														  struct-or-union identifier
struct-or-union := "struct" | "union"
*/
std::shared_ptr<CType> Parser::ParseStructOrUnionSpec() {
    TagKind tagkind;
    if (token.tokenType == TokenType::kw_struct) {
        tagkind = TagKind::KStruct;
    } else if (token.tokenType == TokenType::kw_union) {
        tagkind = TagKind::KUnion;
    } else {
        assert(0);
    }
    NextToken();

    bool anony = false;
    Token tag;
    if (token.tokenType == TokenType::l_brace) {
        anony = true;
    } else {
        Expect(TokenType::identifier);
        /* 记录结构体类型名.如 struct A {...} 中的A */
        tag = token;                 
        Consume(TokenType::identifier);
    }

    /*
        struct A {int a,b; int *p}  // 定义结构体
        struct A           // 使用一个结构体， 如 struct A[20];
    */
    if (token.tokenType == TokenType::l_brace) {
        Consume(TokenType::l_brace);
        // 结构体定义{...}也是一个新的作用域
        sema.EnterScope();

        std::vector<Member> members;
        while(token.tokenType != TokenType::r_brace) {
            auto node = ParseDeclareStmt();
            DeclareStmt *declstmt = llvm::dyn_cast<DeclareStmt>(node.get());
            for (auto &n : declstmt->nodeVec) {
                Member mbr;
                mbr.ty = n->type;
                mbr.name = llvm::StringRef(n->token.ptr, n->token.len);
                members.push_back(mbr);
            }
        }
        sema.ExitScope();
        Consume(TokenType::r_brace);
        if (anony) {
            return sema.semaAnonyTagDeclare(members, tagkind);
        } else {
            return sema.semaTagDeclare(tag, members, tagkind);
        }
    } else {
        return sema.semaTagAccess(tag);
    }

    return nullptr;
}

/*
解析类型声明符后缀；如:
int a[2][3][4] 数组类型中的[2][3][4]
int a(int，float) 函数类型中的(int，float) 
*/
std::shared_ptr<CType> Parser::DirectDeclaratorSuffix(std::shared_ptr<CType> baseType) {
    if (token.tokenType == TokenType::l_bracket) {
        // 数组声明
        return DirectDeclaratorArraySuffix(baseType);
    }
    // 函数声明
    return baseType;
}

/* 
解析数组类型声明的数组后缀， 如：int a[2][3][4] 中的[2][3][4]
direct-declarator ::= identifier | "(" declarator ")" | direct-declarator "[" assign "]"
eg: int a[3][4];
    baseType -> int
*/
std::shared_ptr<CType> Parser::DirectDeclaratorArraySuffix(std::shared_ptr<CType> baseType) {
    if (token.tokenType != TokenType::l_bracket) { /* 递归退出条件 */
        return baseType;
    }

    Consume(TokenType::l_bracket);
    Expect(TokenType::number);
    int elemCount = token.value;
    Consume(TokenType::number);
    Consume(TokenType::r_bracket);
    return std::make_shared<CArrayType>(DirectDeclaratorArraySuffix(baseType), elemCount);
}

// declarator ::= "*"* direct-declarator
// direct-declarator ::= identifier | "(" declarator ")" | direct-declarator "[" assign "]"
std::shared_ptr<AstNode> Parser::DirectDeclarator(std::shared_ptr<CType> baseType) {
    std::shared_ptr<AstNode> varDeclNode;
    if (this->token.tokenType == TokenType::l_parent) {
        // 处理数组指针的语法： 如：int (*p)[5] p是一个指向int [5]数组的指针
        Token curToken = token;

        //--------------------------------------------------------------------
        lexer.SaveState();              /* 保存当前状态 */
        sema.SetMode(Sema::Mode::Skip);

        Consume(TokenType::l_parent);
        Declarator(CType::IntType);     /* CType::IntType只是临时占位类型，没有实际意义 */
        Consume(TokenType::r_parent);

        // 解析类型声明符后缀， 获取完整类型
        baseType = DirectDeclaratorSuffix(baseType);
        lexer.RestoreState();           /* 恢复当前状态 */
        sema.SetMode(Sema::Mode::Normal);
        //--------------------------------------------------------------------

        token = curToken;
        Consume(TokenType::l_parent);
        varDeclNode = Declarator(baseType);
        Consume(TokenType::r_parent);

        /* CType::IntType是临时占位类型，没有实际意义，调用DirectDeclaratorSuffix是为了让token走到 = 符号 */
        DirectDeclaratorSuffix(CType::IntType);
    } else if (token.tokenType == TokenType::identifier){
        Token ident = token;                 /* 记录变量名 */
        Consume(TokenType::identifier);

        // 解析类型声明符后缀, 如： a[2][3][4][5]， 或 a(int, char)
        baseType = DirectDeclaratorSuffix(baseType);
        varDeclNode = sema.semaVariableDeclNode(ident, baseType);
    } else {
        GetDiagEngine().Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_expected_ex, "identifer or '('");
    }
    
    if (token.tokenType == TokenType::equal) {
        NextToken();
        // variableDecl->init = ParseAssignExpr();
        std::vector<int> offsetList;
        VariableDecl *variableDecl = llvm::dyn_cast<VariableDecl>(varDeclNode.get());
        ParseInitializer(variableDecl->initValues, variableDecl->type, offsetList, false);
    }
    return varDeclNode;
}

/*
initializer ::= assign | "{" initializer ("," initializer)*  "}"
arr(引用参数)
declType  代表在解析`{初始化列表}` 时， 当前这个初始化列表对应的数组类型 
offsetList (引用参数): 元素索引列表
返回值bool： 标识初始化列表是否结束
*/
bool Parser::ParseInitializer(std::vector<std::shared_ptr<VariableDecl::InitValue>> &arr, 
    std::shared_ptr<CType> declType, 
    std::vector<int> &offsetList,
    bool hasLBrace
) {
    if (token.tokenType == TokenType::r_brace) {
        if (!hasLBrace) {
            GetDiagEngine().Report(llvm::SMLoc::getFromPointer(token.ptr),  diag::err_miss, "{");
        }
        return true;
    }
    
    // {,,}
    if (token.tokenType == TokenType::l_brace) {
        Consume(TokenType::l_brace);

        if (declType->GetKind() == CType::TY_Array) {
            CArrayType *arrType = llvm::dyn_cast<CArrayType>(declType.get());
            int size = arrType->GetElementCount();

            int isEnd = false;
            // int arr[10] = {1,2,3};
            for (int i = 0; i < size; i++) {
                if (i > 0 && token.tokenType == TokenType::comma) {
                    Consume(TokenType::comma);
                } 

                offsetList.push_back(i);
                isEnd = ParseInitializer(arr, arrType->GetElementType(), offsetList, true);
                offsetList.pop_back();
                if (isEnd) {
                    break;
                }
            }
        }
        Consume(TokenType::r_brace);
    } else {
        Token tmpToken = token;
        // assign expression
        auto node = ParseAssignExpr();

        auto initValue = sema.semaDeclInitValue(declType, node, offsetList, token);
        arr.push_back(initValue);
    }
    return false;
}



/*
declarator ::= "*"* direct-declarator
direct-declarator ::= identifier | direct-declarator "[" assign "]"
*/
// declarator (= expr)
std::shared_ptr<AstNode> Parser::Declarator(std::shared_ptr<CType> baseType) {
    // 处理指针符号 *
    while(token.tokenType == TokenType::star) {
        Consume(TokenType::star);
        // PointType
        baseType = std::make_shared<CPointType>(baseType);
    }
    return DirectDeclarator(baseType);
}



// 解析变量声明语句
std::shared_ptr<AstNode> Parser::ParseDeclareStmt() {
    // 获取基础类型
    std::shared_ptr<CType> baseTy = ParseDeclSpec();

    // 处理无意义的声明， 形如： int ; 
    if (token.tokenType == TokenType::semi) {
        Consume(TokenType::semi);
        return nullptr;
    }

    auto declareStmt = std::make_shared<DeclareStmt>();
    // int a , b = 3;
    // int a = 3;
    // int aa = 1, b = 1;
    int i = 0;
    while(token.tokenType != TokenType::semi) {
        // 如果是第二次进入循环体，判断是否为逗号，
        if (i++ > 0) {
            Consume(TokenType::comma);
        }
        auto variableDecl = Declarator(baseTy);
        declareStmt->nodeVec.push_back(variableDecl);
        /*
        assert(token.tokenType == TokenType::identifier);

        // 获取变量名
        // auto variableName = token.content;
        Token tmpToken = token;  // int a

        std::shared_ptr<VariableDecl> variableDecl = sema.semaVariableDeclNode(token, baseTy);
        declareStmt->nodeVec.push_back(variableDecl);

        Consume(TokenType::identifier);
        if (token.tokenType == TokenType::equal) {
            auto opToken = token;
            NextToken();

            std::shared_ptr<VariableAccessExpr> left = sema.semaVariableAccessNode(tmpToken);
            auto right = ParseExpr();
            
            std::shared_ptr<AssignExpr> assignExpr = sema.semaAssignExprNode(left, right, opToken);
            declareStmt->nodeVec.push_back(assignExpr);
        }
        */
    }
    Consume(TokenType::semi);
    return declareStmt;
}


std::shared_ptr<AstNode> Parser::ParseExprStmt() {
    std::shared_ptr<AstNode> expr = ParseExpr();
    Consume(TokenType::semi);
    return expr;
}

// if-stmt : "if" "(" expr ")" stmt ( "else" stmt )?
std::shared_ptr<AstNode> Parser::ParseIfStmt() {
    Consume(TokenType::kw_if);
    Consume(TokenType::l_parent);
    auto condExpr = ParseExpr();
    Consume(TokenType::r_parent);
    auto thenStmt = ParseStmt();

    std::shared_ptr<AstNode> elseStmt = nullptr;
    // peek token is 'else'
    if (token.tokenType == TokenType::kw_else) {
        Consume(TokenType::kw_else);
        elseStmt = ParseStmt();
    }

    return sema.semaIfStmtNode(condExpr, thenStmt, elseStmt);
}

std::shared_ptr<AstNode> Parser::ParseBlockStmt() {
    sema.EnterScope();
    Consume(TokenType::l_brace);

    auto blockStmt = std::make_shared<BlockStmt>();
    while(token.tokenType != TokenType::r_brace) {
        auto stmt = ParseStmt();
        if (stmt != nullptr) {
            blockStmt->nodeVec.push_back(stmt);
        } 
    }
    
    Consume(TokenType::r_brace);
    sema.ExitScope();
    return blockStmt;
}


// for-stmt : "for" "(" expr? ";" expr? ";" expr? ")" stmt
// 	          "for" "(" decl-stmt expr? ";" expr? ")" stmt
std::shared_ptr<AstNode> Parser::ParseForStmt() {
    Consume(TokenType::kw_for);
    Consume(TokenType::l_parent);

    sema.EnterScope();

    std::shared_ptr<ForStmt> forStmt = std::make_shared<ForStmt>();
    breakNodes.push_back(forStmt);
    continueNodes.push_back(forStmt);

    std::shared_ptr<AstNode> initNode;
    std::shared_ptr<AstNode> condNode;
    std::shared_ptr<AstNode> incNode;
    std::shared_ptr<AstNode> bodyNode;

    if (IsTypeName(token.tokenType)) {
        initNode = ParseDeclareStmt();
    } else {
        if (token.tokenType != TokenType::semi) {
            initNode =  ParseExpr();
        }
        Consume(TokenType::semi);
    }

    if (token.tokenType != TokenType::semi) {
        condNode = ParseExpr();
    }
    Consume(TokenType::semi);

    if (token.tokenType != TokenType::r_parent) {
        incNode = ParseExpr();
    }
    Consume(TokenType::r_parent);

    bodyNode = ParseStmt();

    forStmt->initNode = initNode;
    forStmt->condNode = condNode;
    forStmt->incNode = incNode;
    forStmt->bodyNode = bodyNode;


    breakNodes.pop_back();
    continueNodes.pop_back();
    sema.ExitScope();

    return forStmt;
}


std::shared_ptr<AstNode> Parser::ParseBreakStmt() {
    if (breakNodes.size() == 0) {
        GetDiagEngine().Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_break_stmt);
    }
    Consume(TokenType::kw_break);
    auto breakStmt = std::make_shared<BreakStmt>();
    breakStmt->targetNode = breakNodes.back();
    
    Consume(TokenType::semi);
    return breakStmt;
}

std::shared_ptr<AstNode> Parser::ParseContinueStmt() {
    if (continueNodes.size() == 0) {
        GetDiagEngine().Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_continue_stmt);
    }
    Consume(TokenType::kw_continue);
    auto continueStmt = std::make_shared<ContinueStmt>();
    continueStmt->targetNode = continueNodes.back();

    Consume(TokenType::semi);
    return continueStmt;
}


// expr  ::= assign (, assign )*
// eg： int result = (a = 1, b = 2, c = 3);  // 逗号表达式
std::shared_ptr<AstNode> Parser::ParseExpr() {
    auto left = ParseAssignExpr();
    while(token.tokenType == TokenType::comma) {
        Consume(TokenType::comma);
        left = sema.semaBinaryExprNode(left, ParseAssignExpr(), BinaryOp::comma);
    }
    return left;

    // bool isAssignExpr = false;
    // lexer.SaveState();
    // if (token.tokenType == TokenType::identifier) {
    //     Token tmp;
    //     lexer.NextToken(tmp);
    //     if (tmp.tokenType == TokenType::equal) {
    //         isAssignExpr = true;
    //     }
    // }
    // lexer.RestoreState();

    // if (isAssignExpr) {
    //     // assign-expr: identifier "=" expr
    //     return ParseAssignExpr();
    // }
    // return ParseLogOrExpr();
}




// a = 5;
// a = b = 3;
std::shared_ptr<AstNode> Parser::ParseAssignExpr() {
    auto left = ParseConditionalExpr();
    if (!IsAssignOperator()) {
        return left;
    }
    BinaryOp op;
    switch (token.tokenType)
    {
    case TokenType::equal:
        op = BinaryOp::assign;
        break;
    case TokenType::plus_equal:
        op = BinaryOp::add_assign;
        break;
    case TokenType::minus_equal:
        op = BinaryOp::sub_assign;
        break;
    case TokenType::star_equal:
        op = BinaryOp::mul_assign;
        break;
    case TokenType::slash_equal:
        op = BinaryOp::div_assign;
        break;
    case TokenType::percent_equal:
        op = BinaryOp::mod_assign;
        break;
    case TokenType::pipe_equal:
        op = BinaryOp::bitwise_or_assign;
        break;
    case TokenType::amp_equal:
        op = BinaryOp::bitwise_and_assign;
        break;
    case TokenType::caret_equal:
        op = BinaryOp::bitwise_xor_assign;
        break;
    case TokenType::less_less_equal:
        op = BinaryOp::left_shift_assign;
        break;
    case TokenType::greater_greater_equal:
        op = BinaryOp::right_shift_assign;
        break;
    default:
        break;
    }
    NextToken();
    return sema.semaBinaryExprNode(left, ParseAssignExpr(), op);

    // Expect(TokenType::identifier);
    // std::shared_ptr<VariableAccessExpr> left = sema.semaVariableAccessNode(token);
    // NextToken();

    // auto opToken = token;
    // Consume(TokenType::equal);
    // auto right = ParseExpr();
    // return sema.semaAssignExprNode(left, right, opToken);
}

// conditional ::= logor  ("?" expr ":" conditional)?
std::shared_ptr<AstNode> Parser::ParseConditionalExpr() {
    auto left = ParseLogOrExpr();
    if (token.tokenType != TokenType::question) {
        return left;
    }
    Token tmpToken = this->token;
    Consume(TokenType::question);
    auto then = ParseExpr();
    Consume(TokenType::colon);
    auto els = ParseConditionalExpr();
    return sema.semaThreeExprNode(left, then, els, tmpToken);
}


// logor-expr: logand-expr ("||" logand-expr)*
std::shared_ptr<AstNode> Parser::ParseLogOrExpr() {
    auto left = ParseLogAndExpr();
    while (token.tokenType == TokenType::pipepipe) {
        BinaryOp op = BinaryOp::logic_or;
        Advance(); // 前进一个token
        left = sema.semaBinaryExprNode(left, ParseLogAndExpr(), op);
    }
    return left;
}

// logand-expr: bitor-expr ("&&" bitor-expr)*
std::shared_ptr<AstNode> Parser::ParseLogAndExpr() {
    auto left = ParseBitOrExpr();
    while (token.tokenType == TokenType::ampamp) {
        BinaryOp op = BinaryOp::logic_and;
        Advance(); // 前进一个token
        left = sema.semaBinaryExprNode(left, ParseBitOrExpr(), op);
    }
    return left;
}

// bitor-expr: bitxor-expr ("|" bitxor-expr)*
std::shared_ptr<AstNode> Parser::ParseBitOrExpr() {
    auto left = ParseBitXorExpr();
    while (token.tokenType == TokenType::pipe) {
        BinaryOp op = BinaryOp::bitwise_or;
        Advance(); // 前进一个token
        left = sema.semaBinaryExprNode(left, ParseBitXorExpr(), op);
    }
    return left;
}

// bitxor-expr: bitand-expr ("^" bitand-expr)*
std::shared_ptr<AstNode> Parser::ParseBitXorExpr() {
    auto left = ParseBitAndExpr();
    while (token.tokenType == TokenType::caret) {
        BinaryOp op = BinaryOp::bitwise_xor;
        Advance(); // 前进一个token
        left = sema.semaBinaryExprNode(left, ParseBitAndExpr(), op);
    }
    return left;
}

// bitand-expr: equal-expr ("&" equal-expr)*
std::shared_ptr<AstNode> Parser::ParseBitAndExpr() {
    auto left = ParseEqualExpr();
    while (token.tokenType == TokenType::amp) {
        BinaryOp op = BinaryOp::bitwise_and;
        Advance(); // 前进一个token
        left = sema.semaBinaryExprNode(left, ParseEqualExpr(), op);
    }
    return left;
}


// equal-expr : relational-expr (("==" | "!=") relational-expr)*
std::shared_ptr<AstNode> Parser::ParseEqualExpr() {
    auto left = ParseRelationalExpr();
    while (token.tokenType == TokenType::equal_equal || token.tokenType == TokenType::not_equal) {
        BinaryOp op;
        if (token.tokenType == TokenType::equal_equal) {
            op = BinaryOp::equal;
        } else {
            op = BinaryOp::not_equal;
        }
        Advance(); // 前进一个token
        left = sema.semaBinaryExprNode(left, ParseRelationalExpr(), op);
    }
    return left;
}

// relational-expr: add-expr (("<"|">"|"<="|">=") add-expr)*
std::shared_ptr<AstNode> Parser::ParseRelationalExpr() {
    auto left = ParseShiftExpr();
    while (token.tokenType == TokenType::less || token.tokenType == TokenType::less_equal || 
            token.tokenType == TokenType::greater || token.tokenType == TokenType::greater_equal) {
        BinaryOp op;
        if (token.tokenType == TokenType::less) {
            op = BinaryOp::less;
        } else if (token.tokenType == TokenType::less_equal) {
            op = BinaryOp::less_equal;
        } else if (token.tokenType == TokenType::greater) {
            op = BinaryOp::greater;
        } else if (token.tokenType == TokenType::greater_equal) {
            op = BinaryOp::greater_equal;
        }
        Advance(); // 前进一个token
        left = sema.semaBinaryExprNode(left, ParseShiftExpr(), op);
    }
    return left;
}


// add-expr : mult-expr (("+" | "-") mult-expr)* 
std::shared_ptr<AstNode> Parser::ParseAddExpr() {
    auto left = ParseMultExpr();
    while (token.tokenType == TokenType::plus || token.tokenType == TokenType::minus) {
        BinaryOp op;
        if (token.tokenType == TokenType::plus) {
            op = BinaryOp::add;
        } else {
            op = BinaryOp::sub;
        }
        Advance(); // 前进一个token
        left = sema.semaBinaryExprNode(left, ParseMultExpr(), op);
    }
    return left;
}

// 左结合
// mult-expr : primary-expr (("*" | "/") primary-expr)* 
std::shared_ptr<AstNode> Parser::ParseMultExpr() {
    auto left = ParseUnaryExpr();
    while (token.tokenType == TokenType::star || 
        token.tokenType == TokenType::slash ||
        token.tokenType == TokenType::percent) {
        BinaryOp op;
        if (token.tokenType == TokenType::star) {
            op = BinaryOp::mul;
        } else if (token.tokenType == TokenType::slash) {
            op = BinaryOp::div;
        } else  {
            op = BinaryOp::mod;
        }
        Advance(); // 前进一个token
        left = sema.semaBinaryExprNode(left, ParseUnaryExpr(), op);
    }
    return left;
}

std::shared_ptr<AstNode> Parser::ParseUnaryExpr() {
    if (!IsUnaryOperator()) {
        auto left = ParsePostfixExpr();
        return left;
    }

    // handle sizeof
    if (this->token.tokenType == TokenType::kw_sizeof) {
        bool isTypeName = false;
        Consume(TokenType::kw_sizeof);

        if (token.tokenType == TokenType::l_parent) {
            lexer.SaveState();

            Token tmpToken;
            lexer.NextToken(tmpToken);
            if (IsTypeName(tmpToken.tokenType)) {
                isTypeName = true;
            }
            lexer.RestoreState();
        }

        if (isTypeName) {
            Consume(TokenType::l_parent);
            auto type = ParseType();
            Consume(TokenType::r_parent);
            return sema.semaSizeOfExprNode(nullptr, type);
        } else {
            auto node = ParseUnaryExpr(); 
            return sema.semaSizeOfExprNode(node, nullptr);
        }
    }

    UnaryOp op;
    switch (token.tokenType)
    {
    case TokenType::plus_plus:
        op = UnaryOp::inc;
        break;
    case TokenType::minus_minus:
        op = UnaryOp::dec;
        break;
    case TokenType::plus:
        op = UnaryOp::positive;
        break;
    case TokenType::minus:
        op = UnaryOp::negative;
        break;
    case TokenType::star:
        op = UnaryOp::deref;
        break;
    case TokenType::amp:
        op = UnaryOp::addr;
        break;
    case TokenType::exclaim:
        op = UnaryOp::logic_not;
        break;
    case TokenType::tilde:
        op = UnaryOp::bitwise_not;
        break;
    default:
        break;
    }
    NextToken();

    Token tmpToken = token;
    auto node = ParseUnaryExpr();
    return sema.semaUnaryExprNode(node, op, tmpToken);
}

/// 解析后缀表达式, 如：
/// a++, a--, p++, p--
/// a[3][5]
std::shared_ptr<AstNode> Parser::ParsePostfixExpr() {
    auto left = ParsePrimaryExpr();
    for(;;) {
        if (this->token.tokenType == TokenType::plus_plus) {
            left = sema.semaPostIncExprNode(left, this->token);
            Consume(TokenType::plus_plus);
            continue;
        } 
        
        // a--
        if (this->token.tokenType == TokenType::minus_minus) {
            left = sema.semaPostDecExprNode(left, this->token);
            Consume(TokenType::minus_minus);
            continue;
        } 
        
        // arr[2][3]
        if (this->token.tokenType == TokenType::l_bracket) {
            Token tmp = token;
            Consume(TokenType::l_bracket);
            auto node = ParseExpr();
            Consume(TokenType::r_bracket);
            left = sema.semaPostSubscriptNode(left, node, tmp);
            continue;
        } 
        
        // stu.name 结构体成员访问
        if (this->token.tokenType == TokenType::dot) {
            Token dottoken = token;
            Consume(TokenType::dot);
            Token tmpToken = token;
            Consume(TokenType::identifier);
            left = sema.semaPostMemberDotNode(left, tmpToken, dottoken);
            continue;
        }

        // stu->name 结构体成员访问
        if (this->token.tokenType == TokenType::arrow) {
            Token arrowtoken = token;
            Consume(TokenType::arrow);
            Token tmpToken = token;
            Consume(TokenType::identifier);
            left = sema.semaPostMemberArrowNode(left, tmpToken, arrowtoken);
            continue;
        }
        break;
    }
    return left;
}


// primary-expr : identifier | number | "(" expr ")" 
std::shared_ptr<AstNode> Parser::ParsePrimaryExpr() {
    if (token.tokenType == TokenType::l_parent) {
        NextToken();

        auto expr = ParseExpr();
        assert(Expect(TokenType::r_parent));

        NextToken();
        return expr;
    } else if (token.tokenType == TokenType::identifier) {
        // 说明是变量访问
        auto expr = sema.semaVariableAccessNode(token);
        NextToken();
        return expr;
    }else {
        Expect(TokenType::number);
        auto factor = sema.semaNumberExprNode(token, token.type);
        NextToken();
        return factor;
    }
}

// shift-expr: add-expr (("<<" | ">>") add-expr)*
std::shared_ptr<AstNode> Parser::ParseShiftExpr() {
    auto left = ParseAddExpr();
    while (token.tokenType == TokenType::less_less || token.tokenType == TokenType::greater_greater) {
        BinaryOp op;
        if (token.tokenType == TokenType::less_less) {
            op = BinaryOp::left_shift;
        } else {
            op = BinaryOp::right_shift;
        }
        Advance(); // 前进一个token
        left = sema.semaBinaryExprNode(left, ParseAddExpr(), op);
    }
    return left;
}

/// 解析类型如: sizeof 后面的类型
/// sizeof(int)
/// sizeof(int [3][4])
/// sizeof(int *[3][4])
std::shared_ptr<CType> Parser::ParseType() {   
    std::shared_ptr<CType> baseType = ParseDeclSpec();
    // baseType 不能为空
    assert(baseType);

    // 解析指针类型
    while(token.tokenType == TokenType::star) {
        baseType = std::make_shared<CPointType>(baseType);
        Consume(TokenType::star);
    }

    if (token.tokenType == TokenType::l_bracket) {
       baseType = DirectDeclaratorArraySuffix(baseType);
    }

    return baseType;
}


bool Parser::Expect(TokenType tokenType) {
    if (token.tokenType == tokenType) {
        return true;
    }
    GetDiagEngine().Report(
        llvm::SMLoc::getFromPointer(token.ptr), 
        diag::err_expected, 
        Token::GetSpellingText(tokenType), 
        llvm::StringRef(token.ptr, token.len));
    return false;
}

bool Parser::Consume(TokenType tokenType) {
    if (Expect(tokenType)) {
        Advance();
        return true;
    }
    return false;
}

void Parser::Advance() {
    lexer.NextToken(token);
}

void Parser::NextToken() {
    lexer.NextToken(token);
}


bool Parser::IsTypeName(TokenType tokenType) {
    if (tokenType == TokenType::kw_int) {
        return true;
    } else if (tokenType == TokenType::kw_struct || tokenType == TokenType::kw_union) {
        return true;
    }
    return false;
}


bool Parser::IsAssignOperator() {
    return token.tokenType == TokenType::equal
        || token.tokenType == TokenType::plus_equal
        || token.tokenType == TokenType::minus_equal
        || token.tokenType == TokenType::star_equal
        || token.tokenType == TokenType::slash_equal
        || token.tokenType == TokenType::percent_equal
        || token.tokenType == TokenType::pipe_equal
        || token.tokenType == TokenType::amp_equal
        || token.tokenType == TokenType::caret_equal
        || token.tokenType == TokenType::less_less_equal
        || token.tokenType == TokenType::greater_greater_equal;
}


bool Parser::IsUnaryOperator() {
    return token.tokenType == TokenType::plus_plus
        || token.tokenType == TokenType::minus_minus
        || token.tokenType == TokenType::plus
        || token.tokenType == TokenType::minus
        || token.tokenType == TokenType::star
        || token.tokenType == TokenType::amp
        || token.tokenType == TokenType::exclaim
        || token.tokenType == TokenType::tilde
        || token.tokenType == TokenType::kw_sizeof;
}