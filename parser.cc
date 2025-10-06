#include "parser.h"

/*
prog : stmt*     
stmt : decl-stmt | expr-stmt | null-stmt      
null-stmt : ";"      
decl-stmt : "int" identifier ("," identifier (= expr)?)* ";"
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
    std::vector<std::shared_ptr<AstNode>> exprVec;
    while(token.tokenType != TokenType::eof) {
        // null statement
        if (token.tokenType == TokenType::semi) {
            // Advance();
            NextToken();
            continue;
        }

        // declare statment
        if (token.tokenType == TokenType::kw_int) {
            auto exprs = ParseDeclareStmt();
            for (auto & expr : exprs)
            {
                exprVec.push_back(expr);
            }
        } else {
            // expr statement
            auto expr = ParseExprStmt();
            exprVec.push_back(expr);
        }
    }

    auto program = std::make_shared<Program>();
    program->exprVec = move(exprVec);
    return program;
}

// 解析变量声明语句
std::vector<std::shared_ptr<AstNode>> Parser::ParseDeclareStmt() {
    Consume(TokenType::kw_int);
    CType *baseTy = CType::GetIntTy();

    std::vector<std::shared_ptr<AstNode>> astArr;

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
        astArr.push_back(variableDecl);

        Consume(TokenType::identifier);
        if (token.tokenType == TokenType::equal) {
            auto opToken = token;
            NextToken();

            std::shared_ptr<VariableAccessExpr> left = sema.semaVariableAccessNode(tmpToken);
            auto right = ParseExpr();
            
            std::shared_ptr<AssignExpr> assignExpr = sema.semaAssignExprNode(left, right, opToken);
            astArr.push_back(assignExpr);
        }
    }
    Consume(TokenType::semi);
    return astArr;
}


std::shared_ptr<AstNode> Parser::ParseExprStmt() {
    std::shared_ptr<AstNode> expr = ParseExpr();
    Consume(TokenType::semi);
    return expr;
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

    auto left = ParseTerm();
    while (token.tokenType == TokenType::plus || token.tokenType == TokenType::minus) {
        OpCode op;
        if (token.tokenType == TokenType::plus) {
            op = OpCode::add;
        } else {
            op = OpCode::sub;
        }
        Advance(); // 前进一个token
        
        auto right = ParseTerm();
        auto binaryExpr = sema.semaBinaryExprNode(left, right, op);

        left = binaryExpr;
    }
    return left;
}

// 左结合
std::shared_ptr<AstNode> Parser::ParseTerm() {
    auto left = ParseFactor();
    while (token.tokenType == TokenType::star || token.tokenType == TokenType::slash) {
        OpCode op;
        if (token.tokenType == TokenType::star) {
            op = OpCode::mul;
        } else {
            op = OpCode::div;
        }
        Advance(); // 前进一个token
        
        auto right = ParseFactor();
        auto binaryExpr = sema.semaBinaryExprNode(left, right, op);

        left = binaryExpr;
    }
    return left;
}

std::shared_ptr<AstNode> Parser::ParseFactor() {
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