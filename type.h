#pragma once

#include <memory>

enum TypeKind {
    Int
};


// 单例模式
class CType {
public:
    CType(TypeKind kind, int size, int align): kind(kind), size(size), align(align) {};
    // static CType* GetIntTy(); 
    static std::shared_ptr<CType> IntType;  // 静态成员变量必须在类内声明，在类外初始化
private:
    TypeKind kind;
    int size;      // 类型占用空间大小(字节)
    int align;     // 对齐数， 通常内置类型的对齐数和类型的大小相同
};