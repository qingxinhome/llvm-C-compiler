#pragma

#include "scope.h"
#include "ast.h"

// Semantic 语义分析模块
class Sema {
public:
    Sema(DiagEngine &diagEngine) : diagEngine(diagEngine) {}
    // 对变量声明节点进行语义检查
    std::shared_ptr<VariableDecl> semaVariableDeclNode(Token token, CType *type);
    std::shared_ptr<VariableAccessExpr> semaVariableAccessNode(Token token);
    std::shared_ptr<NumberExpr> semaNumberExprNode(Token token, CType* type);
    std::shared_ptr<AssignExpr> semaAssignExprNode(std::shared_ptr<AstNode> left, std::shared_ptr<AstNode> right, Token token);
    std::shared_ptr<BinaryExpr> semaBinaryExprNode(std::shared_ptr<AstNode> left, std::shared_ptr<AstNode> right, OpCode op);
    std::shared_ptr<IfStmt> semaIfStmtNode(std::shared_ptr<AstNode> condNode, std::shared_ptr<AstNode> thenNode, std::shared_ptr<AstNode> elseNode);
    void EnterScope();
    void ExitScope();
private:
    Scope scope;
    DiagEngine &diagEngine;
};