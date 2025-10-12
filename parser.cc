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
    std::vector<std::shared_ptr<AstNode>> nodeVec;
    while(token.tokenType != TokenType::eof) {
        auto stmt = ParseStmt();
        if (stmt == nullptr) {
            continue;
        }
        nodeVec.push_back(stmt);
    }
    auto program = std::make_shared<Program>();
    program->stmtNodeVec = move(nodeVec);
    return program;
}

std::shared_ptr<AstNode> Parser::ParseStmt() {
    // null statement
    if (token.tokenType == TokenType::semi) {
        NextToken();
        return nullptr;
    }
    // declare statment
    else if (token.tokenType == TokenType::kw_int) {
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

// 解析变量声明语句
std::shared_ptr<AstNode> Parser::ParseDeclareStmt() {
    Consume(TokenType::kw_int);
    CType *baseTy = CType::GetIntTy();

    auto declareStmt = std::make_shared<DeclareStmt>();
    // int a , b = 3;
    // int a = 3;
    int i = 0;
    while(token.tokenType != TokenType::semi) {
        // 如果是第二次进入循环体，判断是否为分号，
        if (i++ > 0) {
            Consume(TokenType::comma);
        }
        assert(token.tokenType == TokenType::identifier);

        // 获取变量名
        // auto variableName = token.content;
        Token tmpToken = token;

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
        blockStmt->nodeVec.push_back(ParseStmt());
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

    if (IsTypeName()) {
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


// 左结合
// expr : assign-expr | add-expr
// assign-expr: identifier "=" expr
// add-expr : mult-expr (("+" | "-") mult-expr)* 
std::shared_ptr<AstNode> Parser::ParseExpr() {
    bool isAssignExpr = false;
    lexer.SaveState();
    if (token.tokenType == TokenType::identifier) {
        Token tmp;
        lexer.NextToken(tmp);
        if (tmp.tokenType == TokenType::equal) {
            isAssignExpr = true;
        }
    }
    lexer.RestoreState();

    if (isAssignExpr) {
        // assign-expr: identifier "=" expr
        return ParseAssignExpr();
    }
    return ParseLogOrExpr();
}


// equal-expr : relational-expr (("==" | "!=") relational-expr)*
std::shared_ptr<AstNode> Parser::ParseEqualExpr() {
    auto left = ParseRelationalExpr();
    while (token.tokenType == TokenType::equal_equal || token.tokenType == TokenType::not_equal) {
        OpCode op;
        if (token.tokenType == TokenType::equal_equal) {
            op = OpCode::equal_equal;
        } else {
            op = OpCode::not_equal;
        }
        Advance(); // 前进一个token
        
        auto right = ParseRelationalExpr();
        auto binaryExpr = sema.semaBinaryExprNode(left, right, op);

        left = binaryExpr;
    }
    return left;
}

// relational-expr: add-expr (("<"|">"|"<="|">=") add-expr)*
std::shared_ptr<AstNode> Parser::ParseRelationalExpr() {
    auto left = ParseShiftExpr();
    while (token.tokenType == TokenType::less || token.tokenType == TokenType::less_equal || 
            token.tokenType == TokenType::greater || token.tokenType == TokenType::greater_equal) {
        OpCode op;
        if (token.tokenType == TokenType::less) {
            op = OpCode::less;
        } else if (token.tokenType == TokenType::less_equal) {
            op = OpCode::less_equal;
        } else if (token.tokenType == TokenType::greater) {
            op = OpCode::greater;
        } else if (token.tokenType == TokenType::greater_equal) {
            op = OpCode::greater_equal;
        }
        Advance(); // 前进一个token
        
        auto right = ParseShiftExpr();
        auto binaryExpr = sema.semaBinaryExprNode(left, right, op);

        left = binaryExpr;
    }
    return left;
}


// add-expr : mult-expr (("+" | "-") mult-expr)* 
std::shared_ptr<AstNode> Parser::ParseAddExpr() {
    auto left = ParseMultExpr();
    while (token.tokenType == TokenType::plus || token.tokenType == TokenType::minus) {
        OpCode op;
        if (token.tokenType == TokenType::plus) {
            op = OpCode::add;
        } else {
            op = OpCode::sub;
        }
        Advance(); // 前进一个token
        
        auto right = ParseMultExpr();
        auto binaryExpr = sema.semaBinaryExprNode(left, right, op);

        left = binaryExpr;
    }
    return left;
}

// 左结合
// mult-expr : primary-expr (("*" | "/") primary-expr)* 
std::shared_ptr<AstNode> Parser::ParseMultExpr() {
    auto left = ParsePrimaryExpr();
    while (token.tokenType == TokenType::star || 
        token.tokenType == TokenType::slash ||
        token.tokenType == TokenType::percent) {
        OpCode op;
        if (token.tokenType == TokenType::star) {
            op = OpCode::mul;
        } else if (token.tokenType == TokenType::slash) {
            op = OpCode::div;
        } else  {
            op = OpCode::mod;
        }
        Advance(); // 前进一个token
        
        auto right = ParsePrimaryExpr();
        auto binaryExpr = sema.semaBinaryExprNode(left, right, op);

        left = binaryExpr;
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

// a = 5;
// a = b = 3;
std::shared_ptr<AstNode> Parser::ParseAssignExpr() {
    Expect(TokenType::identifier);
    std::shared_ptr<VariableAccessExpr> left = sema.semaVariableAccessNode(token);
    NextToken();

    auto opToken = token;
    Consume(TokenType::equal);
    auto right = ParseExpr();
    return sema.semaAssignExprNode(left, right, opToken);
}

// logor-expr: logand-expr ("||" logand-expr)*
std::shared_ptr<AstNode> Parser::ParseLogOrExpr() {
    auto left = ParseLogAndExpr();
    while (token.tokenType == TokenType::pipepipe) {
        OpCode op = OpCode::logOr;
        Advance(); // 前进一个token
        
        auto right = ParseLogAndExpr();
        auto binaryExpr = sema.semaBinaryExprNode(left, right, op);

        left = binaryExpr;
    }
    return left;
}

// logand-expr: bitor-expr ("&&" bitor-expr)*
std::shared_ptr<AstNode> Parser::ParseLogAndExpr() {
    auto left = ParseBitOrExpr();
    while (token.tokenType == TokenType::ampamp) {
        OpCode op = OpCode::logAnd;
        Advance(); // 前进一个token
        
        auto right = ParseBitOrExpr();
        auto binaryExpr = sema.semaBinaryExprNode(left, right, op);

        left = binaryExpr;
    }
    return left;
}

// bitor-expr: bitxor-expr ("|" bitxor-expr)*
std::shared_ptr<AstNode> Parser::ParseBitOrExpr() {
    auto left = ParseBitXorExpr();
    while (token.tokenType == TokenType::pipe) {
        OpCode op = OpCode::bitOr;
        Advance(); // 前进一个token
        
        auto right = ParseBitXorExpr();
        auto binaryExpr = sema.semaBinaryExprNode(left, right, op);

        left = binaryExpr;
    }
    return left;
}

// bitxor-expr: bitand-expr ("^" bitand-expr)*
std::shared_ptr<AstNode> Parser::ParseBitXorExpr() {
    auto left = ParseBitAndExpr();
    while (token.tokenType == TokenType::caret) {
        OpCode op = OpCode::bitXor;
        Advance(); // 前进一个token
        
        auto right = ParseBitAndExpr();
        auto binaryExpr = sema.semaBinaryExprNode(left, right, op);

        left = binaryExpr;
    }
    return left;
}

// bitand-expr: equal-expr ("&" equal-expr)*
std::shared_ptr<AstNode> Parser::ParseBitAndExpr() {
    auto left = ParseEqualExpr();
    while (token.tokenType == TokenType::amp) {
        OpCode op = OpCode::bitAnd;
        Advance(); // 前进一个token
        
        auto right = ParseEqualExpr();
        auto binaryExpr = sema.semaBinaryExprNode(left, right, op);

        left = binaryExpr;
    }
    return left;
}

// shift-expr: add-expr (("<<" | ">>") add-expr)*
std::shared_ptr<AstNode> Parser::ParseShiftExpr() {
    auto left = ParseAddExpr();
    while (token.tokenType == TokenType::less_less || token.tokenType == TokenType::greater_greater) {
        OpCode op;
        if (token.tokenType == TokenType::less_less) {
            op = OpCode::leftShift;
        } else {
            op = OpCode::rightShift;
        }
        Advance(); // 前进一个token
        
        auto right = ParseAddExpr();
        auto binaryExpr = sema.semaBinaryExprNode(left, right, op);

        left = binaryExpr;
    }
    return left;
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


bool Parser::IsTypeName() {
    if (token.tokenType == TokenType::kw_int) {
        return true;
    }
    return false;
}