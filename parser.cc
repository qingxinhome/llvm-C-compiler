#include "parser.h"

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
    std::shared_ptr<CType> baseType = ParseDeclSpec();          // 解析声明说明符

    // 从函数定义的参数列表开始就是函数的作用域
    sema.EnterScope();
    std::shared_ptr<AstNode> node = Declarator(baseType, true); // 解析声明符

    std::shared_ptr<AstNode> blockStmt = nullptr;
    // 遇到 ；号，说明是C语言函数的声明
    if (token.tokenType != TokenType::semi) {
        blockStmt = ParseBlockStmt();
    } else {
        Consume(TokenType::semi);
    }
    sema.ExitScope();
    
    return sema.semaFunctionDecl(node->token, node->type, blockStmt);
}



std::shared_ptr<AstNode> Parser::ParseStmt() {
    // null statement
    if (token.tokenType == TokenType::semi) {
        NextToken();
        return nullptr;
    }
    // declare statment
    else if (IsTypeName(token.tokenType)) {
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
    // expr statement
    else {
        return ParseExprStmt();
    }   
}

// 处理声明语句中的基础类型，如：int *p; 中的int
std::shared_ptr<CType> Parser::ParseDeclSpec() {
    // 先消解类型类型限定符，如const, static）
    ConsumeTypeQulify();
    if (token.tokenType == TokenType::kw_int) {
        Consume(TokenType::kw_int);
        return CType::IntType;
    } else if (token.tokenType == TokenType::kw_struct 
        || token.tokenType == TokenType::kw_union) {
        return ParseStructOrUnionSpec();
    } else if (token.tokenType == TokenType::kw_void) {
        Consume(TokenType::kw_void);
        return CType::VoidType;
    } else if (token.tokenType == TokenType::kw_char) {
        Consume(TokenType::kw_char);
        return CType::CharType;
    }
    GetDiagEngine().Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_type);
    return nullptr;
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
        Expect(TokenType::number);
        elemCount = token.value;
        Consume(TokenType::number);
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

        // 可变参数 ... 只能是函数最后一个参数
        if (i > 0 && token.tokenType == TokenType::ellipse) {
            isVarArg = true;
            Consume(TokenType::ellipse);
            break;
        }

        // 解析函数形参
        auto ty = ParseDeclSpec();
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
        sema.SetMode(Sema::Mode::Normal);
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
        ParseInitializer(variableDecl->initValues, variableDecl->type, offsetList, false);
    }
    return varDeclNode;
}


bool Parser::ParseStringInitializer(std::vector<std::shared_ptr<VariableDecl::InitValue>> &arr, 
    std::shared_ptr<CType> declType, 
    std::vector<int> &offsetList
) {
    Token strToken = token;
    Consume(TokenType::string);

    CArrayType *arrayTy = llvm::dyn_cast<CArrayType>(declType.get());

    int elemCount = arrayTy->GetElementCount();
    if (elemCount < 0) {
        elemCount = token.strValue.size();
        arrayTy->SetElementCount(elemCount);
    }

    int strCount = strToken.strValue.size();
    if (elemCount < strCount) {
        GetDiagEngine().Report(llvm::SMLoc::getFromPointer(token.ptr), diag::err_large_len);
    }
    
    
    // 字符串初始值的长度小于等于声明长度， 如： 
    // char a[10] = "12345";  or  char a[10] = "0123456789";
    for (int i = 0; i < strCount; i++) {
        // 把字符串的每一个字符当成一个number表达式
        auto numNode = sema.semaNumberExprNode(strToken, strToken.strValue[i] ,CType::IntType);
        offsetList.push_back(i);
        auto initValue = sema.semaDeclInitValue(arrayTy->GetElementType(), numNode, offsetList, strToken);
        offsetList.pop_back();
        arr.push_back(initValue);
    }
    // 对于字符数组剩余未初始化的字符值，置为0
    for (int i = strCount; i < elemCount; i++) {
        auto numNode = sema.semaNumberExprNode(strToken, 0,CType::IntType);
        offsetList.push_back(i);
        auto initValue = sema.semaDeclInitValue(arrayTy->GetElementType(), numNode, offsetList, strToken);
        offsetList.pop_back();
        arr.push_back(initValue);
    }
    return true;
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
    if (token.tokenType == TokenType::string && IsStringArrayTy(declType)) {
        return ParseStringInitializer(arr, declType, offsetList);
    }

    // {,,}
    if (token.tokenType == TokenType::l_brace) {
        Consume(TokenType::l_brace);

        //  处理：char a[] = {"abcdef"}; 这种情况
        if (token.tokenType == TokenType::string && IsStringArrayTy(declType)) {
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
        Consume(TokenType::r_brace);
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
    std::shared_ptr<CType> baseTy = ParseDeclSpec();

    // 处理无意义的声明， 形如： int ; 
    if (token.tokenType == TokenType::semi) {
        Consume(TokenType::semi);
        return nullptr;
    }

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
        /*
        assert(token.tokenType == TokenType::identifier);

        // 获取变量名
        // auto variableName = token.content;
        Token tmpToken = token;  // int a

        std::shared_ptr<VariableDecl> variableDecl = sema.semaVariableDeclNode(token, baseTy);
        declareStmt->nodeVec.push_back(variableDecl);

        Consume(TokenType::identifier);
        if (token.tokenType == TokenType::equal) {
            auto opToken = token;
            NextToken();

            std::shared_ptr<VariableAccessExpr> left = sema.semaVariableAccessNode(tmpToken);
            auto right = ParseExpr();
            
            std::shared_ptr<AssignExpr> assignExpr = sema.semaAssignExprNode(left, right, opToken);
            declareStmt->nodeVec.push_back(assignExpr);
        }
        */
    }
    Consume(TokenType::semi);
    return declareStmt;
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
    breakNodes.push_back(forStmt);
    continueNodes.push_back(forStmt);

    std::shared_ptr<AstNode> initNode;
    std::shared_ptr<AstNode> condNode;
    std::shared_ptr<AstNode> incNode;
    std::shared_ptr<AstNode> bodyNode;

    if (IsTypeName(token.tokenType)) {
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
        Consume(TokenType::comma);
        left = sema.semaBinaryExprNode(left, ParseAssignExpr(), BinaryOp::comma);
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
    return sema.semaBinaryExprNode(left, ParseAssignExpr(), op);

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
        BinaryOp op = BinaryOp::logic_or;
        Advance(); // 前进一个token
        left = sema.semaBinaryExprNode(left, ParseLogAndExpr(), op);
    }
    return left;
}

// logand-expr: bitor-expr ("&&" bitor-expr)*
std::shared_ptr<AstNode> Parser::ParseLogAndExpr() {
    auto left = ParseBitOrExpr();
    while (token.tokenType == TokenType::ampamp) {
        BinaryOp op = BinaryOp::logic_and;
        Advance(); // 前进一个token
        left = sema.semaBinaryExprNode(left, ParseBitOrExpr(), op);
    }
    return left;
}

// bitor-expr: bitxor-expr ("|" bitxor-expr)*
std::shared_ptr<AstNode> Parser::ParseBitOrExpr() {
    auto left = ParseBitXorExpr();
    while (token.tokenType == TokenType::pipe) {
        BinaryOp op = BinaryOp::bitwise_or;
        Advance(); // 前进一个token
        left = sema.semaBinaryExprNode(left, ParseBitXorExpr(), op);
    }
    return left;
}

// bitxor-expr: bitand-expr ("^" bitand-expr)*
std::shared_ptr<AstNode> Parser::ParseBitXorExpr() {
    auto left = ParseBitAndExpr();
    while (token.tokenType == TokenType::caret) {
        BinaryOp op = BinaryOp::bitwise_xor;
        Advance(); // 前进一个token
        left = sema.semaBinaryExprNode(left, ParseBitAndExpr(), op);
    }
    return left;
}

// bitand-expr: equal-expr ("&" equal-expr)*
std::shared_ptr<AstNode> Parser::ParseBitAndExpr() {
    auto left = ParseEqualExpr();
    while (token.tokenType == TokenType::amp) {
        BinaryOp op = BinaryOp::bitwise_and;
        Advance(); // 前进一个token
        left = sema.semaBinaryExprNode(left, ParseEqualExpr(), op);
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
        Advance(); // 前进一个token
        left = sema.semaBinaryExprNode(left, ParseRelationalExpr(), op);
    }
    return left;
}

// relational-expr: add-expr (("<"|">"|"<="|">=") add-expr)*
std::shared_ptr<AstNode> Parser::ParseRelationalExpr() {
    auto left = ParseShiftExpr();
    while (token.tokenType == TokenType::less || token.tokenType == TokenType::less_equal || 
            token.tokenType == TokenType::greater || token.tokenType == TokenType::greater_equal) {
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
        left = sema.semaBinaryExprNode(left, ParseShiftExpr(), op);
    }
    return left;
}


// add-expr : mult-expr (("+" | "-") mult-expr)* 
std::shared_ptr<AstNode> Parser::ParseAddExpr() {
    auto left = ParseMultExpr();
    while (token.tokenType == TokenType::plus || token.tokenType == TokenType::minus) {
        BinaryOp op;
        if (token.tokenType == TokenType::plus) {
            op = BinaryOp::add;
        } else {
            op = BinaryOp::sub;
        }
        Advance(); // 前进一个token
        left = sema.semaBinaryExprNode(left, ParseMultExpr(), op);
    }
    return left;
}

// 左结合
// mult-expr : primary-expr (("*" | "/") primary-expr)* 
std::shared_ptr<AstNode> Parser::ParseMultExpr() {
    auto left = ParseUnaryExpr();
    while (token.tokenType == TokenType::star || 
        token.tokenType == TokenType::slash ||
        token.tokenType == TokenType::percent) {
        BinaryOp op;
        if (token.tokenType == TokenType::star) {
            op = BinaryOp::mul;
        } else if (token.tokenType == TokenType::slash) {
            op = BinaryOp::div;
        } else  {
            op = BinaryOp::mod;
        }
        Advance(); // 前进一个token
        left = sema.semaBinaryExprNode(left, ParseUnaryExpr(), op);
    }
    return left;
}

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
            if (IsTypeName(tmpToken.tokenType)) {
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
    } else if (token.tokenType == TokenType::string) {
        // 对字符串字面量进行语义分析
        auto expr = sema.semaStringExprNode(token, token.type);
        NextToken();
        return expr;
    } else {
        // 对数值字面量进行语义分析
        Expect(TokenType::number);
        auto factor = sema.semaNumberExprNode(token, token.value, token.type);
        NextToken();
        return factor;
    }
}

// shift-expr: add-expr (("<<" | ">>") add-expr)*
std::shared_ptr<AstNode> Parser::ParseShiftExpr() {
    auto left = ParseAddExpr();
    while (token.tokenType == TokenType::less_less || token.tokenType == TokenType::greater_greater) {
        BinaryOp op;
        if (token.tokenType == TokenType::less_less) {
            op = BinaryOp::left_shift;
        } else {
            op = BinaryOp::right_shift;
        }
        Advance(); // 前进一个token
        left = sema.semaBinaryExprNode(left, ParseAddExpr(), op);
    }
    return left;
}

/// 解析类型如: sizeof 后面的类型
/// sizeof(int)
/// sizeof(int [3][4])
/// sizeof(int *[3][4])
std::shared_ptr<CType> Parser::ParseType() {   
    std::shared_ptr<CType> baseType = ParseDeclSpec();
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
void Parser::ConsumeTypeQulify() {
    while (token.tokenType == TokenType::kw_const ||
        token.tokenType == TokenType::kw_volatile ||
        token.tokenType == TokenType::kw_static ||
        token.tokenType == TokenType::kw_extern) {
            NextToken();
    }
}



bool Parser::IsTypeName(TokenType tokenType) {
    if (tokenType == TokenType::kw_int) {
        return true;
    } else if (tokenType == TokenType::kw_struct || tokenType == TokenType::kw_union) {
        return true;
    } else if (tokenType == TokenType::kw_void) {
        return true;
    } else if (tokenType == TokenType::kw_char) {
        return true;
    }
    return false;
}

bool Parser::IsFunctionDecl() {
    bool isFuncDecl = false;
    Token begin = token;

    sema.SetMode(Sema::Mode::Skip);
    lexer.SaveState();

    std::shared_ptr<CType> baseType = ParseDeclSpec();   // 解析声明说明符
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
        if (node->type->GetKind() == CType::Kind::TY_Func) {
            isFuncDecl = true;
        }       
    }

    lexer.RestoreState();
    sema.SetMode(Sema::Mode::Normal);
    token = begin;

    return isFuncDecl;
}


bool Parser::IsStringArrayTy(std::shared_ptr<CType> ty) {
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