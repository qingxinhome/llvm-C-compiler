#pragma

#include "scope.h"
#include "ast.h"

// Semantic 语义分析模块
class Sema {
public:
    /* Semantic check mode*/
    enum class Mode {
        Normal,      /* 正常模式 */
        Skip         /* 跳过模式，用于提前初探 */
    };
public:
    Sema(DiagEngine &diagEngine) : diagEngine(diagEngine) {}
    // 对变量声明节点进行语义检查
    std::shared_ptr<VariableDecl> semaVariableDeclNode(Token token, std::shared_ptr<CType> type);
    std::shared_ptr<VariableAccessExpr> semaVariableAccessNode(Token token);
    std::shared_ptr<NumberExpr> semaNumberExprNode(Token token, std::shared_ptr<CType> type);

    std::shared_ptr<UnaryExpr> semaUnaryExprNode(std::shared_ptr<AstNode> unary, UnaryOp op, Token token);
    std::shared_ptr<BinaryExpr> semaBinaryExprNode(std::shared_ptr<AstNode> left, std::shared_ptr<AstNode> right, BinaryOp op);
    std::shared_ptr<ThreeExpr> semaThreeExprNode(std::shared_ptr<AstNode> cond, std::shared_ptr<AstNode> then, std::shared_ptr<AstNode> els, Token token);
    std::shared_ptr<SizeOfExpr> semaSizeOfExprNode(std::shared_ptr<AstNode> unary, std::shared_ptr<CType> type);
    std::shared_ptr<PostIncExpr> semaPostIncExprNode(std::shared_ptr<AstNode> left, Token token);
    std::shared_ptr<PostDecExpr> semaPostDecExprNode(std::shared_ptr<AstNode> left, Token token);
    std::shared_ptr<PostSubscript> semaPostSubscriptNode(std::shared_ptr<AstNode> left, std::shared_ptr<AstNode> node, Token curtoken);
    std::shared_ptr<VariableDecl::InitValue> semaDeclInitValue(std::shared_ptr<CType> declType, std::shared_ptr<AstNode> value, std::vector<int> &offsetList, Token token);

    std::shared_ptr<IfStmt> semaIfStmtNode(std::shared_ptr<AstNode> condNode, std::shared_ptr<AstNode> thenNode, std::shared_ptr<AstNode> elseNode);
    
    void EnterScope();
    void ExitScope();

    void SetMode(Mode mode);
private:
    Scope scope;
    Mode mode{Mode::Normal};         /* 语义检查模式 */
    DiagEngine &diagEngine;
};