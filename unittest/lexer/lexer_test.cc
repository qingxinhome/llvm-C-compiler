#include <gtest/gtest.h>
#include "lexer.h"


// LexerTest 类继承自 ::testing::Test，用于设置和清理测试环境。
class LexerTest : public ::testing::Test
{
public:
    // SetUp 方法在 GoogleTest 中用于在每个测试用例（TEST_F）执行之前初始化测试环境。它的作用是为测试用例提供一致的、
    // 可靠的初始状态，确保每个测试用例独立运行，不受其他测试用例的影响。
    void SetUp() override {
        // 加载测试文件lexer_01.txt, 使用 LLVM 的 MemoryBuffer 读取文件内容。
        static llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> buf = llvm::MemoryBuffer::getFile("../testset/lexer_01.txt");
        if (!buf) {
            llvm::errs() << "can't open file!!!\n";
            return;
        }

        llvm::SourceMgr mgr;
        DiagEngine diagEngine(mgr);

        mgr.AddNewSourceBuffer(std::move(*buf), llvm::SMLoc());

        lexer = new Lexer(mgr, diagEngine);
    }

    // TearDown 方法在 GoogleTest 中用于在每个测试用例执行之后清理测试环境。它的作用是释放
    // 资源、恢复环境状态，避免资源泄漏或对后续测试用例的干扰。
    void TearDown() override {
        // 释放 lexer 对象
        delete lexer;
    }
    Lexer *lexer;
};

/*
int aa, b = 4;
aa=1 ;
*/


// TEST_F 是 GoogleTest 中的一个宏，全称为 "Test Fixture"，
// 用于定义一个测试用例，该用例依赖于一个测试夹具类（继承自 ::testing::Test）
TEST_F(LexerTest, NextToken) {
    /// 正确集
    /// 当前集
    std::vector<Token> expectedVec, curVec;
    expectedVec.push_back(Token{TokenType::kw_int, 1, 1});
    expectedVec.push_back(Token{TokenType::identifier, 1, 5});
    expectedVec.push_back(Token{TokenType::comma, 1, 7});
    expectedVec.push_back(Token{TokenType::identifier, 1, 9});
    expectedVec.push_back(Token{TokenType::equal, 1, 11});
    expectedVec.push_back(Token{TokenType::number, 1, 13});
    expectedVec.push_back(Token{TokenType::semi, 1, 14});
    expectedVec.push_back(Token{TokenType::identifier, 2, 1});
    expectedVec.push_back(Token{TokenType::equal, 2, 3});
    expectedVec.push_back(Token{TokenType::number, 2, 4});
    expectedVec.push_back(Token{TokenType::semi, 2, 6});

    Token token;
    while (true) {
        lexer->NextToken(token);
        if (token.tokenType == TokenType::eof)
            break;
        curVec.push_back(token);
    }

    ASSERT_EQ(expectedVec.size(), curVec.size());
    for (int i = 0; i < expectedVec.size(); i++) {
        const auto &expected_token = expectedVec[i];
        const auto &cur_token = curVec[i];

        EXPECT_EQ(expected_token.tokenType, cur_token.tokenType);
        EXPECT_EQ(expected_token.row, cur_token.row);
        EXPECT_EQ(expected_token.col, cur_token.col);
    }
}


/*
1. llvm::MemoryBuffer::getFile 返回 llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>>，表示：
成功：返回 std::unique_ptr<llvm::MemoryBuffer>（文件内容的智能指针）。
失败：返回 std::error_code（如文件不存在或无权限）。


2. 
TEST_F 是 GoogleTest 中的一个宏，全称为 "Test Fixture"，用于定义一个测试用例，该用例依赖于一个测试夹具类（继承自 ::testing::Test）。
它的主要作用是：
关联测试夹具：允许测试用例访问测试夹具类（如 LexerTest）的成员变量和方法（如 SetUp、TearDown 和 lexer）。

语法：
TEST_F(TestFixtureClassName, TestName) {
    // 测试代码
}

TestFixtureClassName：测试夹具类的名称（如 LexerTest）。
TestName：测试用例的名称（如 NextToken）。
在测试中，可以直接访问夹具类的成员（如 lexer）。
*/