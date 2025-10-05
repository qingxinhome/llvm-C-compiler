#include "diag_engine.h"

// 生成消息表
// 替换过程：
// DIAG(err_unknown_char, Error, "unknown char '{0}'") 被替换为 "unknown char '{0}'",
// const char* DiagnosticMessages[] = { "unknown char '{0}'" };
static const char*diag_msg[] = {
    #define DIAG(ID, KIND, MSG) MSG,
    #include "diag.inc"
};

// ## 是预处理器中的连接运算符（token pasting），将 DK_ 和 KIND 拼接为一个标识符。
// 例如，DIAG(err_unknown_char, Error, "unknown char '{0}'") 会生成 DK_Error。
static llvm::SourceMgr::DiagKind diag_kind[] = {
    #define DIAG(ID, KIND, MSG) llvm::SourceMgr::DK_##KIND
    #include "diag.inc"
};



llvm::SourceMgr::DiagKind DiagEngine::GetDiagKind(unsigned id) {
    return diag_kind[id];
}

const char *DiagEngine::GetDiagMsg(unsigned id) {
    return diag_msg[id];
}