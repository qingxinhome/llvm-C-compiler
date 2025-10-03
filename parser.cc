#include "parser.h"

/**
prog : (expr? ";")*
expr : term (("+" | "-") term)* ;
term : factor (("*" | "/") factor)* ;
factor : number | "(" expr ")" ;
number: ([0-9])+ ;
 */
std::shared_ptr<Program> Parser::ParseProgram(){
    // while (true) .... exit ?
    // token -> eof
    std::vector<std::shared_ptr<AstNode>> exprVec;
    while(token.tokenType != TokenType::eof) {
        if (token.tokenType == TokenType::semi) {
            // Advance();
            NextToken();
            continue;
        }

        if (token.tokenType == TokenType::kw_int) {
            auto exprs = ParseDeclare();
            for (auto & expr : exprs)
            {
                exprVec.push_back(expr);
            }
        } else {
            auto expr = ParseExpr();
            exprVec.push_back(expr);
        }
    }

    auto program = std::make_shared<Program>();
    program->exprVec = move(exprVec);
    return program;
}

// 解析变量声明语句
std::vector<std::shared_ptr<AstNode>> Parser::ParseDeclare() {
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
        auto variableName = token.content;

        std::shared_ptr<VariableDecl> variableDecl = sema.semaVariableDeclNode(variableName, baseTy);
        astArr.push_back(variableDecl);

        Consume(TokenType::identifier);
        if (token.tokenType == TokenType::equal) {
            NextToken();

            std::shared_ptr<VariableAccessExpr> left = sema.semaVariableAccessNode(variableName);
            auto right = ParseExpr();
            
            std::shared_ptr<AssignExpr> assignExpr = sema.semaAssignExprNode(left, right);
            astArr.push_back(assignExpr);
        }
    }
    Consume(TokenType::semi);
    return astArr;
}


// 左结合
std::shared_ptr<AstNode> Parser::ParseExpr() {
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
        auto expr = sema.semaVariableAccessNode(token.content);
        NextToken();
        return expr;
    }else {
        auto factor = sema.semaNumberExprNode(token.value, token.type);
        NextToken();
        return factor;
    }
}

bool Parser::Expect(TokenType tokenType) {
    if (token.tokenType == tokenType) {
        return true;
    }
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