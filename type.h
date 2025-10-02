#pragma once


enum TypeKind {
    Int
};


// 单例模式
class CType {
public:
    static CType* GetIntTy(); 
private:
    CType(TypeKind kind, int size, int align): kind(kind), size(size), align(align) {};
private:
    TypeKind kind;
    int size;      // 类型占用空间大小(字节)
    int align;     // 对齐数， 通常内置类型的对齐数和类型的大小相同
};