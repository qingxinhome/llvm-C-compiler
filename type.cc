#include "type.h"

// CType* CType::GetIntTy() {
//     static CType intType(TypeKind::Int, 4, 4);
//     return &intType;
// }

// 静态成员变量必须在类内声明，在类外初始化
std::shared_ptr<CType> CType::IntType = std::make_shared<CPrimaryType>(Kind::TY_Int, 4, 4);



CRecordType::CRecordType(llvm::StringRef name, const std::vector<Member> &members, TagKind tagKind) : 
    CType(Kind::TY_Record, 0, 0), name(name), members(members), tagKind(tagKind) {
    //TODO
    // update size and align
    return;
}
    