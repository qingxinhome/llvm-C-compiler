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
class ReturnStmt;
class BlockStmt;
class VariableDecl;
class FunctionDecl;
class VariableAccessExpr;
class BinaryExpr;
class ThreeExpr;
class UnaryExpr;
class SizeOfExpr;
class PostDecExpr;
class PostIncExpr;
class PostSubscript;
class PostMemberDotExpr;
class PostMemberArrowExpr;
class PostFunctionCallExpr;
class NumberExpr;
class StringExpr;
class DoWhileStmt;
class SwitchStmt;
class CaseStmt;
class DefaultStmt;



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
    virtual llvm::Value* VisitReturnStmt(ReturnStmt *stmt) = 0;
    virtual llvm::Value* VisitBlockStmt(BlockStmt *blockstmt) = 0;
    virtual llvm::Value* VisitDoWhileStmt(DoWhileStmt *stmt) = 0;
    virtual llvm::Value* VisitSwitchStmt(SwitchStmt *stmt) = 0;
    virtual llvm::Value* VisitCaseStmt(CaseStmt *stmt) = 0;
    virtual llvm::Value* VisitDefaultStmt(DefaultStmt *stmt) = 0;
    virtual llvm::Value* VisitVariableDeclExpr(VariableDecl *decl) = 0;
    virtual llvm::Value* VisitFunctionDeclExpr(FunctionDecl *decl) = 0;
    virtual llvm::Value* VisitVariableAccessExpr(VariableAccessExpr *expr) = 0;
    virtual llvm::Value* VisitBinaryExpr(BinaryExpr *binaryExpr) = 0;
    virtual llvm::Value* VisitThreeExpr(ThreeExpr *threeExpr) = 0;
    virtual llvm::Value* VisitUnaryExpr(UnaryExpr *unaryExpr) = 0;
    virtual llvm::Value* VisitSizeOfExpr(SizeOfExpr *sizeofExpr) = 0;
    virtual llvm::Value* VisitPostIncExpr(PostIncExpr *postIncExpr) = 0;
    virtual llvm::Value* VisitPostDecExpr(PostDecExpr *postDecExpr) = 0;
    virtual llvm::Value* VisitPostSubscript(PostSubscript *expr) = 0;
    virtual llvm::Value* VisitPostMemberDotExpr(PostMemberDotExpr *expr) = 0;
    virtual llvm::Value* VisitPostMemberArrowExpr(PostMemberArrowExpr *expr) = 0;
    virtual llvm::Value* VisitPostFunctionCallExpr(PostFunctionCallExpr *expr) = 0;
    virtual llvm::Value* VisitNumberExpr(NumberExpr *expr) = 0;
    virtual llvm::Value* VisitStringExpr(StringExpr *expr) = 0;
};


// llvm rtti
// 定义语法树节点的抽象基类（对应表达式和语句）
class AstNode {
public:
    enum Kind{
        Node_IfStmt,
        Node_ReturnStmt,
        Node_ForStmt,
        Node_BreakStmt,
        Node_ContinueStmt,
        Node_BlockStmt,
        Node_DoWhileStmt,
        Node_SwithcStmt,
        Node_CaseStmt,
        Node_DefaultStmt,
        Node_DeclareStmt,
        Node_VariableDecl,
        Node_FunctionDecl,
        Node_BinaryExpr,
        Node_ThreeExpr,
        Node_UnaryExpr,
        Node_SizeOfExpr,
        Node_PostIncExpr,
        Node_PostDecExpr,
        Node_PostSubscript,
        Node_PostMemberDotExpr,
        Node_PostMemberArrowExpr,
        Node_PostFunctionCallExpr,
        Node_NumberExpr,
        Node_StringExpr,
        Node_VariableAccessExpr
    };
private:
    const Kind kind;
public:
    AstNode(Kind kind) : kind(kind) {}
    virtual ~AstNode(){}

    std::shared_ptr<CType> type;         /* 语句或者表达式的类型*/
    Token token;                         /* 语句或者表达式的当前token*/

    // 是否为左值
    bool isLValue{false}; 

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

class ReturnStmt : public AstNode {
public:
    // ruturn 语句返回一个表达式， 也有可能是空
    std::shared_ptr<AstNode> expr{nullptr};
public:
    ReturnStmt() : AstNode(Node_ReturnStmt) {}
    llvm::Value * Accept(Visitor *v) {
        return v->VisitReturnStmt(this);
    }

    static bool classof(const AstNode *node) {
        return node->GetKind() == Node_ReturnStmt;
    }
};


class DoWhileStmt : public AstNode {
public:
    std::shared_ptr<AstNode> body;   // do while 的循环体
    std::shared_ptr<AstNode> cond;   // do while 的条件表达式
public:
    DoWhileStmt() : AstNode(Node_DoWhileStmt) {}
    llvm::Value * Accept(Visitor *v) {
        return v->VisitDoWhileStmt(this);
    }

    static bool classof(const AstNode *node) {
        return node->GetKind() == Node_DoWhileStmt;
    }
};

class SwitchStmt : public AstNode {
public:
    std::shared_ptr<AstNode> expr;      // switch语句的判断表达式
    std::shared_ptr<AstNode> stmt;      // switch语句中body语句
    // 用来标识是否包含default语句（switch语句中不能包含多个default语句）
    std::shared_ptr<AstNode> defaultStmt{nullptr};  
public:
    SwitchStmt() : AstNode(Node_SwithcStmt) {}
    llvm::Value * Accept(Visitor *v) {
        return v->VisitSwitchStmt(this);
    }

    static bool classof(const AstNode *node) {
        return node->GetKind() == Node_SwithcStmt;
    }
};

class CaseStmt : public AstNode {
public:
    // case语句的表达式
    std::shared_ptr<AstNode> expr;
    // case语句body语句体
    std::shared_ptr<AstNode> stmt;
public:
    CaseStmt() : AstNode(Node_CaseStmt) {}
    llvm::Value * Accept(Visitor *v) {
        return v->VisitCaseStmt(this);
    }

    static bool classof(const AstNode *node) {
        return node->GetKind() == Node_CaseStmt;
    }
};

class DefaultStmt : public AstNode {
public:
    // default语句的语句体
    std::shared_ptr<AstNode> stmt;
public:
    DefaultStmt() : AstNode(Node_DefaultStmt) {}
    llvm::Value * Accept(Visitor *v) {
        return v->VisitDefaultStmt(this);
    }

    static bool classof(const AstNode *node) {
        return node->GetKind() == Node_DefaultStmt;
    }
};


class FunctionDecl : public AstNode {
public:
    // 函数声明语句，有可能是一个函数声明，也有可能是一个包含block语句的函数定义
    // 函数的参数和返回值信息会作为函数的类型CFuncType进行存储
    std::shared_ptr<AstNode> blockStmt{nullptr};
public:
    FunctionDecl() : AstNode(Node_FunctionDecl) {}
    llvm::Value * Accept(Visitor *v) {
        return v->VisitFunctionDeclExpr(this);
    }

    static bool classof(const AstNode *node) {
        return node->GetKind() == Node_FunctionDecl;
    }
};

/// VariableDecl变量声明节点， 可以针对局部变量的声明，也可以针对于全局变量的声明
class VariableDecl : public AstNode {
public:
    // 数组初始值：数组元素的槽位(地址)和对应元素的关联关系
    struct InitValue {
        std::shared_ptr<CType> decType;     /* 数组元素类型 */
        std::shared_ptr<AstNode> value;     /* 数组元素值表达式 */
        // int a[3][4] = {{1,2,3,4}, {5,6,7,8}, {9, 10, 11, 12}}
        // 那么元素 10 的 offset位置为：{2, 1}
        std::vector<int> offsetList;
    };

    // 变量声明的初始值表达式,可以是单值，也可以是列表；有可能为空
    // 这里使用std::vector存放所有初始值，不是基于其原始结构， 而是展平存储在vector中
    std::vector<std::shared_ptr<InitValue>> initValues;

    // isGlobal属性标识是否为全局变量
    bool isGlobal{false};

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
    positive,    // 正号
    negative,    // 负号
    deref,       // 指针解引用
    addr,        // 取地址
    inc,         // ++
    dec,         // --
    logic_not,   // 逻辑取反，就是逻辑非：！
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
    std::shared_ptr<CType> ty;

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

// 数组访问下标表达式(后缀表达式的一种). 如：
// a[5]
// a -> left
// [5] -> node
class PostSubscript : public AstNode {
public:
    std::shared_ptr<AstNode> left;    // 存放基址
    std::shared_ptr<AstNode> node;    // 存放偏移量

    PostSubscript() : AstNode(Node_PostSubscript){}
    llvm::Value* Accept(Visitor *v) override {
        return v->VisitPostSubscript(this);
    }

    static bool classof(const AstNode *node) {
        return node->GetKind() == Node_PostSubscript;
    }
};

// 后缀表达式-- 成员选择（对象）
class PostMemberDotExpr : public AstNode {
public:
    std::shared_ptr<AstNode> left;    // 存放基址
    Member member;                    // 存放结构体成员

    PostMemberDotExpr() : AstNode(Node_PostMemberDotExpr){}
    llvm::Value* Accept(Visitor *v) override {
        return v->VisitPostMemberDotExpr(this);
    }

    static bool classof(const AstNode *node) {
        return node->GetKind() == Node_PostMemberDotExpr;
    }
};

// 后缀表达式-- 成员选择（指针）
class PostMemberArrowExpr : public AstNode {
public:
    std::shared_ptr<AstNode> left;    // 存放基址
    Member member;                    // 存放结构体成员

    PostMemberArrowExpr() : AstNode(Node_PostMemberArrowExpr){}
    llvm::Value* Accept(Visitor *v) override {
        return v->VisitPostMemberArrowExpr(this);
    }

    static bool classof(const AstNode *node) {
        return node->GetKind() == Node_PostMemberArrowExpr;
    }
};

// 函数调用表达式也是后缀表达式的一种
// add(1,2);
class PostFunctionCallExpr : public AstNode {
public:
    std::shared_ptr<AstNode> left;               // 函数名
    std::vector<std::shared_ptr<AstNode>> args;  // 函数实参列表

    PostFunctionCallExpr() : AstNode(Node_PostFunctionCallExpr){}
    llvm::Value* Accept(Visitor *v) override {
        return v->VisitPostFunctionCallExpr(this);
    }

    static bool classof(const AstNode *node) {
        return node->GetKind() == Node_PostFunctionCallExpr;
    }
};


// 因子表达式
class NumberExpr : public AstNode {
public:
    int value;

    NumberExpr() : AstNode(Node_NumberExpr){}

    llvm::Value* Accept(Visitor *v) override {
        return v->VisitNumberExpr(this);
    }
    static bool classof(const AstNode *node) {
        return node->GetKind() == Node_NumberExpr;
    }
};

/// 字符串表达式
class StringExpr : public AstNode {
public:
    StringExpr() : AstNode(Node_StringExpr){}

    llvm::Value* Accept(Visitor *v) override {
        return v->VisitStringExpr(this);
    }
    static bool classof(const AstNode *node) {
        return node->GetKind() == Node_StringExpr;
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
    llvm::StringRef fileName;     // 程序的源文件名

    // 程序是由一个或多个全局变量或者函数组成, 即：
    // prog          ::= (external-decl)+
    // external-decl ::= func-def | decl-stmt
    std::vector<std::shared_ptr<AstNode>> externalDecls;
};