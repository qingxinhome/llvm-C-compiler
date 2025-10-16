#pragma

#include "scope.h"
#include "ast.h"

// Semantic 语义分析模块
class Sema {
public:
    Sema(DiagEngine &diagEngine) : diagEngine(diagEngine) {}
    // 对变量声明节点进行语义检查
    std::shared_ptr<VariableDecl> semaVariableDeclNode(Token token, std::shared_ptr<CType> type);
    std::shared_ptr<VariableAccessExpr> semaVariableAccessNode(Token token);
    std::shared_ptr<NumberExpr> semaNumberExprNode(Token token, std::shared_ptr<CType> type);
    std::shared_ptr<BinaryExpr> semaBinaryExprNode(std::shared_ptr<AstNode> left, std::shared_ptr<AstNode> right, BinaryOp op);
    std::shared_ptr<IfStmt> semaIfStmtNode(std::shared_ptr<AstNode> condNode, std::shared_ptr<AstNode> thenNode, std::shared_ptr<AstNode> elseNode);
    void EnterScope();
    void ExitScope();
private:
    Scope scope;
    DiagEngine &diagEngine;
};