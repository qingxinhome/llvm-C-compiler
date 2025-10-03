#pragma

#include "scope.h"
#include "ast.h"

// Semantic 语义分析模块
class Sema {
public:
    // 对变量声明节点进行语义检查
    std::shared_ptr<VariableDecl> semaVariableDeclNode(llvm::StringRef name, CType *type);
    std::shared_ptr<VariableAccessExpr> semaVariableAccessNode(llvm::StringRef name);
    std::shared_ptr<NumberExpr> semaNumberExprNode(int number, CType* type);
    std::shared_ptr<AssignExpr> semaAssignExprNode(std::shared_ptr<AstNode> left, std::shared_ptr<AstNode> right);
    std::shared_ptr<BinaryExpr> semaBinaryExprNode(std::shared_ptr<AstNode> left, std::shared_ptr<AstNode> right, OpCode op);
private:
    Scope scope;
};