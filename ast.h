#pragma once

#include <memory>
#include <vector>
#include <llvm/IR/Value.h>
#include "type.h"
#include "lexer.h"

class Program;
class DeclareStmt;
class IfStmt;
class ForStmt;
class ContinueStmt;
class BreakStmt;
class BlockStmt;
class VariableDecl;
class VariableAccessExpr;
class BinaryExpr;
class ThreeExpr;
class UnaryExpr;
class SizeOfExpr;
class PostDecExpr;
class PostIncExpr;
class NumberExpr;

// 抽象访问者基类
class Visitor {
public:
    virtual ~Visitor(){}
    virtual llvm::Value* VisitProgram(Program *program) = 0;
    virtual llvm::Value* VisitDeclareStmt(DeclareStmt *declstmt) = 0;
    virtual llvm::Value* VisitIfStmt(IfStmt *ifstmt) = 0;
    virtual llvm::Value* VisitForStmt(ForStmt *ifstmt) = 0;
    virtual llvm::Value* VisitBreakStmt(BreakStmt *breakstmt) = 0;
    virtual llvm::Value* VisitContinueStmt(ContinueStmt *continuestmt) = 0;
    virtual llvm::Value* VisitBlockStmt(BlockStmt *blockstmt) = 0;
    virtual llvm::Value* VisitVariableDeclExpr(VariableDecl *decl) = 0;
    virtual llvm::Value* VisitVariableAccessExpr(VariableAccessExpr *expr) = 0;
    virtual llvm::Value* VisitBinaryExpr(BinaryExpr *binaryExpr) = 0;
    virtual llvm::Value* VisitThreeExpr(ThreeExpr *threeExpr) = 0;
    virtual llvm::Value* VisitUnaryExpr(UnaryExpr *unaryExpr) = 0;
    virtual llvm::Value* VisitSizeOfExpr(SizeOfExpr *sizeofExpr) = 0;
    virtual llvm::Value* VisitPostIncExpr(PostIncExpr *postIncExpr) = 0;
    virtual llvm::Value* VisitPostDecExpr(PostDecExpr *postDecExpr) = 0;
    virtual llvm::Value* VisitNumberExpr(NumberExpr *expr) = 0;
};


// llvm rtti
// 定义语法树节点的抽象基类（对应表达式和语句）
class AstNode {
public:
    enum Kind{
        Node_IfStmt,
        Node_ForStmt,
        Node_BreakStmt,
        Node_ContinueStmt,
        Node_BlockStmt,
        Node_DeclareStmt,
        Node_VariableDecl,
        Node_BinaryExpr,
        Node_ThreeExpr,
        Node_UnaryExpr,
        Node_SizeOfExpr,
        Node_PostIncExpr,
        Node_PostDecExpr,
        Node_NumberExor,
        Node_VariableAccessExpr
    };
private:
    const Kind kind;
public:
    AstNode(Kind kind) : kind(kind) {}
    virtual ~AstNode(){}

    std::shared_ptr<CType> type;
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


class BlockStmt : public AstNode {
public:
    std::vector<std::shared_ptr<AstNode>> nodeVec;
public:
    BlockStmt() : AstNode(Node_BlockStmt) {}
    llvm::Value* Accept(Visitor *v) {
        return v->VisitBlockStmt(this);
    }
    static bool classof(const AstNode *node) {
        return node->GetKind() == Node_BlockStmt;
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


/*
for (int i = 0; i < 100; i = i+1) {
    aa = aa + 1;
}
*/
class ForStmt : public AstNode {
public:
    std::shared_ptr<AstNode> initNode;
    std::shared_ptr<AstNode> condNode;
    std::shared_ptr<AstNode> incNode;
    std::shared_ptr<AstNode> bodyNode;
public:
    ForStmt() : AstNode(Node_ForStmt) {}
    llvm::Value* Accept(Visitor *v) {
        return v->VisitForStmt(this);
    }

    static bool classof(const AstNode *node) {
        return node->GetKind() == Node_ForStmt;
    }
};


class BreakStmt : public AstNode {
public:
    // continue 所针对的语句
    std::shared_ptr<AstNode> targetNode;
public:
    BreakStmt() : AstNode(Node_BreakStmt) {}
    llvm::Value* Accept(Visitor *v) {
        return v->VisitBreakStmt(this);
    }

    static bool classof(const AstNode *node) {
        return node->GetKind() == Node_BreakStmt;
    }
};


class ContinueStmt : public AstNode {
public:
    // continue 所针对的语句
    std::shared_ptr<AstNode> targetNode;
public:
    ContinueStmt() : AstNode(Node_ContinueStmt) {}
    llvm::Value* Accept(Visitor *v) {
        return v->VisitContinueStmt(this);
    }

    static bool classof(const AstNode *node) {
        return node->GetKind() == Node_ContinueStmt;
    }
};


class VariableDecl : public AstNode {
public:
    // 变量声明的初始值表达式，有可能为空
    std::shared_ptr<AstNode> init;  

    VariableDecl() : AstNode(Node_VariableDecl) {}
    llvm::Value* Accept(Visitor *v) {
        return v->VisitVariableDeclExpr(this);
    }
    static bool classof(const AstNode *node) {
        return node->GetKind() == Node_VariableDecl;
    }
};

// 二元操作符枚举
enum class BinaryOp {
    add,
    sub,
    mul,
    div,
    mod,
    equal, // ==
    not_equal, // !=
    less,
    less_equal, // <=
    greater,
    greater_equal, // >=
    logic_or,
    logic_and,
    bitwise_or,
    bitwise_and,
    bitwise_xor,
    left_shift,
    right_shift,
    comma,       // 逗号表达式

    assign,
    add_assign,
    sub_assign,
    mul_assign,
    div_assign,
    mod_assign,
    bitwise_or_assign,
    bitwise_and_assign,
    bitwise_xor_assign,
    left_shift_assign,
    right_shift_assign
};

// 二元表达式
class BinaryExpr : public AstNode {
public:
    BinaryOp op;
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


class ThreeExpr : public AstNode {
public:
    std::shared_ptr<AstNode> cond;
    std::shared_ptr<AstNode> then;
    std::shared_ptr<AstNode> els;

    ThreeExpr() : AstNode(Node_ThreeExpr){}
    llvm::Value* Accept(Visitor *v) override {
        return v->VisitThreeExpr(this);
    }

    static bool classof(const AstNode *node) {
        return node->GetKind() == Node_ThreeExpr;
    }    
};


// 一元操作符枚举
enum class UnaryOp {
    positive, // 正号
    negative, // 负号
    deref,   // 指针解引用
    addr,    // 取地址
    inc,     // ++
    dec,     // --
    logic_not, // 逻辑取反，就是逻辑非：！
    bitwise_not  // 按位取反， 就是按位非：~
};

class UnaryExpr : public AstNode {
public:
    UnaryOp op;
    std::shared_ptr<AstNode> node;

    UnaryExpr() : AstNode(Node_UnaryExpr){}
    llvm::Value* Accept(Visitor *v) override {
        return v->VisitUnaryExpr(this);
    }

    static bool classof(const AstNode *node) {
        return node->GetKind() == Node_UnaryExpr;
    }
};


class SizeOfExpr : public AstNode {
public:
    std::shared_ptr<AstNode> node;
    std::shared_ptr<CType> type;

    SizeOfExpr() : AstNode(Node_SizeOfExpr){}
    llvm::Value* Accept(Visitor *v) override {
        return v->VisitSizeOfExpr(this);
    }

    static bool classof(const AstNode *node) {
        return node->GetKind() == Node_SizeOfExpr;
    }
};

// 后置++ 表达式
class PostIncExpr : public AstNode {
public:
    std::shared_ptr<AstNode> left;

    PostIncExpr() : AstNode(Node_PostIncExpr){}
    llvm::Value* Accept(Visitor *v) override {
        return v->VisitPostIncExpr(this);
    }

    static bool classof(const AstNode *node) {
        return node->GetKind() == Node_PostIncExpr;
    }
};

// 后置-- 表达式
class PostDecExpr : public AstNode {
public:
    std::shared_ptr<AstNode> left;

    PostDecExpr() : AstNode(Node_PostDecExpr){}
    llvm::Value* Accept(Visitor *v) override {
        return v->VisitPostDecExpr(this);
    }

    static bool classof(const AstNode *node) {
        return node->GetKind() == Node_PostDecExpr;
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

// 语法树根节点类型
class Program {
public:
    // std::vector<std::shared_ptr<AstNode>> stmtNodeVec;
    std::shared_ptr<AstNode> node;
};