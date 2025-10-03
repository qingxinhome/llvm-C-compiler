#include "sema.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Casting.h"

std::shared_ptr<VariableDecl> Sema::semaVariableDeclNode(llvm::StringRef name, CType *type) {
    // 1. 检查在当前作用域内是否存在重名
    auto symbol = scope.FindSymbolInCurScope(name);
    if (symbol) {
        llvm::errs() << "re define variable name " << name << "\n";
        return nullptr;
    }
    // 2. 添加到符号表中
    scope.AddSymbol(SymbolKind::LocalVariable, type, name);

    std::shared_ptr<VariableDecl> variableDecl = std::make_shared<VariableDecl>();
    variableDecl->name = name;
    variableDecl->type = type;
    return variableDecl;
}


std::shared_ptr<VariableAccessExpr> Sema::semaVariableAccessNode(llvm::StringRef name) {
    // 从符号表栈的所有符号表中检查符号是否存在
    std::shared_ptr<Symbol> symbol = scope.FindSymbol(name);
    if(symbol == nullptr) {
        llvm::errs() << "use undefined symbol " << name << "\n";
        return nullptr; 
    }
    std::shared_ptr<VariableAccessExpr> expr = std::make_shared<VariableAccessExpr>();
    expr->name = name;
    expr->type = symbol->GetType();
    return expr;
}

std::shared_ptr<AssignExpr> Sema::semaAssignExprNode(std::shared_ptr<AstNode> left, std::shared_ptr<AstNode> right) {
    // 1.检查左右表达式节点是否为空
    if (!left || !right) {
        llvm::errs() << "left and right node of assign expression cannot be null" << "\n";
        return nullptr;
    }
    
    // 2. 赋值表达式的左节点必须是左值
    if (!llvm::isa<VariableAccessExpr>(left.get())) {
        llvm::errs() << "left node of assignment expression must be lvalue \n";
        return nullptr;
    }

    std::shared_ptr<AssignExpr> assignExpr = std::make_shared<AssignExpr>();
    assignExpr->left = left;
    assignExpr->right = right;
    return assignExpr;
}


std::shared_ptr<BinaryExpr> Sema::semaBinaryExprNode(std::shared_ptr<AstNode> left, std::shared_ptr<AstNode> right, OpCode op){
    auto binaryExpr = std::make_shared<BinaryExpr>();
    binaryExpr->op = op;
    binaryExpr->left = left;
    binaryExpr->right = right;
    return binaryExpr;
}


std::shared_ptr<NumberExpr> Sema::semaNumberExprNode(int number, CType* type) {
    std::shared_ptr<NumberExpr> factor = std::make_shared<NumberExpr>();
    factor->number = number;
    factor->type = type;
    return factor;
}