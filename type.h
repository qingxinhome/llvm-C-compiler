#pragma once

#include <memory>
#include <vector>
#include <llvm/IR/Type.h>


class CPrimaryType;
class CPointType;
class CArrayType;
class CRecordType;
class CFuncType;

// 类型访问者基类
class TypeVisitor {
public:
    virtual ~TypeVisitor() {};
    virtual llvm::Type* VisitPrimaryType(CPrimaryType *type) = 0;
    virtual llvm::Type* VisitPointType(CPointType *type) = 0;
    virtual llvm::Type* VisitArrayType(CArrayType *type) = 0;
    virtual llvm::Type* VisitRecordType(CRecordType *type) = 0;
    virtual llvm::Type* VisitFuncType(CFuncType *type) = 0;
};

// TagKind用于标识聚合类型的类别
enum class TagKind {
    KStruct,
    KUnion
};

class CType {
public:
    enum Kind {
        TY_Void,       // 基础类型 void
        TY_Char,       // 基础类型 char
        TY_UChar,      // 基础类型 unsigned char
        TY_Short,      // 基础类型 short
        TY_UShort,     // 基础类型 unsigned short
        TY_Int,        // 基础类型 int
        TY_UInt,       // 基础类型 unsigned int
        TY_Long,       // 基础类型 long
        TY_ULong,      // 基础类型 unsigned long
        TY_LLong,      // 基础类型 long long 
        TY_ULLong,     // 基础类型 unsigned long long 
        TY_Float,      // 基础类型 float
        TY_Double,     // 基础类型
        TY_LDouble,     // 基础类型 long double 扩展精度，**取决于编译器和平台**
        TY_Point,      // 指针类型
        TY_Array,      // 数组类型
        TY_Record,     // 聚合类型（struct or union
        TY_Func        // 函数类型
    };
public:
    CType(Kind kind, int size, int align, bool sign = true): kind(kind), size(size), align(align), sign(sign) {};
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

    bool IsIntegerType();

    bool IsFloatType();

    bool IsArithType();

    virtual llvm::Type* Accept(TypeVisitor *v) {
        return nullptr;
    }
public:
    // 使用静态成员变量来枚举C语言中的基础类型：
    // Note:静态成员变量必须在类内声明，在类外初始化
    static std::shared_ptr<CType> VoidType;
    static std::shared_ptr<CType> CharType;
    static std::shared_ptr<CType> UCharType;
    static std::shared_ptr<CType> ShortType; 
    static std::shared_ptr<CType> UShortType; 
    static std::shared_ptr<CType> IntType;
    static std::shared_ptr<CType> UIntType;
    static std::shared_ptr<CType> LongType;
    static std::shared_ptr<CType> ULongType;
    static std::shared_ptr<CType> LongLongType;
    static std::shared_ptr<CType> ULongLongType;
    static std::shared_ptr<CType> FloatType;
    static std::shared_ptr<CType> DoubleType;
    static std::shared_ptr<CType> LDoubleType;
public:
    // GenAnonyRecordName 用于为匿名结构体或者union生成一个名字
    static llvm::StringRef GenAnonyRecordName(TagKind tagKind);
protected:
    Kind kind;
    int size;      // 类型占用空间大小(字节)
    int align;     // 对齐数， 通常内置类型的对齐数和类型的大小相同
    bool sign{true}; //是否为有符号， 默认是有符号数
};


// 基础类型
class CPrimaryType : public CType {
public:
    CPrimaryType(Kind kind, int size, int align, bool sign) : CType(kind, size, align, sign){};
    
    llvm::Type* Accept(TypeVisitor *v) override{
        return v->VisitPrimaryType(this);
    }
    
    static bool classof(const CType *ty) {
        return ty->GetKind() == Kind::TY_Char || ty->GetKind() == Kind::TY_UChar ||
            ty->GetKind() == Kind::TY_Int || ty->GetKind() == Kind::TY_UInt ||
            ty->GetKind() == Kind::TY_Long || ty->GetKind() == Kind::TY_ULong ||
            ty->GetKind() == Kind::TY_LLong || ty->GetKind() == Kind::TY_ULLong ||
            ty->GetKind() == Kind::TY_Float || ty->GetKind() == Kind::TY_Double || ty->GetKind() == Kind::TY_LDouble;
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
    int elementCount{-1};                    /* 数组元素个数*/
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

    void SetElementCount(int count) {
        this->elementCount = count;
        // 同时要更新数组类型的大小
        this->size = elementCount*elementType->GetSize();
    }

    llvm::Type* Accept(TypeVisitor *v) override{
        return v->VisitArrayType(this);
    }

    static bool classof(const CType *ty) {
        return ty->GetKind() == Kind::TY_Array;
    }

};



// Member表示聚合类型的成员
struct Member {
    std::shared_ptr<CType> ty;       // 成员的类型
    llvm::StringRef name;            // 成员的名字
    int offset;                      // 成员地址偏移
    int elemIdx;                     // 元素的位置
};


class CRecordType : public CType {
private:
    llvm::StringRef name;          // 聚合类型的名字，如结构体名
    std::vector<Member> members;   // 聚合类型的成员
    int maxElementIdx;             // 占用空间最大的元素位置
    TagKind tagKind;               // 聚合类型的类别(struct or union)
public:
    CRecordType(llvm::StringRef name, const std::vector<Member> &members, TagKind tagKind);
    
    // 函数返回类型是const llvm::StringRef，
    // 这表示返回的llvm::StringRef对象是只读的，调用者无法通过返回值修改name
    const llvm::StringRef GetName() {
        return name;
    }

    const std::vector<Member>& GetMembers() {
        return members;
    }

    void SetMembers(const std::vector<Member>& members);

    const TagKind GetTagKind() {
        return tagKind;
    }

    int GetMaxElementIdx() {
        return maxElementIdx;
    }

    llvm::Type* Accept(TypeVisitor *v) override{
        return v->VisitRecordType(this);
    }

    static bool classof(const CType *ty) {
        return ty->GetKind() == Kind::TY_Record;
    }
private:
    void UpdateStructOffset();
    void UpdateUnionOffset();
};

struct Parameter {
    std::shared_ptr<CType> type;
    llvm::StringRef name;
};

class CFuncType : public CType {
private:
    std::shared_ptr<CType> retType;     // 函数返回值类型
    std::vector<Parameter> params;      // 函数参数列表
    llvm::StringRef name;               // 函数名字
    bool isVarArg{false};               // 是否有可变参数， 默认初始值为false
public:
    bool hasBody{false};                //  是否有函数体， 如果没有函数体说明是函数声明
public:
    CFuncType(std::shared_ptr<CType> retType, const std::vector<Parameter> &params, llvm::StringRef name, bool isVarArg);

    const llvm::StringRef GetName() {
        return name;
    }

    const std::vector<Parameter>& GetParams() {
        return params;
    }

    bool IsVarArg() {
        return isVarArg;
    }

    const std::shared_ptr<CType> GetRetType() {
        return retType;
    }

    llvm::Type* Accept(TypeVisitor *v) override{
        return v->VisitFuncType(this);
    }

    static bool classof(const CType *ty) {
        return ty->GetKind() == Kind::TY_Func;
    }
};