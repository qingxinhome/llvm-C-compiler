#include "sema.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Casting.h"

std::shared_ptr<VariableDecl> Sema::semaVariableDeclNode(Token token, std::shared_ptr<CType> type) {
    // 1. 检查在当前作用域内是否存在重名
    llvm::StringRef text(token.ptr, token.len);
    auto symbol = scope.FindSymbolInCurScope(text);
    if (symbol) {
        // llvm::errs() << "re define variable name " << text << "\n";
        diagEngine.Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_redefined, text);
    }
    // 2. 添加到符号表中
    scope.AddSymbol(SymbolKind::LocalVariable, type, text);

    std::shared_ptr<VariableDecl> variableDecl = std::make_shared<VariableDecl>();
    variableDecl->token = token;
    variableDecl->type = type;
    return variableDecl;
}


std::shared_ptr<VariableAccessExpr> Sema::semaVariableAccessNode(Token token) {
    // 从符号表栈的所有符号表中检查符号是否存在
    llvm::StringRef text(token.ptr, token.len);
    std::shared_ptr<Symbol> symbol = scope.FindSymbol(text);
    if(symbol == nullptr) {
        // llvm::errs() << "use undefined symbol " << text << "\n";
        diagEngine.Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_undefined, text);
    }
    std::shared_ptr<VariableAccessExpr> expr = std::make_shared<VariableAccessExpr>();
    expr->token = token;
    expr->type = symbol->GetType();
    return expr;
}


std::shared_ptr<BinaryExpr> Sema::semaBinaryExprNode(std::shared_ptr<AstNode> left, std::shared_ptr<AstNode> right, BinaryOp op){
    auto binaryExpr = std::make_shared<BinaryExpr>();
    binaryExpr->op = op;
    binaryExpr->left = left;
    binaryExpr->right = right;
    return binaryExpr;
}


std::shared_ptr<NumberExpr> Sema::semaNumberExprNode(Token token, std::shared_ptr<CType> type) {
    std::shared_ptr<NumberExpr> expr = std::make_shared<NumberExpr>();
    expr->token = token;
    expr->type = type;
    return expr;
}


std::shared_ptr<IfStmt> Sema::semaIfStmtNode(std::shared_ptr<AstNode> condNode, std::shared_ptr<AstNode> thenNode, std::shared_ptr<AstNode> elseNode) {
    std::shared_ptr<IfStmt> ifStmt = std::make_shared<IfStmt>();
    ifStmt->condNode = condNode;
    ifStmt->thenNode = thenNode;
    ifStmt->elseNode = elseNode;
    return ifStmt;
}


void Sema::EnterScope() {
    scope.EnterScope();
}

void Sema::ExitScope() {
    scope.ExitScope();
}