#include "printVisitor.h"


PrintVisitor::PrintVisitor(std::shared_ptr<Program> program, llvm::raw_ostream* out) {
    this->out = out;
    VisitProgram(program.get());
}

llvm::Value* PrintVisitor::VisitProgram(Program *program) {
    for (const auto &decl : program->externalDecls) {
        decl->Accept(this);
    }
    return nullptr;
}

llvm::Value* PrintVisitor::VisitDeclareStmt(DeclareStmt *declstmt) {
    int i = 0;
    int size = declstmt->nodeVec.size();
    for (const auto & node : declstmt->nodeVec){
        node->Accept(this);
        if (i != size-1) {
            *out << ";";
        }
        i++;
    }
    return nullptr;
}

llvm::Value* PrintVisitor::VisitIfStmt(IfStmt *ifstmt) {
    *out << "if (";
    ifstmt->condNode->Accept(this);
    *out << ")\n";
    ifstmt->thenNode->Accept(this);
    if (ifstmt->elseNode != nullptr) {
        *out << "\nelse\n";
        ifstmt->elseNode->Accept(this);
    }
    return nullptr;
}

llvm::Value* PrintVisitor::VisitBlockStmt(BlockStmt *blockstmt) {
    *out << "{";
    for (const auto & node : blockstmt->nodeVec) {
        node->Accept(this);
        *out << ";";
    }
    *out << "}";
    return nullptr;
}

llvm::Value* PrintVisitor::VisitForStmt(ForStmt *ifstmt) {
    *out << "for (";
    if (ifstmt->initNode != nullptr) {
        ifstmt->initNode->Accept(this);
    }
    *out << "; ";
    if (ifstmt->condNode != nullptr) {
        ifstmt->condNode->Accept(this);
    }
    *out << "; ";
    if (ifstmt->incNode != nullptr) {
        ifstmt->incNode->Accept(this);
    }
    *out << ") ";
    if (ifstmt->bodyNode != nullptr) {
        ifstmt->bodyNode->Accept(this);
    }
    return nullptr;
}

llvm::Value* PrintVisitor::VisitDoWhileStmt(DoWhileStmt *stmt) {
    *out << "do";
    stmt->body->Accept(this);
    *out << "while(";
    stmt->cond->Accept(this);
    *out << ")";
    return nullptr;
};

llvm::Value* PrintVisitor::VisitSwitchStmt(SwitchStmt *stmt) {
    *out << "switch(";
    stmt->expr->Accept(this);
    *out << ")";
    stmt->stmt->Accept(this);
    return nullptr;
};

llvm::Value* PrintVisitor::VisitCaseStmt(CaseStmt *stmt) {
    *out << "case ";
    stmt->expr->Accept(this);
    *out << ":";
    stmt->stmt->Accept(this);
    return nullptr;
};

llvm::Value* PrintVisitor::VisitDefaultStmt(DefaultStmt *stmt) {
    *out << "default:";
    stmt->stmt->Accept(this);
    return nullptr;
};


llvm::Value* PrintVisitor::VisitBreakStmt(BreakStmt *breakstmt) {
    *out << "break";
     return nullptr;
}

llvm::Value* PrintVisitor::VisitContinueStmt(ContinueStmt *continuestmt) {
    *out << "continue";
    return nullptr;
}

llvm::Value* PrintVisitor::VisitReturnStmt(ReturnStmt *stmt) {
    *out << "return ";
    if (stmt->expr != nullptr) {
        stmt->expr->Accept(this);
    }
    return nullptr;
}

llvm::Value* PrintVisitor::VisitVariableDeclExpr(VariableDecl *decl) {
    decl->type->Accept(this);

    llvm::StringRef text(decl->token.ptr, decl->token.len);
    *out << text;

    if (decl->initValues.size() > 0) {
        *out << " = ";
        int i = 0;
        int size = decl->initValues.size();
        for (auto &initVal : decl->initValues) {
            initVal->value->Accept(this);
            if (i < size -1) {
                *out << ",";
            }
            i++;
        }
    }
    return nullptr;

    // if (decl->init != nullptr) {
    //     *out << " = ";
    //     decl->init->Accept(this);
    // }
    // return nullptr;
}

llvm::Value* PrintVisitor::VisitFunctionDeclExpr(FunctionDecl *decl) {
    decl->type->Accept(this);
    if (decl->blockStmt != nullptr) {
        decl->blockStmt->Accept(this);
    } else {
        *out << ";";
    }
    return nullptr;
}


llvm::Value* PrintVisitor::VisitVariableAccessExpr(VariableAccessExpr *expr) {
    *out << llvm::StringRef(expr->token.ptr, expr->token.len);
    return nullptr;
}


llvm::Value* PrintVisitor::VisitBinaryExpr(BinaryExpr *binaryExpr) {
    binaryExpr->left->Accept(this);
    switch (binaryExpr->op)
    {
    case BinaryOp::add:{
        *out << " + ";
        break;
    }
    case BinaryOp::sub:{
        *out << " - ";
        break;
    }
    case BinaryOp::mul:{
        *out << " * ";
        break;
    }
    case BinaryOp::div:{
        *out << " / ";
        break;
    }
    case BinaryOp::equal:{
        *out << " == ";
        break;
    }
    case BinaryOp::not_equal:{
        *out << " != ";
        break;
    }
    case BinaryOp::less:{
        *out << " < ";
        break;
    }
    case BinaryOp::less_equal:{
        *out << " <= ";
        break;
    }
    case BinaryOp::greater:{
        *out << " > ";
        break;
    }
    case BinaryOp::greater_equal:{
        *out << " >= ";
        break;
    }
    case BinaryOp::mod:{
        *out << " % ";
        break;
    }
    case BinaryOp::logic_or:{
        *out << " || ";
        break;
    }
    case BinaryOp::logic_and:{
        *out << " && ";
        break;
    }
    case BinaryOp::bitwise_and:{
        *out << " & ";
        break;
    }
    case BinaryOp::bitwise_or:{
        *out << " | ";
        break;
    }
    case BinaryOp::bitwise_xor:{
        *out << " ^ ";
        break;
    }
    case BinaryOp::left_shift:{
        *out << " << ";
        break;
    }
    case BinaryOp::right_shift:{
        *out << " >> ";
        break;
    }
    case BinaryOp::comma:
        *out << " , ";
        break;
    case BinaryOp::assign:
        *out << " = ";
        break;
    case BinaryOp::add_assign:
        *out << " += ";
        break;
    case BinaryOp::sub_assign:
        *out << " -= ";
        break;
    case BinaryOp::mul_assign:
        *out << " *= ";
        break;
    case BinaryOp::div_assign:
        *out << " /= ";
        break;
    case BinaryOp::mod_assign:
        *out << " %= ";
        break;
    case BinaryOp::bitwise_and_assign:
        *out << " &= ";
        break;
    case BinaryOp::bitwise_or_assign:
        *out << " |= ";
        break;
    case BinaryOp::bitwise_xor_assign:
        *out << " ^= ";
        break;
    case BinaryOp::left_shift_assign:
        *out << " <<= ";
        break;
    case BinaryOp::right_shift_assign:
        *out << " >>= ";
        break;
    default:
        break;
    }
    binaryExpr->right->Accept(this);
    return nullptr;
}

llvm::Value* PrintVisitor::VisitNumberExpr(NumberExpr *expr) {
    if (expr->type->IsIntegerType()) {
        *out << expr->value.v;
    } else {
        *out << expr->value.d;
    }

    return nullptr;
}

llvm::Value* PrintVisitor::VisitStringExpr(StringExpr *expr) {
    *out << llvm::StringRef(expr->token.ptr, expr->token.len);
    return nullptr;
}


llvm::Value* PrintVisitor::VisitThreeExpr(ThreeExpr *threeExpr) {
    threeExpr->cond->Accept(this);
    *out << "?";
    threeExpr->then->Accept(this);
    *out << ":";
    threeExpr->els->Accept(this);
    return nullptr;
}

llvm::Value* PrintVisitor::VisitUnaryExpr(UnaryExpr *unaryExpr) {
    switch (unaryExpr->op)
    {
    case UnaryOp::positive:
        *out << "+";
        break;
    case UnaryOp::negative:
        *out << "-";
        break;
    case UnaryOp::deref:
        *out << "*";
        break;
    case UnaryOp::addr:
        *out << "&";
        break;
    case UnaryOp::logic_not:
        *out << "!";
        break;
    case UnaryOp::bitwise_not:
        *out << "~";
        break;
    case UnaryOp::inc:
        *out << "++";
        break;
    case UnaryOp::dec:
        *out << "--";
        break;
    default:
        break;
    }
    unaryExpr->node->Accept(this);
    return nullptr;
}

llvm::Value* PrintVisitor::VisitCastExpr(CastExpr *expr) {
    *out << "(";
    expr->targetType->Accept(this);
    *out << ")";
    expr->node->Accept(this);
    
    return nullptr;
}

llvm::Value* PrintVisitor::VisitSizeOfExpr(SizeOfExpr *sizeofExpr) {
    *out << "sizeof ";
    if (sizeofExpr->ty != nullptr) {
        *out << "(";
        sizeofExpr->ty->Accept(this);
        *out << ")";
    } else {
        sizeofExpr->node->Accept(this);
    }
    return nullptr;
}

llvm::Value* PrintVisitor::VisitPostIncExpr(PostIncExpr *postIncExpr) {
    postIncExpr->left->Accept(this);
    *out << "++";
    return nullptr;
}

llvm::Value* PrintVisitor::VisitPostDecExpr(PostDecExpr *postDecExpr) {
    postDecExpr->left->Accept(this);
    *out << "--";
    return nullptr;
}

llvm::Value* PrintVisitor::VisitPostSubscript(PostSubscript *expr) {
    expr->left->Accept(this);
    *out << "[";
    expr->node->Accept(this);
    *out << "]";
    return nullptr;
}

llvm::Value* PrintVisitor::VisitPostMemberDotExpr(PostMemberDotExpr *expr) {
    expr->left->Accept(this);
    *out << ".";
    *out << expr->member.name;
    return nullptr;
}

llvm::Value* PrintVisitor::VisitPostMemberArrowExpr(PostMemberArrowExpr *expr) {
    expr->left->Accept(this);
    *out << "->";
    *out << expr->member.name;
    return nullptr;
}


llvm::Value* PrintVisitor::VisitPostFunctionCallExpr(PostFunctionCallExpr *expr) {
    expr->left->Accept(this);
    *out << "(";
    int i = 0;
    int size = expr->args.size();
    for (const auto &arg : expr->args) {
        arg->Accept(this);
        if (i < size - 1) {
            *out << ",";
        }
        i++;
    }
    *out << ")";
    return nullptr;
}


llvm::Type* PrintVisitor::VisitPrimaryType(CPrimaryType *type) {
    if (type->GetKind() == CType::TY_Int) {
        *out << "int ";
    } else if (type->GetKind() == CType::TY_Void) {
        *out << "void ";
    } else if (type->GetKind() == CType::TY_Char) {
        *out << "char ";
    }
    return nullptr;
}

llvm::Type* PrintVisitor::VisitPointType(CPointType *type) {
    /// 逆序输出
    type->GetBaseType()->Accept(this);
    *out << "*";
    return nullptr;
}

// int a[3][4]
llvm::Type* PrintVisitor::VisitArrayType(CArrayType *type) {
    *out << "[" << type->GetElementCount() << "]";
    type->GetElementType()->Accept(this);
    
    return nullptr;
}


llvm::Type* PrintVisitor::VisitRecordType(CRecordType *type) {
    TagKind tagkind = type->GetTagKind();
    if (tagkind == TagKind::KStruct) {
        *out << "struct ";
    } else {
        *out << "union ";
    }

    *out << type->GetName() << "{";
    for (auto &mbr : type->GetMembers()) {
        mbr.ty->Accept(this);
        *out << mbr.name << ";";
    }
    *out << "} ";
    return nullptr;
}

llvm::Type* PrintVisitor::VisitFuncType(CFuncType *type) {
    type->GetRetType()->Accept(this);
    *out << type->GetName() << "(";
    int i = 0;
    int size = type->GetParams().size();
    for (const auto &p : type->GetParams()) {
        p.type->Accept(this);
        *out << " " << p.name;
        if (i < size - 1) {
            *out << ",";
        }
        i++;
    }

    if (type->IsVarArg()) {
        *out << ", ...";
    }

    *out << ")";
    return nullptr;
}