#pragma once

#include <memory>
#include <llvm/IR/Type.h>


class CPrimaryType;
class CPointType;
class CArrayType;

// 类型访问者基类
class TypeVisitor {
public:
    virtual ~TypeVisitor() {};
    virtual llvm::Type* VisitPrimaryType(CPrimaryType *type) = 0;
    virtual llvm::Type* VisitPointType(CPointType *type) = 0;
    virtual llvm::Type* VisitArrayType(CArrayType *type) = 0;
};



class CType {
public:
    enum Kind {
        TY_Int,      // 基础类型
        TY_Point,    // 指针类型
        TY_Array     // 数组类型
    };
public:
    CType(Kind kind, int size, int align): kind(kind), size(size), align(align) {};
    virtual ~CType(){}  // 虚析构函数
    
    const Kind GetKind() const {
        return kind;
    }

    const int GetSize() const {
        return size;
    }

    const int GetAlign() {
        return align;
    }

    virtual llvm::Type* Accept(TypeVisitor *v) {
        return nullptr;
    }
public:
    // 使用静态成员变量来枚举Int类型
    static std::shared_ptr<CType> IntType;  // 静态成员变量必须在类内声明，在类外初始化
private:
    Kind kind;
    int size;      // 类型占用空间大小(字节)
    int align;     // 对齐数， 通常内置类型的对齐数和类型的大小相同
};


// 基础类型
class CPrimaryType : public CType {
public:
    CPrimaryType(Kind kind, int size, int align) : CType(kind, size, align){};
    
    llvm::Type* Accept(TypeVisitor *v) override{
        return v->VisitPrimaryType(this);
    }
    
    static bool classof(const CType *ty) {
        return ty->GetKind() == Kind::TY_Int;
    }
};

class CPointType : public CType{
private:
    std::shared_ptr<CType> baseType;
public:
    CPointType(std::shared_ptr<CType> baseType) : CType(Kind::TY_Point, 8, 8), baseType(baseType) {};
    
    std::shared_ptr<CType> GetBaseType() {
        return baseType;
    }

    llvm::Type* Accept(TypeVisitor *v) override{
        return v->VisitPointType(this);
    }

    static bool classof(const CType *ty) {
        return ty->GetKind() == Kind::TY_Point;
    }
};


class CArrayType : public CType {
private:
    std::shared_ptr<CType> elementType;      /* 数组元素类型*/
    int elementCount;                        /* 数组元素个数*/
public:
    CArrayType(std::shared_ptr<CType> elemType, int elemCount) : \
        CType(Kind::TY_Array, elemCount*elemType->GetSize(), elemType->GetAlign()), \
        elementType(elemType), elementCount(elemCount) {};
    
    std::shared_ptr<CType> GetElementType() {
        return elementType;
    }

    int GetElementCount() {
        return elementCount;
    }

    llvm::Type* Accept(TypeVisitor *v) override{
        return v->VisitArrayType(this);
    }

    static bool classof(const CType *ty) {
        return ty->GetKind() == Kind::TY_Array;
    }

};
