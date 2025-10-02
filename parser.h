#pragma once

#include "lexer.h"
#include "ast.h"

class Parser {
private:
    Lexer &lexer;
    Token token;  // 当前token
public:
    Parser(Lexer &lexer) : lexer(lexer){
        // 初始化Parser的时候首先得到第一个token
        Advance();
    }

    std::shared_ptr<Program> ParseProgram();
private:
    std::vector<std::shared_ptr<AstNode>> ParseDeclare(); // 一个声明语句可以声明多个变量
    std::shared_ptr<AstNode> ParseExpr();
    std::shared_ptr<AstNode> ParseTerm();
    std::shared_ptr<AstNode> ParseFactor();
    
    // 检测当前token是否为指定类型， 不会消费
    bool Expect(TokenType tokenType);

    // 先检测， 并消费(前进一个token)
    bool Consume(TokenType tokenType);

    // 前进一个token
    void Advance();
    void NextToken();
};