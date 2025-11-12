#pragma once

#include <vector>
#include <memory>
#include <llvm/ADT/StringMap.h>
#include "type.h"

// 符号类别
enum SymbolKind {
    obj,      // variable, function
    ktypedef, // typedef type
    tag       // struct or union type name
};

// 符号
class Symbol {
private:
    SymbolKind symKind;
    std::shared_ptr<CType> type;
    llvm::StringRef name;
public:
    Symbol(SymbolKind kind, std::shared_ptr<CType> type, llvm::StringRef name) : symKind(kind), type(type), name(name) {}
    std::shared_ptr<CType> GetType(){
        return type;
    }
    SymbolKind GetKind() {
        return symKind;
    }
};


// Env对应一个作用域， 其实就是符号表
class Env {
public:
    /*
        在c语言中，struct和union有单独的命名空间管理. 也就是说struct和uinon的名字
        可以和变量的名字相同；因此变量 和 结构体/union 的符号表需要分开管理
    */
    llvm::StringMap<std::shared_ptr<Symbol>> objSymbolTable;     // object符号表用于存放变量名/函数名符号
    llvm::StringMap<std::shared_ptr<Symbol>> tagSymbolTable;     // tag符号表用于存放struct或union类型名符号
};


// 管理作用域栈，包含 std::vector<std::shared_ptr<Env>> envs, Scope其实就是符号表栈
class Scope {
private:
    std::vector<std::shared_ptr<Env>> envs;
public:
    Scope();
    void EnterScope();
    void ExitScope();
    std::shared_ptr<Symbol> FindObjSymbol(llvm::StringRef name);
    std::shared_ptr<Symbol> FindObjSymbolInCurScope(llvm::StringRef name);
    void AddObjSymbol(std::shared_ptr<CType> type, llvm::StringRef name);
    void AddTypedefSymbol(std::shared_ptr<CType> type, llvm::StringRef name);


    std::shared_ptr<Symbol> FindTagSymbol(llvm::StringRef name);
    std::shared_ptr<Symbol> FindTagSymbolInCurScope(llvm::StringRef name);
    void AddTagSymbol(std::shared_ptr<CType> type, llvm::StringRef name);
};