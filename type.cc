#include "type.h"

// CType* CType::GetIntTy() {
//     static CType intType(TypeKind::Int, 4, 4);
//     return &intType;
// }

// 静态成员变量必须在类内声明，在类外初始化
std::shared_ptr<CType> CType::IntType = std::make_shared<CPrimaryType>(Kind::TY_Int, 4, 4);



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