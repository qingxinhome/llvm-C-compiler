#include "lexer.h"
#include "parser.h"
#include "printVisitor.h"
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/ErrorOr.h>
#include "codegen.h"
#include "diag_engine.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("please input filename\n");
        return -1;
    }

    const char* filename = argv[1];
    llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> buf = llvm::MemoryBuffer::getFile(filename);
    // std::unique_ptr<llvm::MemoryBuffer> membuf = move(*buf);

    llvm::SourceMgr mgr;
    // 诊断引擎
    DiagEngine diagEngine(mgr);

    mgr.AddNewSourceBuffer(std::move(*buf), llvm::SMLoc());

    Lexer lexer(mgr, diagEngine);

    // Token token;
    // while(true){
    //     lexer.NextToken(token);
    //     if (token.tokenType != TokenType::eof) {
    //         token.Dump();
    //     } else {
    //         break;
    //     }
    // }


    Sema sema(diagEngine);
    Parser parser(lexer, sema);
    auto program = parser.ParseProgram();
    // PrintVisitor printVisitor(program);
    CodeGen codegen(program);

    return 0;
}