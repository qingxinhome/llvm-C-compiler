#pragma once

#include <llvm/ADT/StringRef.h>
#include <llvm/Support/raw_ostream.h>
#include "type.h"

// char stream -> token

enum class TokenType {
    unknown,
    number,
    identifier,
    kw_int, // int type
    minus, // -
    plus,  // +
    star,  // *
    slash, // '/'
    l_parent, // {
    r_parent, // }
    semi,  // ;
    equal, // =
    comma, // ,
    eof   // end
};


class Token {
public:
    int row, col;
    TokenType tokenType;
    int value;
    CType *type;
    llvm::StringRef content;

    Token() {
        row = col = -1;
        tokenType = TokenType::unknown;
        value = -1;
    }
    void Dump() {
        llvm::outs() << "{ " << content << " ,row = " << row << ", col = " << col << "}\n";
    }
};

class Lexer {
public:
    Lexer(llvm::StringRef sourceCode);
    void NextToken(Token &token);

    void SaveState();
    void RestoreState();
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