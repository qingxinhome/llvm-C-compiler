#include "parser.h"
#include "eval_constant.h"

/*
prog : stmt*
stmt : decl-stmt | expr-stmt | null-stmt | if-stmt | block-stmt
null-stmt : ";"
decl-stmt : "int" identifier ("," identifier (= expr)?)* ";"
if-stmt : "if" "(" expr ")" stmt ( "else" stmt )?
block-stmt: "{" stmt* "}"
expr-stmt : expr ";"
expr : assign-expr | add-expr
assign-expr: identifier "=" expr
add-expr : mult-expr (("+" | "-") mult-expr)* 
mult-expr : primary-expr (("*" | "/") primary-expr)* 
primary-expr : identifier | number | "(" expr ")" 
number: ([0-9])+ 
identifier : (a-zA-Z_)(a-zA-Z0-9_)*
*/
std::shared_ptr<Program> Parser::ParseProgram() {
    auto program = std::make_shared<Program>();
    program->fileName = lexer.GetFileName();

    while (token.tokenType != TokenType::eof) {
        std::shared_ptr<AstNode> node;
        if (IsFunctionDecl()) {
            /* 函数声明 */
            node = ParseFunctionDecl();
        } else {
            /* 全局变量的声明 */                         
            node = ParseDeclareStmt(true);
        }
        if (node != nullptr) {
            program->externalDecls.push_back(node);
        }   
    }

    Expect(TokenType::eof);
    return program;
}

std::shared_ptr<AstNode> Parser::ParseFunctionDecl() {
    bool isTypedef = false;
    std::shared_ptr<CType> baseType = ParseDeclSpec(isTypedef);   // 解析声明说明符

    // 从函数定义的参数列表开始就是函数的作用域
    sema.EnterScope();
    std::shared_ptr<AstNode> node = Declarator(baseType, true);   // 解析声明符

    // 是否是 typedef 的函数类型声明, 如：typedef float (*func_t)(double a, int b);
    if (isTypedef) {
        Consume(TokenType::semi);
        sema.ExitScope();
        sema.semaTypedefDecl(node->type, node->token);
        return nullptr;
    } else {
        std::shared_ptr<AstNode> blockStmt = nullptr;
        // 遇到 ；号，说明是C语言函数的声明, 否则是函数定义
        if (token.tokenType != TokenType::semi) {
            blockStmt = ParseBlockStmt();
        } else {
            Consume(TokenType::semi);
        }
        sema.ExitScope();
    
        return sema.semaFunctionDecl(node->token, node->type, blockStmt);
    }
}


std::shared_ptr<AstNode> Parser::ParseStmt() {
    // null statement
    if (token.tokenType == TokenType::semi) {
        NextToken();
        return nullptr;
    }
    // declare statment
    else if (IsTypeName(token)) {
        return ParseDeclareStmt();
    }
    // if statement
    else if (token.tokenType == TokenType::kw_if) {
        return ParseIfStmt();
    }
    // block statment
    else if (token.tokenType == TokenType::l_brace) {
        return ParseBlockStmt();
    }
    // for statement
    else if (token.tokenType == TokenType::kw_for) {
        return ParseForStmt();
    }
    // break statement
    else if (token.tokenType == TokenType::kw_break) {
        return ParseBreakStmt();
    }
    // continue statement
    else if (token.tokenType == TokenType::kw_continue) {
        return ParseContinueStmt();
    } 
    // return statement
    else if (token.tokenType == TokenType::kw_return) {
        return ParseReturnStmt();
    }
    // while statment
    else if (token.tokenType == TokenType::kw_while) {
        return ParseWhileStmt();
    }
    // do while statment
    else if (token.tokenType == TokenType::kw_do) {
        return ParseDoWhileStmt();
    }
    // switch statment
    else if (token.tokenType == TokenType::kw_switch) {
        return ParseSwitchStmt();
    }
    // case statemnt
    else if (token.tokenType == TokenType::kw_case) {
        return ParseCaseStmt();
    }
    // default statement
    else if (token.tokenType == TokenType::kw_default) {
        return ParseDefaultStmt();
    }
    // expr statement
    else {
        return ParseExprStmt();
    }   
}

// 声明说明符（Declaration Specifiers）解析, 处理声明语句中的类型说明符，如：int *p; 中的int
std::shared_ptr<CType> Parser::ParseDeclSpec(bool &isTypedef) {
    if (!IsTypeName(token)) {
        GetDiagEngine().Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_type);
    }

    /*
    定义四个状态机,分别记录：
      kind基本类型：void, char, int, float, double
      size长度修饰：short, long, long long
      sig符号性：signed, unsigned
      sclass存储类：typedef, extern, static
    互斥性：同类只能出现一次 → 否则 goto err
    */
    enum {kkindUnused = 0, kvoid = 1, kchar, kint, kfloat, kdouble} kind = {kkindUnused};
    enum {ksizeUnused = 0, kshort = 1, klong, kllong} size = {ksizeUnused};
    enum {ksignedUnused = 0, ksigned = 1, kunsigned} sig = {ksignedUnused};
    enum {ksclassUnused = 0, kTypedef = 1, kExtern, kStatic, kAuto, kRegister} sclass = {ksclassUnused};

    // usertype表示C语言中的用户自定义类型（不是C的内置基本类型）
    std::shared_ptr<CType> userType = nullptr;
    std::shared_ptr<CType> ty;
    for (;;) {
        if (token.tokenType == TokenType::eof) {
            assert(0 && "end of input");
        }
        // 判断是否是typedef的类型(typedef 类型名优先查找)
        if (kind == kkindUnused && !userType && token.tokenType == TokenType::identifier) {
            // 查符号表，看token是否是 typedef 定义的类型名
            std::shared_ptr<CType> tydef = sema.semaTypedefAccess(token);
            if (tydef) {
                userType = tydef;
                NextToken();
                goto errcheck;
            }
        }
        // 根据 token 类型更新：kind / size / sig / sclass / usertype
        switch (token.tokenType) {
            case TokenType::kw_typedef: {
                if (sclass) 
                    goto err; // 已经有存储类，错 (存储类只能出现一个)

                sclass = kTypedef;
                isTypedef = true;
                NextToken();
                break;
            }
            case TokenType::kw_extern:
                if (sclass) 
                    goto err;
                sclass = kExtern;
                NextToken();
                break;
            case TokenType::kw_static:
                if (sclass) 
                    goto err;
                sclass = kStatic;
                NextToken();
                break;
            case TokenType::kw_auto:
                if (sclass) 
                    goto err;
                sclass = kAuto;
                NextToken();
                break;
            case TokenType::kw_register:
                if (sclass) 
                    goto err;
                sclass = kRegister;
                NextToken();
                break;
            /* 类型限定符（const、volatile）不记录，留给 declarator 部分处理 */
            case TokenType::kw_const: NextToken(); break;
            case TokenType::kw_volatile: NextToken(); break;
            case TokenType::kw_inline: NextToken(); break;
            /* 基本类型只能出现一个 */
            case TokenType::kw_void:
                if (kind) 
                    goto err;
                kind = kvoid;
                NextToken();
                break;
            case TokenType::kw_char:
                if (kind) 
                    goto err;
                kind = kchar;
                NextToken();
                break;
            case TokenType::kw_int:
                if (kind) 
                    goto err;
                kind = kint;
                NextToken();
                break;
            case TokenType::kw_float:
                if (kind) 
                    goto err;
                kind = kfloat;
                NextToken();
                break;
            case TokenType::kw_double:
                if (kind) 
                    goto err;
                kind = kdouble;
                NextToken();
                break;
            // 符号类型只能出现一个
            case TokenType::kw_signed:
                if (sig) 
                    goto err;
                sig = ksigned;
                NextToken();
                break;
            case TokenType::kw_unsigned:
                if (sig) 
                    goto err;
                sig = kunsigned;
                NextToken();
                break;
            /* 长度修饰 */
            case TokenType::kw_short:
                if (size)
                    goto err;
                size = kshort;
                NextToken();
                break;
            case TokenType::kw_long:
                if (size == ksizeUnused) {
                    size = klong;
                    NextToken();
                }
                else if (size == klong) {
                    // long → long long  支持 long 出现 2 次
                    size = kllong;
                    NextToken();
                }
                else {
                    // 已经有 short 或 long 或long long, 其他非法
                    goto err;
                }
                break;
            case TokenType::kw_struct:
            case TokenType::kw_union:
                if (userType) 
                    goto err;   // 已经有用户自定义类型了
                userType = ParseStructOrUnionSpec();
                break;
            default:
                goto done;   // 跳出循环
        }

        errcheck:
            if (size == kshort && (kind != 0 && kind != kint))   // short double 非法
                goto err;
            if (size == klong && (kind != 0 && kind != kint && kind != kdouble))  // long float 非法
                goto err;
            if (sig != 0 && (kind == kvoid || kind == kfloat || kind == kdouble))  // unsigned void 非法
                goto err;
            if (userType && (kind != 0 || size != 0 || sig != 0))   // typedef 不能和其他类型混用
                goto err;
    }
done:
    if (userType) {
        return userType;
    }

    switch (kind) {
        case kvoid: ty = CType::VoidType; goto end;
        case kchar: ty = CType::CharType; goto end;
        case kfloat: ty = CType::FloatType; goto end;
        case kdouble: ty = (size ==klong) ? CType::LDoubleType : CType::DoubleType; goto end;
        default: break;
    }

    switch (size) {
        case kshort:
            ty = sig == kunsigned ? CType::UShortType : CType::ShortType;
            goto end;
        case klong:
            ty = sig == kunsigned ? CType::ULongType : CType::LongType;
            goto end;
        case kllong:
            ty = sig == kunsigned ? CType::ULongLongType : CType::LongLongType;
            goto end;
        default:
            ty = sig == kunsigned ? CType::UIntType : CType::IntType;
            goto end;
    }
end:
    return ty;
err:
    GetDiagEngine().Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_type);
    return ty;
}


/*
decl-spec  ::= "int" | struct-or-union-specifier
struct-or-union-specifier ::= struct-or-union identifier "{" (decl-spec declarator(, declarator)* ";")+ "}"
														  struct-or-union identifier
struct-or-union := "struct" | "union"
*/
std::shared_ptr<CType> Parser::ParseStructOrUnionSpec() {
    TagKind tagkind;
    if (token.tokenType == TokenType::kw_struct) {
        tagkind = TagKind::KStruct;
    } else if (token.tokenType == TokenType::kw_union) {
        tagkind = TagKind::KUnion;
    } else {
        assert(0);
    }
    NextToken();

    bool anony = false;
    // 判断是否为匿名结构体
    if (token.tokenType != TokenType::identifier) {
        anony = true;
    }

    // 获取结构体名
    Token tag = token;
    if (token.tokenType == TokenType::identifier) {
        /* 
         记录结构体类型名(即tag)，如:
         struct A {...} 中的A,
         struct A *ptr; 中的 A；
        */
        tag = token;
        Consume(TokenType::identifier);
    }

    std::shared_ptr<CType> recordType = nullptr;
    if (!anony) {
        // 从类型符号表中查找 结构体名是否存在，如果存在就返回对应结构体类型
        recordType = sema.semaTagAccess(tag);
    }

    // 如果结构体类型名没有未定义
    if (recordType == nullptr) {
            llvm::StringRef text;
        if (anony) {
            // 如果是匿名结构体， 则为其生成一个名字
            text = CType::GenAnonyRecordName(tagkind);
        } else {
            text = llvm::StringRef(tag.ptr, tag.len);
        }
        // 由于此时还没有解析到结构体的成员，因此构造CRecordType对象时，members成员属性设为空的vector, 
        recordType = std::make_shared<CRecordType>(text, std::vector<Member>(), tagkind);
    }

    /*
        struct A {int a,b; int *p}  // 定义结构体
        struct A           // 使用一个结构体， 如 struct A[20];
    */
    if (token.tokenType == TokenType::l_brace) { 
        Consume(TokenType::l_brace);
        // 结构体定义{...}也是一个新的作用域
        sema.EnterScope();

        std::vector<Member> members;
        while(token.tokenType != TokenType::r_brace) {
            auto node = ParseDeclareStmt();
            DeclareStmt *declstmt = llvm::dyn_cast<DeclareStmt>(node.get());
            for (auto &n : declstmt->nodeVec) {
                Member mbr;
                mbr.ty = n->type;
                mbr.name = llvm::StringRef(n->token.ptr, n->token.len);
                members.push_back(mbr);
            }
        }
        sema.ExitScope();
        Consume(TokenType::r_brace);

        // 补充设置CRecordType类型的成员属性
        CRecordType *ty = llvm::dyn_cast<CRecordType>(recordType.get());
        ty->SetMembers(members);

        return sema.semaTagDeclare(tag, recordType);
    } else {
        return recordType;
    }

    return nullptr;
}

/*
解析类型声明符后缀；如:
int a[2][3][4] 数组类型中的[2][3][4]
int a(int，float) 函数类型中的(int，float) 
*/
std::shared_ptr<CType> Parser::DirectDeclaratorSuffix(Token ident, std::shared_ptr<CType> baseType, bool isGloabl) {
    if (token.tokenType == TokenType::l_bracket) {
        // 数组声明
        return DirectDeclaratorArraySuffix(baseType, isGloabl);
    } else if (token.tokenType == TokenType::l_parent) {
        // 函数声明
        return DirectDeclaratorFuncSuffix(ident, baseType, isGloabl);
    }
    return baseType;
}

/* 
解析数组类型声明时的数组后缀， 如：int a[2][3][4] 中的[2][3][4]
direct-declarator ::= identifier | direct-declarator "[" assign "]" 
					| direct-declarator "(" parameter-type-list? ")"
eg: int a[3][4];
    baseType -> int
*/
std::shared_ptr<CType> Parser::DirectDeclaratorArraySuffix(std::shared_ptr<CType> baseType, bool isGloabl) {
    if (token.tokenType != TokenType::l_bracket) { /* 递归退出条件 */
        return baseType;
    }

    // 数组在声明的时候也可以不指定长度，如数组做函数参数：
    // int combine(int arr1[], int arr1_length, int arr2[], int arr2_length)
    int elemCount = -1;
    Consume(TokenType::l_bracket);
    if (token.tokenType != TokenType::r_bracket) {
        EvalConstant eval(GetDiagEngine());
        auto expr = ParseExpr();

        EvalConstant::Constant constant = eval.Eval(expr.get());
        if (!std::holds_alternative<int64_t>(constant)) {
            GetDiagEngine().Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_expected_ex, "interger type");
        }
        elemCount = std::get<int64_t>(constant);
        if (elemCount < 0) {
            GetDiagEngine().Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_arr_size);
        }
    }
    Consume(TokenType::r_bracket);
    return std::make_shared<CArrayType>(DirectDeclaratorArraySuffix(baseType, isGloabl), elemCount);
}

/*
解析函数类型声明的后缀也就是函数参数： 如:
int fun(int a); 中的 fun(int a)
文法：
direct-declarator ::= identifier | direct-declarator "[" assign "]" 
					| direct-declarator "(" parameter-type-list? ")"
*/
std::shared_ptr<CType> Parser::DirectDeclaratorFuncSuffix(Token ident, std::shared_ptr<CType> baseType, bool isGloabl) {
    Consume(TokenType::l_parent);

    std::vector<Parameter> params;
    bool isVarArg = false;
    int i = 0;
    while(token.tokenType != TokenType::r_parent) {
        if (i > 0 && token.tokenType == TokenType::comma) {
            Consume(TokenType::comma);
        }

        // void 参数，空参数列表,如： void say_hello(void)
        if (token.tokenType == TokenType::kw_void) {
            Consume(TokenType::kw_void);
            break;
        }

        // 可变参数 ... 只能是函数最后一个参数
        if (i > 0 && token.tokenType == TokenType::ellipse) {
            isVarArg = true;
            Consume(TokenType::ellipse);
            break;
        }

        // 解析函数形参
        bool isTypedef = false;
        auto ty = ParseDeclSpec(isTypedef);
        auto node = Declarator(ty, isGloabl);

        Parameter parameter;
        if (node->type->GetKind() == CType::TY_Array) {
            // 在C语言语法中， 如果函数参数为数组类型，那么会将其转换为指向数组的指针
            parameter.type = std::make_shared<CPointType>(node->type);
        } else {
            parameter.type = node->type;
        }
        parameter.name = llvm::StringRef(node->token.ptr, node->token.len);
        
        params.push_back(parameter);
        i++;
    }

    Consume(TokenType::r_parent);
    return std::make_shared<CFuncType>(baseType, params, llvm::StringRef(ident.ptr, ident.len), isVarArg);
}


// declarator ::= "*"* direct-declarator
// direct-declarator ::= identifier | "(" declarator ")" | direct-declarator "[" assign "]"
std::shared_ptr<AstNode> Parser::DirectDeclarator(std::shared_ptr<CType> baseType, bool isGloabl) {
    std::shared_ptr<AstNode> varDeclNode;
    if (this->token.tokenType == TokenType::l_parent) {
        // 处理数组指针的语法： 如：int (*p)[5] p是一个指向int [5]数组的指针
        Token curToken = token;

        //----------------------------------------------------------------------------
        lexer.SaveState();              /* 保存当前状态 */
        sema.SetMode(Sema::Mode::Skip);

        Consume(TokenType::l_parent);
        Declarator(CType::IntType, isGloabl);     /* CType::IntType只是临时占位类型，没有实际意义 */
        Consume(TokenType::r_parent);

        // 解析类型声明符后缀， 获取完整类型
        baseType = DirectDeclaratorSuffix(token, baseType, isGloabl);
        lexer.RestoreState();           /* 恢复当前状态 */
        sema.UnSetMode();               /* 取消mode设置 */
        //----------------------------------------------------------------------------

        token = curToken;
        Consume(TokenType::l_parent);
        varDeclNode = Declarator(baseType, isGloabl);
        Consume(TokenType::r_parent);

        /* CType::IntType是临时占位类型，没有实际意义，调用DirectDeclaratorSuffix是为了让token走到 = 符号 */
        DirectDeclaratorSuffix(token, CType::IntType, isGloabl);
    } else if (token.tokenType == TokenType::identifier) {
        Token ident = token;                 /* 记录变量名 */
        Consume(TokenType::identifier);

        // 解析类型声明符后缀, 如： a[2][3][4][5]， 或 a(int, char)
        baseType = DirectDeclaratorSuffix(ident, baseType, isGloabl);
        varDeclNode = sema.semaVariableDeclNode(ident, baseType, isGloabl);
    } else {
        GetDiagEngine().Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_expected_ex, "identifer or '('");
    }
    
    if (token.tokenType == TokenType::equal) {
        NextToken();
        // variableDecl->init = ParseAssignExpr();
        std::vector<int> offsetList{0};  // 0 表示访问首元素
        VariableDecl *variableDecl = llvm::dyn_cast<VariableDecl>(varDeclNode.get());
        ParseInitializer(variableDecl->initValues, variableDecl->type, offsetList, token.tokenType == TokenType::l_brace);
    }
    return varDeclNode;
}


void Parser::ParseStringInitializer(std::vector<std::shared_ptr<VariableDecl::InitValue>> &arr, 
    std::shared_ptr<CType> declType, 
    std::vector<int> &offsetList
) {
    CArrayType *arrayTy = llvm::dyn_cast<CArrayType>(declType.get());

    Token strToken = token;
    std::string strValue = strToken.strValue;
    Consume(TokenType::str);

    int arrLen = arrayTy->GetElementCount();
    if (arrLen < 0) {
        arrLen = token.strValue.size() + 1;
        arrayTy->SetElementCount(arrLen);
    }

    int strLen = strToken.strValue.size();
    if (arrLen < strLen) {
        GetDiagEngine().Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_large_len);
    }
    
    // 字符串初始值的长度小于声明长度， 如： 
    // char a[10] = "12345";  or  char a[10] = "0123456789";
    if (arrLen > strLen) {
        for (int i = 0; i < strLen; i++) {
            // 把字符串的每一个字符当成一个number表达式
            auto numNode = sema.semaNumberExprNode(strToken, strToken.strValue[i] ,CType::IntType);
            offsetList.push_back(i);
            auto initValue = sema.semaDeclInitValue(arrayTy->GetElementType(), numNode, offsetList, strToken);
            offsetList.pop_back();
            arr.push_back(initValue);
        }
        // 对于字符数组剩余未初始化的字符值，置为0
        for (int i = strLen; i < arrLen; i++) {
            auto numNode = sema.semaNumberExprNode(strToken, 0, CType::IntType);
            offsetList.push_back(i);
            auto initValue = sema.semaDeclInitValue(arrayTy->GetElementType(), numNode, offsetList, strToken);
            offsetList.pop_back();
            arr.push_back(initValue);
        }
    } 
    // 字符串初始值的长度等于声明长度
    else {
        for (int i = 0; i < strLen; i++) {
            // 把字符串的每一个字符当成一个number表达式
            auto numNode = sema.semaNumberExprNode(strToken, strToken.strValue[i] ,CType::IntType);
            offsetList.push_back(i);
            auto initValue = sema.semaDeclInitValue(arrayTy->GetElementType(), numNode, offsetList, strToken);
            offsetList.pop_back();
            arr.push_back(initValue);
        }
    }
}


/*
initializer ::= assign | "{" initializer ("," initializer)*  "}"
解析表达式的初始值
函数参数：
  arr(引用参数)
  declType  代表在解析`{初始化列表}` 时， 当前这个初始化列表对应的数组类型 
  offsetList (引用参数): 元素索引列表
  hasLBrace: 是否有左大括号
返回值bool： 标识初始化列表是否结束
*/
bool Parser::ParseInitializer(std::vector<std::shared_ptr<VariableDecl::InitValue>> &arr, 
    std::shared_ptr<CType> declType, 
    std::vector<int> &offsetList,
    bool hasLBrace
) {
    if (token.tokenType == TokenType::r_brace) {
        if (!hasLBrace) {
            GetDiagEngine().Report(llvm::SMLoc::getFromPointer(token.ptr),  diag::err_miss, "{");
        }
        return true;
    }
    
    // 处理：char a[] = "abcdef"; 这种情况
    if (token.tokenType == TokenType::str && IsStringArrayType(declType)) {
        ParseStringInitializer(arr, declType, offsetList);
        // 字符串字面量可能只是初始化列表的一部分，后面可能还有, 如： char matrix[2][10] = { "First", "Second" };
        return false;
    }

    // {,,}
    if (token.tokenType == TokenType::l_brace) {
        Consume(TokenType::l_brace);

        //  处理：char a[] = {"abcdef"}; 这种情况
        if (token.tokenType == TokenType::str && IsStringArrayType(declType)) {
            ParseStringInitializer(arr, declType, offsetList);
        } else if (declType->GetKind() == CType::TY_Array) {
            CArrayType *arrType = llvm::dyn_cast<CArrayType>(declType.get());
            int size = arrType->GetElementCount();

            /*
              数组的声明加初始化形式：
                int arr[10] = {1,2,3};
                int arr[] = {1,2,3};
              注：数组也可以在声明时不指定元素的数量， 有初始值的数量决定元素的数量，如：
            */
            int isEnd = false;
            bool isFlex = size < 0? true : false;    /* 如果size < 0 说明数组在声明时没有指定元素个数 */
            int i = 0;
            for (; i < size || isFlex; i++) {
                if (i > 0 && token.tokenType == TokenType::comma) {
                    Consume(TokenType::comma);
                } 

                offsetList.push_back(i);
                isEnd = ParseInitializer(arr, arrType->GetElementType(), offsetList, true);
                offsetList.pop_back();
                if (isEnd) {
                    break;
                }
            }
            // 为数组类型设置元素个数（只是针对声明时没有指定元素个数的情况）
            if (isFlex) {
                arrType->SetElementCount(i);
            }
        } else if (declType->GetKind() == CType::TY_Record) {
            // 处理结构体/共用体类型变量的初始化
            CRecordType *recordType = llvm::dyn_cast<CRecordType>(declType.get());

            TagKind tagKind = recordType->GetTagKind();
            const auto &members = recordType->GetMembers();

            if (tagKind == TagKind::KStruct) {
                int isEnd = false;
                // struct A {int a; int b;} d = {10, 12};
                for (int i = 0; i < members.size(); i++)
                {
                    if (i > 0 && token.tokenType == TokenType::comma) {
                        Consume( TokenType::comma);
                    }
                    // 这里使用offsetList做回溯
                    offsetList.push_back(i);
                    isEnd = ParseInitializer(arr, members[i].ty, offsetList, true);
                    offsetList.pop_back();
                    if (isEnd) {
                        break;
                    }
                }
            } else {
                // union 赋初值
                if (members.size() > 0) {
                    // union 赋初值默认只会给第一个成员赋值
                    offsetList.push_back(0);
                    ParseInitializer(arr, members[0].ty, offsetList, true);
                    offsetList.pop_back();
                }
            }
        }
        if (hasLBrace) {
            Consume(TokenType::r_brace);
        }
    } else {
        Token tmpToken = token;
        // assign expression
        auto node = ParseAssignExpr();

        auto initValue = sema.semaDeclInitValue(declType, node, offsetList, token);
        arr.push_back(initValue);
    }
    return false;
}



/*
declarator ::= "*"* direct-declarator
direct-declarator ::= identifier | direct-declarator "[" assign "]" 
					| direct-declarator "(" parameter-type-list? ")"
*/
std::shared_ptr<AstNode> Parser::Declarator(std::shared_ptr<CType> baseType, bool isGloabl) {
    // 处理指针符号 *
    while(token.tokenType == TokenType::star) {
        Consume(TokenType::star);
        // PointType
        baseType = std::make_shared<CPointType>(baseType);
    }
    return DirectDeclarator(baseType, isGloabl);
}



// 解析变量声明语句
std::shared_ptr<AstNode> Parser::ParseDeclareStmt(bool isGlobal) {
    // 获取基础类型
    bool isTypedef = false;
    std::shared_ptr<CType> baseTy = ParseDeclSpec(isTypedef);

    // 处理无意义的声明， 形如： int ; 
    if (token.tokenType == TokenType::semi) {
        Consume(TokenType::semi);
        return nullptr;
    }

    if (isTypedef) {
        int i = 0;
        while (token.tokenType != TokenType::semi) {
            if (i > 0) {
                Consume(TokenType::comma);
            }
            sema.SetMode(Sema::Mode::Skip);
            auto node = Declarator(baseTy, isGlobal);

            sema.UnSetMode();
            sema.semaTypedefDecl(node->type, node->token);
            i++;
        }

        Consume(TokenType::semi);
        return nullptr;
    } else {
        auto declareStmt = std::make_shared<DeclareStmt>();
        // int a , b = 3;
        // int a = 3;
        // int aa = 1, b = 1;
        int i = 0;
        while(token.tokenType != TokenType::semi) {
            // 如果是第二次进入循环体，判断是否为逗号，
            if (i++ > 0) {
                Consume(TokenType::comma);
            }
            auto variableDecl = Declarator(baseTy, isGlobal);
            declareStmt->nodeVec.push_back(variableDecl);
        }
        Consume(TokenType::semi);
        return declareStmt;
    }
}


std::shared_ptr<AstNode> Parser::ParseExprStmt() {
    std::shared_ptr<AstNode> expr = ParseExpr();
    Consume(TokenType::semi);
    return expr;
}

// if-stmt : "if" "(" expr ")" stmt ( "else" stmt )?
std::shared_ptr<AstNode> Parser::ParseIfStmt() {
    Consume(TokenType::kw_if);
    Consume(TokenType::l_parent);
    auto condExpr = ParseExpr();
    Consume(TokenType::r_parent);
    auto thenStmt = ParseStmt();

    std::shared_ptr<AstNode> elseStmt = nullptr;
    // peek token is 'else'
    if (token.tokenType == TokenType::kw_else) {
        Consume(TokenType::kw_else);
        elseStmt = ParseStmt();
    }

    return sema.semaIfStmtNode(condExpr, thenStmt, elseStmt);
}

std::shared_ptr<AstNode> Parser::ParseBlockStmt() {
    sema.EnterScope();
    Consume(TokenType::l_brace);

    auto blockStmt = std::make_shared<BlockStmt>();
    while(token.tokenType != TokenType::r_brace) {
        auto stmt = ParseStmt();
        if (stmt != nullptr) {
            blockStmt->nodeVec.push_back(stmt);
        } 
    }
    
    Consume(TokenType::r_brace);
    sema.ExitScope();
    return blockStmt;
}


// for-stmt : "for" "(" expr? ";" expr? ";" expr? ")" stmt
// 	          "for" "(" decl-stmt expr? ";" expr? ")" stmt
std::shared_ptr<AstNode> Parser::ParseForStmt() {
    Consume(TokenType::kw_for);
    Consume(TokenType::l_parent);

    sema.EnterScope();

    std::shared_ptr<ForStmt> forStmt = std::make_shared<ForStmt>();
    // -> for循环语句可以被break和continue, 记录父节点信息，为语义检查提供支持
    breakNodes.push_back(forStmt);
    continueNodes.push_back(forStmt);

    std::shared_ptr<AstNode> initNode;
    std::shared_ptr<AstNode> condNode;
    std::shared_ptr<AstNode> incNode;
    std::shared_ptr<AstNode> bodyNode;

    if (IsTypeName(token)) {
        initNode = ParseDeclareStmt();
    } else {
        if (token.tokenType != TokenType::semi) {
            initNode =  ParseExpr();
        }
        Consume(TokenType::semi);
    }

    if (token.tokenType != TokenType::semi) {
        condNode = ParseExpr();
    }
    Consume(TokenType::semi);

    if (token.tokenType != TokenType::r_parent) {
        incNode = ParseExpr();
    }
    Consume(TokenType::r_parent);

    bodyNode = ParseStmt();

    forStmt->initNode = initNode;
    forStmt->condNode = condNode;
    forStmt->incNode = incNode;
    forStmt->bodyNode = bodyNode;


    breakNodes.pop_back();
    continueNodes.pop_back();
    sema.ExitScope();

    return forStmt;
}


std::shared_ptr<AstNode> Parser::ParseWhileStmt() {
    // while循环语句和for循环语句本质上结构是一样的，可以共用一个语法树节点
    std::shared_ptr<ForStmt> node = std::make_shared<ForStmt>();
    // while循环没有初始化表达式，和循环自增表达式
    node->initNode = nullptr;
    node->incNode = nullptr;

    Consume(TokenType::kw_while);
    // -> while循环语句可以被break和continue, 记录父节点信息，为语义检查提供支持
    breakNodes.push_back(node);
    continueNodes.push_back(node);

    // 解析循环条件表达式
    Consume(TokenType::l_parent);
    node->condNode = ParseExpr();
    Consume(TokenType::r_parent);

    // 解析函数体
    node->bodyNode = ParseStmt();

    // <-while循环函数体解析结束后，回溯一下
    breakNodes.pop_back();
    continueNodes.pop_back();

    return node;
}

std::shared_ptr<AstNode> Parser::ParseDoWhileStmt() {
    std::shared_ptr<DoWhileStmt> node = std::make_shared<DoWhileStmt>();

    // -> do while循环语句可以被break和continue, 记录父节点信息，为语义检查提供支持
    breakNodes.push_back(node);
    continueNodes.push_back(node);

    Consume(TokenType::kw_do);
    // 解析函数体
    node->body = ParseStmt();

    Consume(TokenType::kw_while);
    Consume(TokenType::l_parent);

    // 解析循环条件表达式
    node->cond = ParseExpr();
    Consume(TokenType::r_parent);
    Consume(TokenType::semi);

    // <-do while循环函数体解析结束后，回溯一下
    breakNodes.pop_back();
    continueNodes.pop_back();

    return node;
}

std::shared_ptr<AstNode> Parser::ParseSwitchStmt() {
    std::shared_ptr<SwitchStmt> node = std::make_shared<SwitchStmt>();

    // -> switch语句可以被break, 
    breakNodes.push_back(node);
    // 只有switch语句中可以使用case语句，所以要记录父节点信息，为语义检查提供支持
    switchNodes.push_back(node);

    Consume(TokenType::kw_switch);
    Consume(TokenType::l_parent);
    Token tmptoken = token;
    node->expr = ParseExpr();
    if (!node->expr->type->IsIntegerType()) {
        GetDiagEngine().Report(llvm::SMLoc::getFromPointer(tmptoken.ptr), diag::err_expected_ex, "integer type");
    }
    Consume(TokenType::r_parent);

    node->stmt = ParseStmt();

    // switch语句解析结束后，回溯一下
    breakNodes.pop_back();
    switchNodes.pop_back();

    return node;
}

std::shared_ptr<AstNode> Parser::ParseCaseStmt() {
    if (switchNodes.size() == 0) {
        GetDiagEngine().Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_case_stmt);
    }

    std::shared_ptr<CaseStmt> node = std::make_shared<CaseStmt>();

    Consume(TokenType::kw_case);
    Token tmptoken = token;
    node->expr = ParseExpr();
    EvalConstant eval = EvalConstant(GetDiagEngine());
    EvalConstant::Constant c = eval.Eval(node->expr.get());
    if (!std::holds_alternative<int64_t>(c)) {
        GetDiagEngine().Report(llvm::SMLoc::getFromPointer(tmptoken.ptr), diag::err_int_constant_expr);
    }
    Consume(TokenType::colon);

    // 由于case语句的语句体，实际上就是一个block，只是它可以使用{} 进行包裹， 也可以没有{}
    // 因此这里需要做一下兼容性处理, 将case 关键字后的语句当做一个整体
    std::shared_ptr<BlockStmt> blockstmt = std::make_shared<BlockStmt>();
    while(token.tokenType != TokenType::kw_case &&
            token.tokenType != TokenType::kw_default &&
            token.tokenType != TokenType::r_brace) {
        auto stmt = ParseStmt();
        if (stmt != nullptr) {
            blockstmt->nodeVec.push_back(stmt);
        }
    }
    node->stmt = blockstmt;
    return node;
}

std::shared_ptr<AstNode> Parser::ParseDefaultStmt() {
    if (switchNodes.size() == 0) {
        GetDiagEngine().Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_default_stmt);
    }

    SwitchStmt *ststmt = llvm::dyn_cast<SwitchStmt>(switchNodes.back().get());
    if (ststmt->defaultStmt != nullptr) {
        GetDiagEngine().Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_multi_default_stmt);
    }

    std::shared_ptr<DefaultStmt> node = std::make_shared<DefaultStmt>();
    Consume(TokenType::kw_default);
    Consume(TokenType::colon);
    
    // 由于case语句的语句体，实际上就是一个block，只是它可以使用{} 进行包裹， 也可以没有{}
    // 因此这里需要做一下兼容性处理, 将case 关键字后的语句当做一个整体
    std::shared_ptr<BlockStmt> blockstmt = std::make_shared<BlockStmt>();
    while(token.tokenType != TokenType::kw_case &&
            token.tokenType != TokenType::kw_default &&
            token.tokenType != TokenType::r_brace) {
        auto stmt = ParseStmt();
        if (stmt != nullptr) {
            blockstmt->nodeVec.push_back(stmt);
        }
    }
    node->stmt = blockstmt;

    // 给上层的switch语句记录已经有一个default语句了
    ststmt->defaultStmt = node;
    return node;
}



std::shared_ptr<AstNode> Parser::ParseBreakStmt() {
    if (breakNodes.size() == 0) {
        GetDiagEngine().Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_break_stmt);
    }
    Consume(TokenType::kw_break);
    auto breakStmt = std::make_shared<BreakStmt>();
    breakStmt->targetNode = breakNodes.back();
    
    Consume(TokenType::semi);
    return breakStmt;
}

std::shared_ptr<AstNode> Parser::ParseContinueStmt() {
    if (continueNodes.size() == 0) {
        GetDiagEngine().Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_continue_stmt);
    }
    Consume(TokenType::kw_continue);
    auto continueStmt = std::make_shared<ContinueStmt>();
    continueStmt->targetNode = continueNodes.back();

    Consume(TokenType::semi);
    return continueStmt;
}


std::shared_ptr<AstNode> Parser::ParseReturnStmt() {
    Consume(TokenType::kw_return);
    auto returnStmt = std::make_shared<ReturnStmt>();
    if (token.tokenType != TokenType::semi) {
        returnStmt->expr = ParseExpr();
    }
    Consume(TokenType::semi);
    return returnStmt;
}


// expr  ::= assign (, assign )*
// eg： int result = (a = 1, b = 2, c = 3);  // 逗号表达式
std::shared_ptr<AstNode> Parser::ParseExpr() {
    auto left = ParseAssignExpr();
    while(token.tokenType == TokenType::comma) {
        Token tmpToken = token;
        Consume(TokenType::comma);
        left = sema.semaBinaryExprNode(left, ParseAssignExpr(), BinaryOp::comma, tmpToken);
    }
    return left;

    // bool isAssignExpr = false;
    // lexer.SaveState();
    // if (token.tokenType == TokenType::identifier) {
    //     Token tmp;
    //     lexer.NextToken(tmp);
    //     if (tmp.tokenType == TokenType::equal) {
    //         isAssignExpr = true;
    //     }
    // }
    // lexer.RestoreState();

    // if (isAssignExpr) {
    //     // assign-expr: identifier "=" expr
    //     return ParseAssignExpr();
    // }
    // return ParseLogOrExpr();
}




// a = 5;
// a = b = 3;
std::shared_ptr<AstNode> Parser::ParseAssignExpr() {
    auto left = ParseConditionalExpr();
    if (!IsAssignOperator()) {
        return left;
    }
    Token tmpToken = token;
    BinaryOp op;
    switch (token.tokenType)
    {
    case TokenType::equal:
        op = BinaryOp::assign;
        break;
    case TokenType::plus_equal:
        op = BinaryOp::add_assign;
        break;
    case TokenType::minus_equal:
        op = BinaryOp::sub_assign;
        break;
    case TokenType::star_equal:
        op = BinaryOp::mul_assign;
        break;
    case TokenType::slash_equal:
        op = BinaryOp::div_assign;
        break;
    case TokenType::percent_equal:
        op = BinaryOp::mod_assign;
        break;
    case TokenType::pipe_equal:
        op = BinaryOp::bitwise_or_assign;
        break;
    case TokenType::amp_equal:
        op = BinaryOp::bitwise_and_assign;
        break;
    case TokenType::caret_equal:
        op = BinaryOp::bitwise_xor_assign;
        break;
    case TokenType::less_less_equal:
        op = BinaryOp::left_shift_assign;
        break;
    case TokenType::greater_greater_equal:
        op = BinaryOp::right_shift_assign;
        break;
    default:
        break;
    }
    NextToken();
    return sema.semaBinaryExprNode(left, ParseAssignExpr(), op, tmpToken);

    // Expect(TokenType::identifier);
    // std::shared_ptr<VariableAccessExpr> left = sema.semaVariableAccessNode(token);
    // NextToken();

    // auto opToken = token;
    // Consume(TokenType::equal);
    // auto right = ParseExpr();
    // return sema.semaAssignExprNode(left, right, opToken);
}

// conditional ::= logor  ("?" expr ":" conditional)?
std::shared_ptr<AstNode> Parser::ParseConditionalExpr() {
    auto left = ParseLogOrExpr();
    if (token.tokenType != TokenType::question) {
        return left;
    }
    Token tmpToken = this->token;
    Consume(TokenType::question);
    auto then = ParseExpr();
    Consume(TokenType::colon);
    auto els = ParseConditionalExpr();
    return sema.semaThreeExprNode(left, then, els, tmpToken);
}


// logor-expr: logand-expr ("||" logand-expr)*
std::shared_ptr<AstNode> Parser::ParseLogOrExpr() {
    auto left = ParseLogAndExpr();
    while (token.tokenType == TokenType::pipepipe) {
        Token tmpToken = token;
        BinaryOp op = BinaryOp::logic_or;
        Advance(); // 前进一个token
        left = sema.semaBinaryExprNode(left, ParseLogAndExpr(), op, tmpToken);
    }
    return left;
}

// logand-expr: bitor-expr ("&&" bitor-expr)*
std::shared_ptr<AstNode> Parser::ParseLogAndExpr() {
    auto left = ParseBitOrExpr();
    while (token.tokenType == TokenType::ampamp) {
        Token tmpToken = token;
        BinaryOp op = BinaryOp::logic_and;
        Advance(); // 前进一个token
        left = sema.semaBinaryExprNode(left, ParseBitOrExpr(), op, tmpToken);
    }
    return left;
}

// bitor-expr: bitxor-expr ("|" bitxor-expr)*
std::shared_ptr<AstNode> Parser::ParseBitOrExpr() {
    auto left = ParseBitXorExpr();
    while (token.tokenType == TokenType::pipe) {
        Token tmpToken = token;
        BinaryOp op = BinaryOp::bitwise_or;
        Advance(); // 前进一个token
        left = sema.semaBinaryExprNode(left, ParseBitXorExpr(), op, tmpToken);
    }
    return left;
}

// bitxor-expr: bitand-expr ("^" bitand-expr)*
std::shared_ptr<AstNode> Parser::ParseBitXorExpr() {
    auto left = ParseBitAndExpr();
    while (token.tokenType == TokenType::caret) {
        Token tmpToken = token;
        BinaryOp op = BinaryOp::bitwise_xor;
        Advance(); // 前进一个token
        left = sema.semaBinaryExprNode(left, ParseBitAndExpr(), op, tmpToken);
    }
    return left;
}

// bitand-expr: equal-expr ("&" equal-expr)*
std::shared_ptr<AstNode> Parser::ParseBitAndExpr() {
    auto left = ParseEqualExpr();
    while (token.tokenType == TokenType::amp) {
        Token tmpToken = token;
        BinaryOp op = BinaryOp::bitwise_and;
        Advance(); // 前进一个token
        left = sema.semaBinaryExprNode(left, ParseEqualExpr(), op, tmpToken);
    }
    return left;
}


// equal-expr : relational-expr (("==" | "!=") relational-expr)*
std::shared_ptr<AstNode> Parser::ParseEqualExpr() {
    auto left = ParseRelationalExpr();
    while (token.tokenType == TokenType::equal_equal || token.tokenType == TokenType::not_equal) {
        BinaryOp op;
        if (token.tokenType == TokenType::equal_equal) {
            op = BinaryOp::equal;
        } else {
            op = BinaryOp::not_equal;
        }
        Token tmpToken = token;
        Advance(); // 前进一个token
        left = sema.semaBinaryExprNode(left, ParseRelationalExpr(), op, tmpToken);
    }
    return left;
}

// relational-expr: add-expr (("<"|">"|"<="|">=") add-expr)*
std::shared_ptr<AstNode> Parser::ParseRelationalExpr() {
    auto left = ParseShiftExpr();
    while (token.tokenType == TokenType::less || token.tokenType == TokenType::less_equal || 
            token.tokenType == TokenType::greater || token.tokenType == TokenType::greater_equal) {
        Token tmpToken = token;
        BinaryOp op;
        if (token.tokenType == TokenType::less) {
            op = BinaryOp::less;
        } else if (token.tokenType == TokenType::less_equal) {
            op = BinaryOp::less_equal;
        } else if (token.tokenType == TokenType::greater) {
            op = BinaryOp::greater;
        } else if (token.tokenType == TokenType::greater_equal) {
            op = BinaryOp::greater_equal;
        }
        Advance(); // 前进一个token
        left = sema.semaBinaryExprNode(left, ParseShiftExpr(), op, tmpToken);
    }
    return left;
}


// add-expr : mult-expr (("+" | "-") mult-expr)* 
std::shared_ptr<AstNode> Parser::ParseAddExpr() {
    auto left = ParseMultExpr();
    while (token.tokenType == TokenType::plus || token.tokenType == TokenType::minus) {
        Token tmpToken = token;
        BinaryOp op;
        if (token.tokenType == TokenType::plus) {
            op = BinaryOp::add;
        } else {
            op = BinaryOp::sub;
        }
        Advance(); // 前进一个token
        left = sema.semaBinaryExprNode(left, ParseMultExpr(), op, tmpToken);
    }
    return left;
}

// 左结合
// mult-expr : primary-expr (("*" | "/") primary-expr)* 
std::shared_ptr<AstNode> Parser::ParseMultExpr() {
    auto left = ParseCastExpr();
    while (token.tokenType == TokenType::star || 
        token.tokenType == TokenType::slash ||
        token.tokenType == TokenType::percent) {
        Token tmpToken = token;
        BinaryOp op;
        if (token.tokenType == TokenType::star) {
            op = BinaryOp::mul;
        } else if (token.tokenType == TokenType::slash) {
            op = BinaryOp::div;
        } else  {
            op = BinaryOp::mod;
        }
        Advance(); // 前进一个token
        left = sema.semaBinaryExprNode(left, ParseCastExpr(), op, tmpToken);
    }
    return left;
}

// 显式类型转换
// cast        ::= unary | "(" type-name ")" cast
std::shared_ptr<AstNode> Parser::ParseCastExpr() {
    if (token.tokenType != TokenType::l_parent) {
        return ParseUnaryExpr();
    }

    bool isTypeName = false;
    {
        lexer.SaveState();
        Token tmpToken;
        lexer.NextToken(tmpToken);
        if (IsTypeName(tmpToken)) {
            isTypeName = true;
        }
        lexer.RestoreState();
    }

    if (isTypeName) {
        // cast node
        Token idenToken = token;
        Consume(TokenType::l_parent);
        std::shared_ptr<CType> type = ParseType();
        Consume(TokenType::r_parent);
        auto node = ParseCastExpr();
        return sema.semaCastExprNode(type, node, idenToken);
    } else {
        return ParseUnaryExpr();
    }
}

// unary     ::= postfix | ("++"|"--"|"&"|"*"|"+"|"-"|"~"|"!"|"sizeof") unary
//                  | "sizeof" "(" type-name ")"
std::shared_ptr<AstNode> Parser::ParseUnaryExpr() {
    if (!IsUnaryOperator()) {
        auto left = ParsePostfixExpr();
        return left;
    }

    // handle sizeof
    if (this->token.tokenType == TokenType::kw_sizeof) {
        bool isTypeName = false;
        Consume(TokenType::kw_sizeof);

        if (token.tokenType == TokenType::l_parent) {
            lexer.SaveState();

            Token tmpToken;
            lexer.NextToken(tmpToken);
            if (IsTypeName(tmpToken)) {
                isTypeName = true;
            }
            lexer.RestoreState();
        }

        if (isTypeName) {
            Consume(TokenType::l_parent);
            auto type = ParseType();
            Consume(TokenType::r_parent);
            return sema.semaSizeOfExprNode(nullptr, type);
        } else {
            auto node = ParseUnaryExpr(); 
            return sema.semaSizeOfExprNode(node, nullptr);
        }
    }

    UnaryOp op;
    switch (token.tokenType)
    {
    case TokenType::plus_plus:
        op = UnaryOp::inc;
        break;
    case TokenType::minus_minus:
        op = UnaryOp::dec;
        break;
    case TokenType::plus:
        op = UnaryOp::positive;
        break;
    case TokenType::minus:
        op = UnaryOp::negative;
        break;
    case TokenType::star:
        op = UnaryOp::deref;
        break;
    case TokenType::amp:
        op = UnaryOp::addr;
        break;
    case TokenType::exclaim:
        op = UnaryOp::logic_not;
        break;
    case TokenType::tilde:
        op = UnaryOp::bitwise_not;
        break;
    default:
        break;
    }
    NextToken();

    Token tmpToken = token;
    auto node = ParseUnaryExpr();
    return sema.semaUnaryExprNode(node, op, tmpToken);
}

/// 解析后缀表达式, 如：
/// a++, a--, p++, p--
/// a[3][5]
std::shared_ptr<AstNode> Parser::ParsePostfixExpr() {
    auto left = ParsePrimaryExpr();
    for(;;) {
        if (this->token.tokenType == TokenType::plus_plus) {
            left = sema.semaPostIncExprNode(left, this->token);
            Consume(TokenType::plus_plus);
            continue;
        } 
        
        // a--
        if (this->token.tokenType == TokenType::minus_minus) {
            left = sema.semaPostDecExprNode(left, this->token);
            Consume(TokenType::minus_minus);
            continue;
        } 
        
        // arr[2][3]
        if (this->token.tokenType == TokenType::l_bracket) {
            Token tmp = token;
            Consume(TokenType::l_bracket);
            auto node = ParseExpr();
            Consume(TokenType::r_bracket);
            left = sema.semaPostSubscriptNode(left, node, tmp);
            continue;
        } 
        
        // stu.name 结构体成员访问
        if (this->token.tokenType == TokenType::dot) {
            Token dottoken = token;
            Consume(TokenType::dot);
            Token tmpToken = token;
            Consume(TokenType::identifier);
            left = sema.semaPostMemberDotNode(left, tmpToken, dottoken);
            continue;
        }

        // stu->name 结构体成员访问
        if (this->token.tokenType == TokenType::arrow) {
            Token arrowtoken = token;
            Consume(TokenType::arrow);
            Token tmpToken = token;
            Consume(TokenType::identifier);
            left = sema.semaPostMemberArrowNode(left, tmpToken, arrowtoken);
            continue;
        }

        // 函数调用
        if (this->token.tokenType == TokenType::l_parent) {
            Consume(TokenType::l_parent);
            /*
                注意：函数的实参是一个赋值表达式：
                arg-expr-list := assign ("," assign)*
            */ 
            std::vector<std::shared_ptr<AstNode>> args;
            int i = 0;
            // 这里应该是一个新的作用域
            while(token.tokenType != TokenType::r_parent) {
                if (i > 0 && token.tokenType ==TokenType::comma) {
                    Consume(TokenType::comma);
                }
                args.push_back(ParseAssignExpr());
                i++;
            }
            Consume(TokenType::r_parent);
            left = sema.semaFuncCallExprNode(left, args);
            continue;
        }

        break;
    }
    return left;
}

// ParsePrimaryExpr解析基础表达式
// primary-expr : identifier | number | "(" expr ")" 
std::shared_ptr<AstNode> Parser::ParsePrimaryExpr() {
    if (token.tokenType == TokenType::l_parent) {
        // 括号表达式
        NextToken();

        auto expr = ParseExpr();
        assert(Expect(TokenType::r_parent));

        NextToken();
        return expr;
    } else if (token.tokenType == TokenType::identifier) {
        // 说明是变量访问
        auto expr = sema.semaVariableAccessNode(token);
        NextToken();
        return expr;
    } else if (token.tokenType == TokenType::str) {
        // 对字符串字面量进行语义分析
        auto expr = sema.semaStringExprNode(token, token.strValue, token.type);
        Consume(TokenType::str);
        return expr;
    } else {
        // 对数值字面量进行语义分析
        Expect(TokenType::number);
        auto factor = sema.semaNumberExprNode(token, token.type);
        NextToken();
        return factor;
    }
}

// shift-expr: add-expr (("<<" | ">>") add-expr)*
std::shared_ptr<AstNode> Parser::ParseShiftExpr() {
    auto left = ParseAddExpr();
    while (token.tokenType == TokenType::less_less || token.tokenType == TokenType::greater_greater) {
        Token tmpToken = token;
        BinaryOp op;
        if (token.tokenType == TokenType::less_less) {
            op = BinaryOp::left_shift;
        } else {
            op = BinaryOp::right_shift;
        }
        Advance(); // 前进一个token
        left = sema.semaBinaryExprNode(left, ParseAddExpr(), op, tmpToken);
    }
    return left;
}

/// 解析类型如: sizeof 后面的类型
/// sizeof(int)
/// sizeof(int [3][4])
/// sizeof(int *[3][4])
std::shared_ptr<CType> Parser::ParseType() {
    bool isTypedef = false;
    std::shared_ptr<CType> baseType = ParseDeclSpec(isTypedef);
    // baseType 不能为空
    assert(baseType);

    // 解析指针类型
    while(token.tokenType == TokenType::star) {
        baseType = std::make_shared<CPointType>(baseType);
        Consume(TokenType::star);
    }

    baseType = DirectDeclaratorSuffix(token, baseType, false);
    return baseType;
}


bool Parser::Expect(TokenType tokenType) {
    if (token.tokenType == tokenType) {
        return true;
    }
    GetDiagEngine().Report(
        llvm::SMLoc::getFromPointer(token.ptr), 
        diag::err_expected, 
        Token::GetSpellingText(tokenType), 
        llvm::StringRef(token.ptr, token.len));
    return false;
}

bool Parser::Consume(TokenType tokenType) {
    if (Expect(tokenType)) {
        Advance();
        return true;
    }
    return false;
}

void Parser::Advance() {
    lexer.NextToken(token);
}

void Parser::NextToken() {
    lexer.NextToken(token);
}

// 消解类型类型限定符（在类型声明时有类型限定符，如const, static）
void Parser::ConsumeTypeQualify() {
    while (token.tokenType == TokenType::kw_const ||
        token.tokenType == TokenType::kw_volatile ||
        token.tokenType == TokenType::kw_static) {
            NextToken();
    }
}



bool Parser::IsTypeName(Token token) {
    TokenType tokenType = token.tokenType;
    if (tokenType == TokenType::kw_void ||
        tokenType == TokenType::kw_char ||
        tokenType == TokenType::kw_short ||
        tokenType == TokenType::kw_int ||  
        tokenType == TokenType::kw_long || 
        tokenType == TokenType::kw_float ||
        tokenType == TokenType::kw_double ||
        tokenType == TokenType::kw_signed ||
        tokenType == TokenType::kw_unsigned ||
        tokenType == TokenType::kw_static ||
        tokenType == TokenType::kw_extern ||
        tokenType == TokenType::kw_auto ||
        tokenType == TokenType::kw_register ||
        tokenType == TokenType::kw_typedef ||
        tokenType == TokenType::kw_const ||
        tokenType == TokenType::kw_volatile ||
        tokenType == TokenType::kw_inline ||
        tokenType == TokenType::kw_typedef || 
        tokenType == TokenType::kw_struct || 
        tokenType == TokenType::kw_union) {
        return true;
    } else if (tokenType == TokenType::identifier) {
        if (sema.semaTypedefAccess(token) != nullptr) {
            return true;
        }
    }
    return false;
}

bool Parser::IsFunctionDecl() {
    bool isFuncDecl = false;
    Token begin = token;

    sema.SetMode(Sema::Mode::Skip);
    lexer.SaveState();

    bool isTypedef = false;
    std::shared_ptr<CType> baseType = ParseDeclSpec(isTypedef);   // 解析声明说明符
    if (token.tokenType == TokenType::semi) {
        /*
        如果是分号， 那么就是一个纯粹的类型定义，如：
            struct A {
                int x;
            };
        */
        isFuncDecl = false;
    } else {
        auto node = Declarator(baseType, true);              // 解析声明符
        isFuncDecl = IsFunctionTypeNode(node);   
    }

    lexer.RestoreState();
    token = begin;
    sema.UnSetMode();
    return isFuncDecl;
}


// 用于判断一个 AST 节点（AstNode）的类型是否为函数类型，包括：
// 直接是函数类型：int func()
// 函数指针：int (*pf)()
// 多级指针指向函数：int (**ppf)() 
bool Parser::IsFunctionTypeNode(std::shared_ptr<AstNode> node) {
    if (node->type->GetKind() == CType::TY_Func) {
        return true;
    }

    CType *type = node->type.get();
    while (type->GetKind() == CType::TY_Point) {
        CPointType *cPtrType = llvm::dyn_cast<CPointType>(type);

        if (cPtrType->GetBaseType()->GetKind() == CType::TY_Point) {
            type = cPtrType->GetBaseType().get();
            continue;
        }

        // 最后一层指针指向的是函数类型 → 说明这是函数指针
        if (cPtrType->GetBaseType()->GetKind() == CType::TY_Func){
            return true;
        }
        // 既不是指针也不是函数，退出
        break;
    }

    return false;
}


bool Parser::IsStringArrayType(std::shared_ptr<CType> ty) {
    if (ty->GetKind() == CType::TY_Array) {
        CArrayType *arrayTy = llvm::dyn_cast<CArrayType>(ty.get());
        if (arrayTy->GetElementType()->GetKind() == CType::TY_Char) {
            return true;
        }
    }
    return false;
}


bool Parser::IsAssignOperator() {
    return token.tokenType == TokenType::equal
        || token.tokenType == TokenType::plus_equal
        || token.tokenType == TokenType::minus_equal
        || token.tokenType == TokenType::star_equal
        || token.tokenType == TokenType::slash_equal
        || token.tokenType == TokenType::percent_equal
        || token.tokenType == TokenType::pipe_equal
        || token.tokenType == TokenType::amp_equal
        || token.tokenType == TokenType::caret_equal
        || token.tokenType == TokenType::less_less_equal
        || token.tokenType == TokenType::greater_greater_equal;
}


bool Parser::IsUnaryOperator() {
    return token.tokenType == TokenType::plus_plus
        || token.tokenType == TokenType::minus_minus
        || token.tokenType == TokenType::plus
        || token.tokenType == TokenType::minus
        || token.tokenType == TokenType::star
        || token.tokenType == TokenType::amp
        || token.tokenType == TokenType::exclaim
        || token.tokenType == TokenType::tilde
        || token.tokenType == TokenType::kw_sizeof;
}