#include "lexer.h"

/*
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
    kw_void,     // void
    kw_char,     // char
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
    case TokenType::kw_struct:
        return "struct";
    case TokenType::kw_union:
        return "union";
    case TokenType::kw_return:
        return "return";
    case TokenType::kw_void:
        return "void";
    case TokenType::kw_char:
        return "char";
    case TokenType::kw_const:
        return "const";
    case TokenType::kw_volatile:
        return "volatile";
    case TokenType::kw_static:
        return "static";
    case TokenType::kw_extern:
        return "extern";
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
    case TokenType::dot:
        return ".";
    case TokenType::arrow:
        return "->";
    case TokenType::ellipse:
        return "...";
    case TokenType::identifier:
        return "identifier";
    case TokenType::string:
        return "string";
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


// Read a single character in a char or string literal.
static const char *c_char(int *res, const char *p) {
    // Nonescaped
    if (*p != '\\') {
        *res = *p;
        return p + 1;
    }
    p++;

    int esc = 0;
    switch (*p) {
        case 'a': {
            esc = '\a';
            break;
        }
        case 'b': {
            esc = '\b';
            break;
        }
        case 'f': {
            esc = '\f';
            break;
        }
        case 'n': {
            esc = '\n';
            break;
        }
        case 'r': {
            esc = '\r';
            break;
        }
        case 't': {
            esc = '\t';
            break;
        }
        case 'v': {
            esc = '\v';
            break;
        }
        default : {
            /// '\'' '"'
            esc = *p;
            break;
        }
    }
    *res = esc;
    return p + 1;
}



bool Lexer::StartWith(const char* p) {
    return !strncmp(CurBufPtr, p, strlen(p));
}

void Lexer::NextToken(Token &token) {
    // 1. 过滤空格和换行, 以及注释
    while(IsWhiteSpace(*CurBufPtr) || StartWith("//") || StartWith("/*")) {
        // 过滤单行注释
        if (StartWith("//")) {
            while (*CurBufPtr != '\n' && (CurBufPtr < BufEnd)) {
                CurBufPtr++;
            }
        }

        // 过滤多行注释
        if (StartWith("/*")) {
            while ((CurBufPtr[0] != '*' || CurBufPtr[1] != '/') && (CurBufPtr < BufEnd) ) {
                if (*CurBufPtr == '\n' ) {
                    row++;
                    LineHeadPtr = CurBufPtr + 1;
                }
                CurBufPtr++;
            }
            CurBufPtr += 2;
        }
        if (CurBufPtr >= BufEnd) {
            token.tokenType = TokenType::eof;
            return;
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

    // 3. 获取token的起始位置指针(用于计算token长度)
    const char *startPtr = CurBufPtr;


    // 4. 处理字符值（注意是字符，不是字符串）
    // 'a'  or '\n'
    if (CurBufPtr[0] == '\'') {
        token.tokenType = TokenType::number;    /* 字符的类型也可以认为是一个数字*/
        CurBufPtr++;     // skip left '
        token.ptr = CurBufPtr;
        token.type = CType::IntType;            /* token 的语言类型 */
        CurBufPtr = c_char(&token.value, CurBufPtr);
        if (CurBufPtr[0] != '\'') {
            diagEngine.Report(llvm::SMLoc::getFromPointer(CurBufPtr), diag::err_unclosed_charactor);
        }

        token.len = CurBufPtr - startPtr;
        CurBufPtr++;     // skip right '
    } else if (CurBufPtr[0] == '\"') {
        CurBufPtr++;     // skip left "
        token.tokenType = TokenType::string;
        token.ptr = CurBufPtr;

        std::string value;
        while (CurBufPtr[0] != '\"') {
            if (CurBufPtr[0] == '\0') {
                // 相当于字符串提前结束了
                diagEngine.Report(llvm::SMLoc::getFromPointer(CurBufPtr), diag::err_unclosed_string);
            }
            int c;
            CurBufPtr = c_char(&c, CurBufPtr);
            value += c;
        }

        // 字符串的类型就是数组（字符数组）
        token.strValue = value;
        token.type = std::make_shared<CArrayType>(CType::CharType, value.length());
        token.len = CurBufPtr - startPtr;
        CurBufPtr++;     // skip right "
    } else if (IsDigit(*CurBufPtr)) {
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
        } else if (content == "struct") {
            token.tokenType = TokenType::kw_struct;
        } else if (content == "union") {
            token.tokenType = TokenType::kw_union;
        } else if (content == "return") {
            token.tokenType = TokenType::kw_return;
        } else if (content == "void") {
            token.tokenType = TokenType::kw_void;
        } else if (content == "char") {
            token.tokenType = TokenType::kw_char;
        } else if (content == "const") {
            token.tokenType = TokenType::kw_const;
        } else if (content == "static") {
            token.tokenType = TokenType::kw_static;
        } else if (content == "volatile") {
            token.tokenType = TokenType::kw_volatile;
        } else if (content == "extern") {
            token.tokenType = TokenType::kw_extern;
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
            } else if (CurBufPtr[1] == '>') {
                token.tokenType = TokenType::arrow;
                token.ptr = startPtr;
                token.len = 2;
                CurBufPtr +=2;
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
        case '.':
            if (CurBufPtr[1] == '.' && CurBufPtr[2] == '.') {
                token.tokenType = TokenType::ellipse;
                token.ptr = startPtr;
                token.len = 3;
                CurBufPtr += 3;
            } else {
                token.tokenType = TokenType::dot;
                token.ptr = startPtr;
                token.len = 1;
                CurBufPtr++;
            }
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