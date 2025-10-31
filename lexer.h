#pragma once

#include <llvm/ADT/StringRef.h>
#include <llvm/Support/raw_ostream.h>
#include "type.h"
#include "diag_engine.h"

// char stream -> token

enum class TokenType {
    number,
    identifier,
    kw_int,  // int type
    kw_if,   // if
    kw_else, // else
    kw_for,  // for
    kw_break, // break
    kw_continue, // continue
    kw_sizeof,   // sizeof
    kw_struct,   // struct
    kw_union,    // union
    kw_return,   // return
    minus, // -
    plus,  // +
    star,  // *
    slash, // '/'
    l_parent, // (
    r_parent, // )
    l_brace,  // {
    r_brace,  // }
    semi,  // ;
    equal, // =
    comma, // ,
    equal_equal, // ==
    not_equal,  // !=
    less,       // <
    less_equal,  // <=
    greater,     // >
    greater_equal, // >=
    pipepipe,     // ||
    pipe,       // |
    ampamp,     // &&
    amp,        // &
    caret,      // ^
    percent,    // %
    less_less,  // <<
    greater_greater, // >>
    plus_plus,      // ++
    minus_minus,    // --
    tilde,          // ~
    exclaim,        // !
    plus_equal,     // +=
    minus_equal,    // -=
    star_equal,     // *=
    slash_equal,    // /=
    percent_equal,  // %=
    less_less_equal, // <<=
    greater_greater_equal, // >>=
    amp_equal,       // &=
    caret_equal,     // ^=
    pipe_equal,      // |=
    question,        // ?
    colon,           // :
    l_bracket,       // [
    r_bracket,       // ]
    dot,             // .
    arrow,           // ->
    eof   // end
};


class Token {
public:
    TokenType tokenType;
    int row, col;
    
    int value;   // for number
    
    // CType *type; 
    std::shared_ptr<CType> type; // for built-in type, 内置数据类型通过词法分析即可得到
    
    const char *ptr;   // for debug && diag
    int len;
public:
    void Dump() {
        llvm::outs() << "{ " << llvm::StringRef(ptr, len) << " ,row = " << row << ", col = " << col << "}\n";
    }
    // 静态成员方法
    static llvm::StringRef GetSpellingText(TokenType tokenType);
};

class Lexer {
private:
    llvm::SourceMgr &mgr;    // 用于存放源文件
    DiagEngine &diagEngine;  // 诊断引擎
public:
    Lexer(llvm::SourceMgr &mgr, DiagEngine &diagEngine) : mgr(mgr), diagEngine(diagEngine) {
        unsigned int id = mgr.getMainFileID();
        llvm::StringRef buf = mgr.getMemoryBuffer(id)->getBuffer();
        CurBufPtr = buf.begin();
        LineHeadPtr = buf.begin();
        BufEnd = buf.end();
        row = 1;
    }
    void NextToken(Token &token);

    void SaveState();
    void RestoreState();
    DiagEngine& GetDiagEngine() const{
        return diagEngine;
    }
private:
    bool StartWith(const char* p);
private:
    const char *CurBufPtr;
    const char *LineHeadPtr;
    const char *BufEnd;
    int row;

    // 定义一个token状态，用于恢复到某个位置
    struct State {
        const char *CurBufPtr;
        const char *LineHeadPtr;
        const char *BufEnd;
        int row;
    };
    State state;
};