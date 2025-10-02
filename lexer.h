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
private:
    const char *CurBufPtr;
    const char *LineHeadPtr;
    const char *BufEnd;
    int row;
};