#pragma once

#include "lexer.h"
#include "ast.h"
#include "sema.h"

class Parser {
private:
    Lexer &lexer;
    Sema &sema;
    Token token;  // 当前token

    // 存储能够被break的语句节点（利用vector的栈特性）
    std::vector<std::shared_ptr<AstNode>> breakNodes;
    // 存储能够被continue的语句节点（利用vector的栈特性）
    std::vector<std::shared_ptr<AstNode>> continueNodes;
public:
    Parser(Lexer &lexer, Sema &sema) : lexer(lexer), sema(sema){
        // 初始化Parser的时候首先得到第一个token
        Advance();
    }

    std::shared_ptr<Program> ParseProgram();

private:
    std::shared_ptr<AstNode> ParseFunctionDecl();
    std::shared_ptr<AstNode> ParseStmt();

    // 一个声明语句可以声明多个变量
    std::shared_ptr<AstNode> ParseDeclareStmt(bool isGloabl = false);
    // decl-spec  ::= "int" | struct-or-union-specifier
    std::shared_ptr<CType> ParseDeclSpec();
    std::shared_ptr<CType> ParseStructOrUnionSpec();
    // declarator ::= "*"* direct-declarator
    std::shared_ptr<AstNode> Declarator(std::shared_ptr<CType> baseType, bool isGloabl);
    // direct-declarator ::= identifier | direct-declarator "[" assign "]"
    std::shared_ptr<AstNode> DirectDeclarator(std::shared_ptr<CType> baseType, bool isGloabl);
    
    std::shared_ptr<CType> DirectDeclaratorSuffix(Token ident, std::shared_ptr<CType> baseType, bool isGloabl);
    std::shared_ptr<CType> DirectDeclaratorArraySuffix(std::shared_ptr<CType> baseType, bool isGloabl); 
    std::shared_ptr<CType> DirectDeclaratorFuncSuffix(Token ident, std::shared_ptr<CType> baseType, bool isGloabl); // 解析标识符后面加()的后缀形式， 就是函数声明
    
    // initializer ::= assign | "{" initializer ("," initializer)*  "}"
    bool ParseInitializer(std::vector<std::shared_ptr<VariableDecl::InitValue>> &arr, 
        std::shared_ptr<CType> declType, 
        std::vector<int> &offsetList,
        bool hasLBrace
    );

    bool ParseStringInitializer(std::vector<std::shared_ptr<VariableDecl::InitValue>> &arr, 
        std::shared_ptr<CType> declType, 
        std::vector<int> &offsetList
    );


    std::shared_ptr<AstNode> ParseIfStmt();
    std::shared_ptr<AstNode> ParseForStmt();
    std::shared_ptr<AstNode> ParseBreakStmt();
    std::shared_ptr<AstNode> ParseContinueStmt();
    std::shared_ptr<AstNode> ParseReturnStmt();
    std::shared_ptr<AstNode> ParseBlockStmt();
    std::shared_ptr<AstNode> ParseExprStmt();

    std::shared_ptr<AstNode> ParseExpr();
    std::shared_ptr<AstNode> ParseAssignExpr();
    std::shared_ptr<AstNode> ParseConditionalExpr();

    std::shared_ptr<AstNode> ParseLogOrExpr();
    std::shared_ptr<AstNode> ParseLogAndExpr();
    std::shared_ptr<AstNode> ParseBitOrExpr();
    std::shared_ptr<AstNode> ParseBitXorExpr();
    std::shared_ptr<AstNode> ParseBitAndExpr();
    std::shared_ptr<AstNode> ParseShiftExpr();

    std::shared_ptr<AstNode> ParseEqualExpr();
    std::shared_ptr<AstNode> ParseRelationalExpr();
    std::shared_ptr<AstNode> ParseAddExpr();
    std::shared_ptr<AstNode> ParseMultExpr(); //std::shared_ptr<AstNode> ParseTerm();
    std::shared_ptr<AstNode> ParseUnaryExpr();
    std::shared_ptr<AstNode> ParsePostfixExpr();
    std::shared_ptr<AstNode> ParsePrimaryExpr(); //std::shared_ptr<AstNode> ParseFactor();
    
    std::shared_ptr<CType> ParseType();

    // 检测当前token是否为指定类型， 不会消费
    bool Expect(TokenType tokenType);

    // 先检测， 并消费(前进一个token)
    bool Consume(TokenType tokenType);

    // 前进一个token
    void Advance();
    void NextToken();
    // 消解类型类型限定符（在类型声明时有类型限定符，如const, static）
    void ConsumeTypeQulify();

    // 判断当前token是否为是类型声明关键字
    bool IsTypeName(TokenType tokenType);

    // 判断是函数声明 or 全局变量声明
    bool IsFunctionDecl();

    // 判断是否为字符串数组类型的声明， 即： char a[20];
    bool IsStringArrayTy(std::shared_ptr<CType> ty);

    // 判断是否为赋值系列运算符
    bool IsAssignOperator();
    bool IsUnaryOperator();

    DiagEngine& GetDiagEngine() {
        return lexer.GetDiagEngine();
    }
};