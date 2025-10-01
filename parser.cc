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
    std::vector<std::shared_ptr<Expr>> exprVec;
    while(token.tokenType != TokenType::eof) {
        if (token.tokenType == TokenType::semi) {
            Advance();
            continue;
        }

        auto expr = ParseExpr();
        exprVec.push_back(expr);
    }

    auto program = std::make_shared<Program>();
    program->exprVec = move(exprVec);
    return program;
}

// 左结合
std::shared_ptr<Expr> Parser::ParseExpr() {
    auto left = ParseTerm();
    while (token.tokenType == TokenType::plus || token.tokenType == TokenType::minus) {
        OpCode op;
        if (token.tokenType == TokenType::plus) {
            op = OpCode::add;
        } else {
            op = OpCode::sub;
        }
        Advance(); // 前进一个token
        
        auto binaryExpr = std::make_shared<BinaryExpr>();
        binaryExpr->op = op;
        binaryExpr->left = left;
        binaryExpr->right = ParseTerm();

        left = binaryExpr;
    }
    return left;
}

// 左结合
std::shared_ptr<Expr> Parser::ParseTerm() {
    auto left = ParseFactor();
    while (token.tokenType == TokenType::star || token.tokenType == TokenType::slash) {
        OpCode op;
        if (token.tokenType == TokenType::star) {
            op = OpCode::mul;
        } else {
            op = OpCode::div;
        }
        Advance(); // 前进一个token
        
        auto binaryExpr = std::make_shared<BinaryExpr>();
        binaryExpr->op = op;
        binaryExpr->left = left;
        binaryExpr->right = ParseFactor();

        left = binaryExpr;
    }
    return left;
}

std::shared_ptr<Expr> Parser::ParseFactor() {
    if (token.tokenType == TokenType::l_parent) {
        Advance();
        auto expr = ParseExpr();
        assert(Expect(TokenType::r_parent));
        Advance();
        return expr;
    } else {
        auto factor = std::make_shared<FactorExpr>();
        factor->number = token.value;
        Advance();
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