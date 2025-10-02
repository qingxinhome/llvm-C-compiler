#include "type.h"

CType* CType::GetIntTy() {
    static CType intType(TypeKind::Int, 4, 4);
    return &intType;
}