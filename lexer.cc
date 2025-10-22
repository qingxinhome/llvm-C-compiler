#include "lexer.h"

/*
    number,
    identifier,
    kw_int, // int type
    kw_if,   // if
    kw_else, // else
    kw_for,  // for
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
    greater_greater // >>
*/
llvm::StringRef Token::GetSpellingText(TokenType tokenType) {
    switch (tokenType)
    {
    case TokenType::kw_int:
        return "int";
    case TokenType::kw_if:
        return "if";
    case TokenType::kw_else:
        return "else";
    case TokenType::kw_for:
        return "for";
    case TokenType::kw_break:
        return "break";
    case TokenType::kw_continue:
        return "continue";
    case TokenType::kw_sizeof:
        return "sizeof";
    case TokenType::minus:
        return "-";
    case TokenType::plus:
        return "+";
    case TokenType::star:
        return "*";
    case TokenType::slash:
        return "/";
    case TokenType::l_parent:
        return "(";
    case TokenType::r_parent:
        return ")";
    case TokenType::l_brace:
        return "{";
    case TokenType::r_brace:
        return "}";
    case TokenType::semi:
        return ";";
    case TokenType::equal:
        return "=";
    case TokenType::comma:
        return ",";
    case TokenType::number:
        return "number";
    case TokenType::equal_equal: // ==
        return "==";
    case TokenType::not_equal:  // !=
        return "!=";
    case TokenType::less:       // <
        return "<";
    case TokenType::less_equal:  // <=
        return "<=";
    case TokenType::greater:     // >
        return ">";
    case TokenType::greater_equal: // >=
        return ">=";
    case TokenType::pipepipe:
        return "||";
    case TokenType::pipe:
        return "|";
    case TokenType::ampamp:
        return "&&";
    case TokenType::amp:
        return "&";
    case TokenType::caret:
        return "^";
    case TokenType::percent:
        return "%";
    case TokenType::less_less:
        return "<<";
    case TokenType::greater_greater:
        return ">>";
    case TokenType::plus_plus:      
        return "++";
    case TokenType::minus_minus:    
        return "--";
    case TokenType::tilde:          
        return "~";
    case TokenType::exclaim:        
        return "!";
    case TokenType::plus_equal:
        return "+=";
    case TokenType::minus_equal:
        return "-=";
    case TokenType::star_equal:
        return "*=";
    case TokenType::slash_equal:
        return "/=";
    case TokenType::less_less_equal:
        return "<<=";
    case TokenType::greater_greater_equal:
        return ">>=";
    case TokenType::amp_equal:
        return "&=";
    case TokenType::caret_equal:
        return "^=";
    case TokenType::pipe_equal:
        return "|=";
    case TokenType::question:
        return  "?";
    case TokenType::colon:
        return ":";
    case TokenType::l_bracket:
        return "[";
    case TokenType::r_bracket:
        return "]";
    case TokenType::identifier:
        return "identifier";
    default:
        llvm::llvm_unreachable_internal();
    }
}

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

bool Lexer::StartWith(const char* p) {
    return !strncmp(CurBufPtr, p, strlen(p));
}

void Lexer::NextToken(Token &token) {
    // 1. 过滤空格和换行, 以及注释
    while(IsWhiteSpace(*CurBufPtr) || StartWith("//") || StartWith("/*")) {
        // 过滤单行注释
        if (StartWith("//")) {
            while (*CurBufPtr != '\n') {
                CurBufPtr++;
            }
            if (*CurBufPtr == '\n' ) {
                row++;
                LineHeadPtr = CurBufPtr + 1;
            }
            CurBufPtr++;
        }

        // 过滤多行注释
        if (StartWith("/*")) {
            while (CurBufPtr[0] != '*' || CurBufPtr[1] != '/') {
                if (*CurBufPtr == '\n' ) {
                    row++;
                    LineHeadPtr = CurBufPtr + 1;
                }
                CurBufPtr++;
            }
            CurBufPtr += 2;
        }

        if (*CurBufPtr == '\n' ) {
            row++;
            LineHeadPtr = CurBufPtr + 1;
        }
        CurBufPtr++;
    }


    // 2. 判断是否到达结尾
    if (CurBufPtr >= BufEnd) {
        token.tokenType = TokenType::eof;
        return;
    }
    
    // 设置token的当前行和位置信息
    token.row = row;
    token.col = CurBufPtr - LineHeadPtr + 1;

    // 3. 获取token的起始位置
    const char *startPtr = CurBufPtr;

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
        token.type = CType::IntType;
        token.ptr = startPtr;
        token.len = CurBufPtr - startPtr;
    } else if (IsLetter(*CurBufPtr)) {
        while(IsLetter(*CurBufPtr) || IsDigit(*CurBufPtr)) {
            CurBufPtr++;
        }
        token.tokenType = TokenType::identifier;
        token.ptr = startPtr;
        token.len = CurBufPtr - startPtr;
        llvm::StringRef content = llvm::StringRef(token.ptr, token.len);

        if (content == "int") {
            token.tokenType = TokenType::kw_int;
        } else if(content == "if") {
            token.tokenType = TokenType::kw_if;
        } else if(content == "else") {
            token.tokenType = TokenType::kw_else;
        } else if (content == "for") {
            token.tokenType = TokenType::kw_for;
        } else if (content == "break") {
            token.tokenType = TokenType::kw_break;
        } else if (content == "continue") {
            token.tokenType = TokenType::kw_continue;
        } else if (content == "sizeof") {
            token.tokenType = TokenType::kw_sizeof;
        }
    } else {
        switch (*CurBufPtr)
        {
        case '+':
            if (CurBufPtr[1] == '+') {
                token.tokenType = TokenType::plus_plus;
                token.ptr = startPtr;
                token.len = 2;
                CurBufPtr+=2;
            } else if (CurBufPtr[1] == '=') {
                token.tokenType = TokenType::plus_equal;
                token.ptr = startPtr;
                token.len = 2;
                CurBufPtr+=2;
            } else {
                token.tokenType = TokenType::plus;
                token.ptr = startPtr;
                token.len = 1;
                CurBufPtr++;
            }
            break;
        case '-':
            if (CurBufPtr[1] == '-') {
                token.tokenType = TokenType::minus_minus;
                token.ptr = startPtr;
                token.len = 2;
                CurBufPtr+=2;
            } else if (CurBufPtr[1] == '=') {
                token.tokenType = TokenType::minus_equal;
                token.ptr = startPtr;
                token.len = 2;
                CurBufPtr+=2;
            } else {
                token.tokenType = TokenType::minus;
                token.ptr = startPtr;
                token.len = 1;
                CurBufPtr++;
            }
            break;
        case '*':
            if (CurBufPtr[1] == '=') {
                token.tokenType = TokenType::star_equal;
                token.ptr = startPtr;
                token.len = 2;
                CurBufPtr+=2;
            } else {
                 token.tokenType = TokenType::star;
                token.ptr = startPtr;
                token.len = 1;
                CurBufPtr++;
            }
            break;
        case '/':
            if (CurBufPtr[1] == '=') {
                token.tokenType = TokenType::slash_equal;
                token.ptr = startPtr;
                token.len = 2;
                CurBufPtr+=2;
            } else {
                token.tokenType = TokenType::slash;
                token.ptr = startPtr;
                token.len = 1;
                CurBufPtr++;
            }
            break;
        case '%':
            if (CurBufPtr[1] == '=') {
                token.tokenType = TokenType::percent_equal;
                token.ptr = startPtr;
                token.len = 2;
                CurBufPtr+=2;
            } else {
                 token.tokenType = TokenType::percent;
                token.ptr = startPtr;
                token.len = 1;
                CurBufPtr++;
            }
            break;
        case ';':
            token.tokenType = TokenType::semi;
            token.ptr = startPtr;
            token.len = 1;
            CurBufPtr++;
            break;
        case '(':
            token.tokenType = TokenType::l_parent;
            token.ptr = startPtr;
            token.len = 1;
            CurBufPtr++;
            break;
        case ')':
            token.tokenType = TokenType::r_parent;
            token.ptr = startPtr;
            token.len = 1;
            CurBufPtr++;
            break;
        case '=':
            if (*(CurBufPtr+1) == '=') {
                token.tokenType = TokenType::equal_equal;
                token.ptr = startPtr;
                token.len = 2;
                CurBufPtr+=2;
            } else {
                token.tokenType = TokenType::equal;
                token.ptr = startPtr;
                token.len = 1;
                CurBufPtr++;
            }
            break;
        case '|':
            if (*(CurBufPtr+1) == '|') {
                token.tokenType = TokenType::pipepipe;
                token.ptr = startPtr;
                token.len = 2;
                CurBufPtr+=2;
            } else if (*(CurBufPtr+1) == '=') {
                token.tokenType = TokenType::pipe_equal;
                token.ptr = startPtr;
                token.len = 2;
                CurBufPtr+=2;
            } else {
                token.tokenType = TokenType::pipe;
                token.ptr = startPtr;
                token.len = 1;
                CurBufPtr++;
            }
            break;
        case '&':
            if (*(CurBufPtr+1) == '&') {
                token.tokenType = TokenType::ampamp;
                token.ptr = startPtr;
                token.len = 2;
                CurBufPtr+=2;
            } else if (*(CurBufPtr+1) == '=') {
                token.tokenType = TokenType::amp_equal;
                token.ptr = startPtr;
                token.len = 2;
                CurBufPtr+=2;
            } else {
                token.tokenType = TokenType::amp;
                token.ptr = startPtr;
                token.len = 1;
                CurBufPtr++;
            }
            break;
        case '^':
            if (*(CurBufPtr+1) == '=') {
                token.tokenType = TokenType::caret_equal;
                token.ptr = startPtr;
                token.len = 2;
                CurBufPtr+=2;
            } else {
                token.tokenType = TokenType::caret;
                token.ptr = startPtr;
                token.len = 1;
                CurBufPtr++;
            }
            break;
        case '~':
            token.tokenType = TokenType::tilde;
            token.ptr = startPtr;
            token.len = 1;
            CurBufPtr++;
            break;
        case ',':
            token.tokenType = TokenType::comma;
            token.ptr = startPtr;
            token.len = 1;
            CurBufPtr++;
            break;
        case '{':
            token.tokenType = TokenType::l_brace;
            token.ptr = startPtr;
            token.len = 1;
            CurBufPtr++;
            break;
        case '}':
            token.tokenType = TokenType::r_brace;
            token.ptr = startPtr;
            token.len = 1;
            CurBufPtr++;
            break;
        case '<':
            if (*(CurBufPtr+1) == '=') {
                token.tokenType = TokenType::less_equal;
                token.ptr = startPtr;
                token.len = 2;
                CurBufPtr+=2;
            } else if (*(CurBufPtr+1) == '<') {
                if (CurBufPtr[2] == '=') {
                    token.tokenType = TokenType::less_less_equal;
                    token.ptr = startPtr;
                    token.len = 3;
                    CurBufPtr+=3;
                } else {
                    token.tokenType = TokenType::less_less;
                    token.ptr = startPtr;
                    token.len = 2;
                    CurBufPtr+=2;
                }
            } else {
                token.tokenType = TokenType::less;
                token.ptr = startPtr;
                token.len = 1;
                CurBufPtr++;
            }
            break;
        case '>':
            if (*(CurBufPtr+1) == '=') {
                token.tokenType = TokenType::greater_equal;
                token.ptr = startPtr;
                token.len = 2;
                CurBufPtr+=2;
            } else if (*(CurBufPtr+1) == '>') {
                if (CurBufPtr[2] == '=') {
                    token.tokenType = TokenType::greater_greater_equal;
                    token.ptr = startPtr;
                    token.len = 3;
                    CurBufPtr+=3;
                } else {
                    token.tokenType = TokenType::greater_greater;
                    token.ptr = startPtr;
                    token.len = 2;
                    CurBufPtr+=2;
                }
            } else {
                token.tokenType = TokenType::greater;
                token.ptr = startPtr;
                token.len = 1;
                CurBufPtr++;
            }
            break;
        case '!':
            if (*(CurBufPtr+1) == '=') {
                token.tokenType = TokenType::not_equal;
                token.ptr = startPtr;
                token.len = 2;
                CurBufPtr+=2;
            } else {
                token.tokenType = TokenType::exclaim;
                token.ptr = startPtr;
                token.len = 1;
                CurBufPtr++;
            }
            break;
        case '?':
            token.tokenType = TokenType::question;
            token.ptr = startPtr;
            token.len = 1;
            CurBufPtr++;
            break;
        case ':':
            token.tokenType = TokenType::colon;
            token.ptr = startPtr;
            token.len = 1;
            CurBufPtr++;
            break;
        case '[':
            token.tokenType = TokenType::l_bracket;
            token.ptr = startPtr;
            token.len = 1;
            CurBufPtr++;
            break;
        case ']':
            token.tokenType = TokenType::r_bracket;
            token.ptr = startPtr;
            token.len = 1;
            CurBufPtr++;
            break;
        default:
            // token.content = llvm::StringRef(startPtr, 1);
            // llvm::outs() << "unkown char " << *CurBufPtr << "\n";
            diagEngine.Report(llvm::SMLoc::getFromPointer(CurBufPtr), diag::err_unknown_char, *CurBufPtr);
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