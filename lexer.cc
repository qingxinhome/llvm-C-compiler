#include "lexer.h"


// 判断当前字符是否为空格或者换行
bool IsWhiteSpace(char ch) {
    return ch == ' ' || ch == '\r' || ch == '\n';
}

// 判断当前字符是否为数字
bool IsDigit(char ch) {
    return (ch >= '0' && ch <= '9');
}


Lexer::Lexer(llvm::StringRef sourceCode) {
    LineHeadPtr = sourceCode.begin();
    CurBufPtr = sourceCode.begin();
    BufEnd = sourceCode.end();
    row = 1;
}

void Lexer::NextToken(Token &token) {
    // 1. 过滤空格
    while(IsWhiteSpace(*CurBufPtr)) {
        if (*CurBufPtr == '\n' ) {
            row++;
            LineHeadPtr = CurBufPtr + 1;
        }
        CurBufPtr++;
    }
    token.row = row;
    token.col = CurBufPtr - LineHeadPtr + 1;
    

    
    // 2. 判断是否到达结尾
    if (CurBufPtr >= BufEnd) {
        token.tokenType = TokenType::eof;
        return;
    }

    const char *start = CurBufPtr;

    if (IsDigit(*CurBufPtr)) {
        int number = 0;
        int len = 0;
        while(IsDigit(*CurBufPtr)) {
            number =  number * 10 + (*CurBufPtr-'0');
            CurBufPtr++;
            len++;
        }
        token.tokenType = TokenType::number;
        token.value = number;
        token.content = llvm::StringRef(start, len);
    } else {
        switch (*CurBufPtr)
        {
        case '+':
            token.tokenType = TokenType::plus;
            token.content = llvm::StringRef(start, 1);
            CurBufPtr++;
            break;
        case '-':
            token.tokenType = TokenType::minus;
            token.content = llvm::StringRef(start, 1);
            CurBufPtr++;
            break;
        case '*':
            token.tokenType = TokenType::star;
            token.content = llvm::StringRef(start, 1);
            CurBufPtr++;
            break;
        case '/':
            token.tokenType = TokenType::slash;
            token.content = llvm::StringRef(start, 1);
            CurBufPtr++;
            break;
        case ';':
            token.tokenType = TokenType::semi;
            token.content = llvm::StringRef(start, 1);
            CurBufPtr++;
            break;
        case '(':
            token.tokenType = TokenType::l_parent;
            token.content = llvm::StringRef(start, 1);
            CurBufPtr++;
            break;
        case ')':
            token.tokenType = TokenType::r_parent;
            token.content = llvm::StringRef(start, 1);
            CurBufPtr++;
            break;
        default:
            token.tokenType = TokenType::unknown;
            token.content = llvm::StringRef(start, 1);
            CurBufPtr++;
            break;
        }
    }
}


