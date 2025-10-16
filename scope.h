#pragma once

#include <vector>
#include <memory>
#include <llvm/ADT/StringMap.h>
#include "type.h"

// 符号类别
enum SymbolKind {
    LocalVariable
};

// 符号
class Symbol {
private:
    SymbolKind symKind;
    std::shared_ptr<CType> type;
    llvm::StringRef name;
public:
    Symbol(SymbolKind kind, std::shared_ptr<CType> type, llvm::StringRef name) : symKind(kind), type(type), name(name) {}
    std::shared_ptr<CType> GetType(){return type;}
};


// Env对应一个作用域， 其实就是符号表
class Env {
public:
    llvm::StringMap<std::shared_ptr<Symbol>> variableSymbolTable;
};


// 管理作用域栈，包含 std::vector<std::shared_ptr<Env>> envs, Scope其实就是符号表栈
class Scope {
private:
    std::vector<std::shared_ptr<Env>> envs;
public:
    Scope();
    void EnterScope();
    void ExitScope();
    std::shared_ptr<Symbol> FindSymbol(llvm::StringRef name);
    std::shared_ptr<Symbol> FindSymbolInCurScope(llvm::StringRef name);
    void AddSymbol(SymbolKind kind, std::shared_ptr<CType> type, llvm::StringRef name);
};