#pragma once

#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/FormatVariadic.h"

namespace diag {

//结果形式： enum class diag { err_unknown_char };
// 相当于定义了枚举值的索引，或者数组的下标
enum diag {
    #define DIAG(ID, KIND, MSG) ID,
    #include "diag.inc"
};
}


class DiagEngine {
private:
    llvm::SourceMgr &mgr;
private:
    llvm::SourceMgr::DiagKind GetDiagKind(unsigned id);
    const char *GetDiagMsg(unsigned id);
public:
    DiagEngine(llvm::SourceMgr &mgr) : mgr(mgr) {}

    template<typename... Args>
    void Report(llvm::SMLoc loc, unsigned diagId, Args... args) {

        auto kind = GetDiagKind(diagId);
        const char * fmtmsg = GetDiagMsg(diagId);
        auto f = llvm::formatv(fmtmsg, std::forward<Args>(args)...).str();
        mgr.PrintMessage(loc, kind, f);

        if (kind == llvm::SourceMgr::DK_Error) {
            exit(0);
        }
    }
};