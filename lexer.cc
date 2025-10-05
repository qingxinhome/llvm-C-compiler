#include "lexer.h"


// 判断当前字符是否为空格或者换行
bool IsWhiteSpace(char ch) {
    return ch == ' ' || ch == '\r' || ch == '\n';
}

// 判断当前字符是否为数字
bool IsDigit(char ch) {
    return (ch >= '0' && ch <= '9');
}

bool IsLetter(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_';
}


void Lexer::NextToken(Token &token) {
    // 1. 过滤空格和换行
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

    // 3. 获取token的起始位置
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
        token.type = CType::GetIntTy();

    } else if (IsLetter(*CurBufPtr)) {
        while(IsLetter(*CurBufPtr) || IsDigit(*CurBufPtr)) {
            CurBufPtr++;
        }
        token.tokenType = TokenType::identifier;
        token.content = llvm::StringRef(start, CurBufPtr - start);
        if (token.content == "int") {
            token.tokenType = TokenType::kw_int;
        }
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
        case '=':
            token.tokenType = TokenType::equal;
            token.content = llvm::StringRef(start, 1);
            CurBufPtr++;
            break;
        case ',':
            token.tokenType = TokenType::comma;
            token.content = llvm::StringRef(start, 1);
            CurBufPtr++;
            break;
        default:
            token.tokenType = TokenType::unknown;
            token.content = llvm::StringRef(start, 1);
            // llvm::outs() << "unkown char " << *CurBufPtr << "\n";
            diagEngine.Report(llvm::SMLoc::getFromPointer(CurBufPtr), diag::err_unknown_char, *CurBufPtr);
            CurBufPtr++;
            break;
        }
    }
}


void Lexer::SaveState() {
    state.CurBufPtr = CurBufPtr;
    state.LineHeadPtr = LineHeadPtr;
    state.BufEnd = BufEnd;
    state.row = row;
}

void Lexer::RestoreState() {
    CurBufPtr = state.CurBufPtr;
    LineHeadPtr = state.LineHeadPtr;
    BufEnd = state.BufEnd;
    row = state.row;
}