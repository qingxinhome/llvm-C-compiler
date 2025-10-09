#include "scope.h"

Scope::Scope(){
    // 构造时加入一个根作用域
    envs.push_back(std::make_shared<Env>());
}

void Scope::EnterScope(){
    envs.push_back(std::make_shared<Env>());
}

void Scope::ExitScope(){
    envs.pop_back();
}
std::shared_ptr<Symbol> Scope::FindSymbol(llvm::StringRef name){
    // 反向遍历(从栈顶开始查找)
    for(std::vector<std::shared_ptr<Env>>::reverse_iterator it = envs.rbegin(); it != envs.rend(); it++){
        auto table = (*it)->variableSymbolTable;
        if (table.count(name) > 0) {
            return table[name];
        }
    }
    return nullptr;
}
std::shared_ptr<Symbol> Scope::FindSymbolInCurScope(llvm::StringRef name){
    auto curTable = envs.back()->variableSymbolTable;
    if (curTable.count(name) > 0) {
        return curTable[name];
    }
    return nullptr;
}
void Scope::AddSymbol(SymbolKind kind, CType *type, llvm::StringRef name){
    auto symbol = std::make_shared<Symbol>(kind, type, name);
    // 当你将一个对象赋值给新变量时，C++ 会创建该对象的完整拷贝，新变量拥有独立的内存，包含相同的状态（数据）。
    // 引用是现有对象的别名，操作引用等同于操作原对象（共享同一块内存）。
    auto &curTable = envs.back()->variableSymbolTable;
    curTable.insert({name, symbol});
}
