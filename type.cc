#include "type.h"

// CType* CType::GetIntTy() {
//     static CType intType(TypeKind::Int, 4, 4);
//     return &intType;
// }

// 静态成员变量必须在类内声明，在类外初始化
std::shared_ptr<CType> CType::IntType = std::make_shared<CType>(TypeKind::Int, 4, 4);