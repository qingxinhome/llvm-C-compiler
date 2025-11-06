#include "type.h"

// CType* CType::GetIntTy() {
//     static CType intType(TypeKind::Int, 4, 4);
//     return &intType;
// }

// CType类的静态成员变量的初始化: 必须在类内声明，在类外初始化
std::shared_ptr<CType> CType::IntType = std::make_shared<CPrimaryType>(Kind::TY_Int, 4, 4);
// void 类型的大小和对齐 都是 0， 因为它是空类型
std::shared_ptr<CType> CType::VoidType = std::make_shared<CPrimaryType>(Kind::TY_Void, 0, 0);
std::shared_ptr<CType> CType::CharType = std::make_shared<CPrimaryType>(Kind::TY_Char, 1, 1);

llvm::StringRef CType::GenAnonyRecordName(TagKind tagKind) {
    // 定义静态局部变量
    static long long idx = 0;
    std::string name;
    if (tagKind == TagKind::KStruct) {
        name = "_1anony_struct_" + std::to_string(idx++) + "_";
    } else {
        name = "_1anony_union_" + std::to_string(idx++) + "_";
    }

    // 申请一块持久化的内存（在编译器退出的时候，才会释放）
    char *buf = (char *)malloc(name.size()+1);
    memset(buf, 0, sizeof(buf));
    strcpy(buf, name.data());
    // 保证返回一个持久化的名字
    return llvm::StringRef(buf, strlen(buf));
}


// roundup地址对齐算法, 参数：x: 地址, align: 对齐方式
// 找到大于等于x， 且按照align（必须是2的倍数）对齐的最小地址
/// (0, 1) => 0
/// (1, 4) => 4
/// (4, 4) => 4 
static int roundup(int x, int align) {
    return (x + align -1) & ~(align -1);
}


CRecordType::CRecordType(llvm::StringRef name, const std::vector<Member> &members, TagKind tagKind) : 
    CType(Kind::TY_Record, 0, 0), name(name), members(members), tagKind(tagKind) {
    // update size and align
    if (tagKind == TagKind::KStruct) {
        UpdateStructOffset();
    } else {
        UpdateUnionOffset();
    }
}

void CRecordType::SetMembers(const std::vector<Member>& members) {
    this->members = members;
    // update size and align
    if (tagKind == TagKind::KStruct) {
        UpdateStructOffset();
    } else {
        UpdateUnionOffset();
    }
}


void CRecordType::UpdateStructOffset() {
    int offset = 0;      /* 起始offset位置为0 */

    int total_size = 0;  /* 结构体的大小 */
    int max_align = 0;       /* 结构体的对齐数， 等于结构体成员中最大的align*/
    int max_element_size = 0, max_element_idx = 0;

    int idx = 0;         /* 记录元素位置 */
    for (auto &m : members) {
        offset = roundup(offset, m.ty->GetAlign());
        m.offset = offset;
        m.elemIdx = idx;
        idx++;

        // 求出最大的member的对齐数
        if (max_align < m.ty->GetAlign()) {
            max_align = m.ty->GetAlign();
        }

        if (max_element_size < m.ty->GetSize()) {
            max_element_size = m.ty->GetSize();
            max_element_idx = m.elemIdx;
        }


        // 下一个元素的offset起始位置
        offset += m.ty->GetSize();
    }

    // struct A {int a; float b;}
    total_size = roundup(offset,  max_align);

    this->size = total_size;
    this->align = max_align;
    this->maxElementIdx = max_element_idx;
}


void CRecordType::UpdateUnionOffset() {
    int max_align = 0;
    int max_size = 0;
    int max_element_idx = 0;

    int idx = 0;
    for (auto &m : members) {
        // union的每个元素的offset都是0
        m.offset = 0;
        m.elemIdx = idx++;

        if (max_align < m.ty->GetAlign()) {
            max_align = m.ty->GetAlign();
        }

        if (max_size < m.ty->GetSize()) {
            max_size = m.ty->GetSize();
            max_element_idx = m.elemIdx;
        }
    }
    max_size = roundup(max_size, max_align);
    this->size = max_size;
    this->align = max_align;
    this->maxElementIdx = max_element_idx;
}


CFuncType::CFuncType(std::shared_ptr<CType> retType, const std::vector<Parameter> &params, llvm::StringRef name, bool isVarArg) : 
    CType(Kind::TY_Func, 1, 1), retType(retType), params(params), name(name), isVarArg(isVarArg) {
    
}