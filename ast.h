#pragma once

#include <memory>
#include <vector>
#include <llvm/IR/Value.h>
#include "type.h"
#include "lexer.h"

class Program;
class DeclareStmt;
class IfStmt;
class VariableDecl;
class VariableAccessExpr;
class AssignExpr;
class BinaryExpr;
class NumberExpr;

// 抽象访问者基类
class Visitor {
public:
    virtual ~Visitor(){}
    virtual llvm::Value* VisitProgram(Program *program) = 0;
    virtual llvm::Value* VisitDeclareStmt(DeclareStmt *declstmt) = 0;
    virtual llvm::Value* VisitIfStmt(IfStmt *ifstmt) = 0;
    virtual llvm::Value* VisitVariableDeclExpr(VariableDecl *decl) = 0;
    virtual llvm::Value* VisitVariableAccessExpr(VariableAccessExpr *expr) = 0;
    virtual llvm::Value* VisitAssignExpr(AssignExpr *expr) = 0;
    virtual llvm::Value* VisitBinaryExpr(BinaryExpr *binaryExpr) = 0;
    virtual llvm::Value* VisitNumberExpr(NumberExpr *expr) = 0;
};


// llvm rtti
// 定义语法树节点的抽象基类（对应表达式和语句）
class AstNode {
public:
    enum Kind{
        Node_IfStmt,
        Node_DeclareStmt,
        Node_VariableDecl,
        Node_BinaryExpr,
        Node_NumberExor,
        Node_VariableAccessExpr,
        Node_AssignExpr
    };
private:
    const Kind kind;
public:
    AstNode(Kind kind) : kind(kind) {}
    virtual ~AstNode(){}
    CType *type;
    Token token;
    const Kind GetKind() const {
        return kind;
    }
    virtual llvm::Value* Accept(Visitor *v) {return nullptr;}
};


class DeclareStmt : public AstNode {
public:
    std::vector<std::shared_ptr<AstNode>> nodeVec;
public:
    DeclareStmt() : AstNode(Node_DeclareStmt) {}
    llvm::Value* Accept(Visitor *v) {
        return v->VisitDeclareStmt(this);
    }
    static bool classof(const AstNode *node) {
        return node->GetKind() == Node_DeclareStmt;
    }
};


class IfStmt : public AstNode {
public:
    std::shared_ptr<AstNode> condNode;
    std::shared_ptr<AstNode> thenNode;
    std::shared_ptr<AstNode> elseNode;
public:
    IfStmt() : AstNode(Node_IfStmt) {}
    llvm::Value* Accept(Visitor *v) {
        return v->VisitIfStmt(this);
    }

    static bool classof(const AstNode *node) {
        return node->GetKind() == Node_IfStmt;
    }
};

class VariableDecl : public AstNode {
public:
    // llvm::StringRef name;
    VariableDecl() : AstNode(Node_VariableDecl) {}
    llvm::Value* Accept(Visitor *v) {
        return v->VisitVariableDeclExpr(this);
    }
    static bool classof(const AstNode *node) {
        return node->GetKind() == Node_VariableDecl;
    }
};

// 操作符枚举
enum class OpCode {
    add,
    sub,
    mul,
    div
};

// 二元表达式
class BinaryExpr : public AstNode {
public:
    OpCode op;
    std::shared_ptr<AstNode> left;
    std::shared_ptr<AstNode> right;

    BinaryExpr() : AstNode(Node_BinaryExpr){}
    llvm::Value* Accept(Visitor *v) override {
        return v->VisitBinaryExpr(this);
    }

    static bool classof(const AstNode *node) {
        return node->GetKind() == Node_BinaryExpr;
    }
};


// 因子表达式
class NumberExpr : public AstNode {
public:
    // int number;

    NumberExpr() : AstNode(Node_NumberExor){}

    llvm::Value* Accept(Visitor *v) override {
        return v->VisitNumberExpr(this);
    }
    static bool classof(const AstNode *node) {
        return node->GetKind() == Node_NumberExor;
    }
};


class VariableAccessExpr : public AstNode {
public:
    // llvm::StringRef name;

    VariableAccessExpr() : AstNode(Node_VariableAccessExpr) {}
    llvm::Value* Accept(Visitor *v) override {
        return v->VisitVariableAccessExpr(this);
    }
    static bool classof(const AstNode *node) {
        return node->GetKind() == Node_VariableAccessExpr;
    }
};


class AssignExpr : public AstNode {
public:
    std::shared_ptr<AstNode> left;
    std::shared_ptr<AstNode> right;

    AssignExpr() : AstNode(Node_AssignExpr) {}
    llvm::Value* Accept(Visitor *v) override {
        return v->VisitAssignExpr(this);
    }
    static bool classof(const AstNode *node) {
        return node->GetKind() == Node_AssignExpr;
    }
};

// 语法树根节点类型
class Program {
public:
    std::vector<std::shared_ptr<AstNode>> stmtNodeVec;
};