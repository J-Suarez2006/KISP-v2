#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "parser.hpp"
#include "scanner.hpp"

#include <string>
#include <vector>

std::string getExprString(const Expr& expr)
{
    std::string result = "";

    if (std::holds_alternative<LiteralExpr>(expr))
    {
        auto& lit = std::get<LiteralExpr>(expr);
        result += lit.literal.lexeme;
    }
    else if (std::holds_alternative<BinaryExpr>(expr))
    {   
        auto& bin = std::get<BinaryExpr>(expr);
        result += "(";
        result += getExprString(*bin.lhs);
        result += bin.op.lexeme;
        result += getExprString(*bin.rhs);
        result += ")";
    }
    else if (std::holds_alternative<UnaryExpr>(expr))
    {
        auto& un = std::get<UnaryExpr>(expr);
        result +=  "(";
        result +=  un.op.lexeme;
        result += getExprString(*un.expr);
        result +=  ")";
    }
    else if (std::holds_alternative<GroupedExpr>(expr))
    {
        auto& grouped = std::get<GroupedExpr>(expr);
        result +=  "(";
        result += getExprString(*grouped.expr);
        result +=  ")";
    }
    
    return result;
}

TEST_CASE("Testing Expression Parsing")
{
    Scanner scanner;
    Parser parser;

    std::string source = "5 + 3 * 2 - 5 / 4";
    
    scanner.loadSource(source);
    parser.loadTokens(scanner.scan());

    auto AST = parser.parseExpr();

    std::string ast_string = getExprString(*AST);

    CHECK(ast_string == "(5+((3*2)-(5/4)))");

    source = "8 + -3 / 7";

    scanner.loadSource(source);
    parser.loadTokens(scanner.scan());

    AST = parser.parseExpr();

    ast_string = getExprString(*AST);
    CHECK(ast_string == "(8+((-3)/7))");
}

TEST_CASE("Testing Variable Declarations")
{
    Scanner scanner;
    Parser parser;

    std::string source = "let x : int = 5;";
    
    scanner.loadSource(source);
    auto tokens = scanner.scan();

    parser.loadTokens(tokens);

    auto stmt = std::move(parser.declaration());
    auto& varDecl = std::get<VarDeclStmt>(*stmt);

    CHECK(varDecl.data_type.type == DTYPE_INT);
    CHECK(varDecl.identifier == "x");

    std::cout << getExprString(*varDecl.expr);

    source = "let bob : uint = 7;";
    
    scanner.loadSource(source);
    tokens = scanner.scan();
    parser.loadTokens(tokens);
    stmt = std::move(parser.declaration());
    auto& varDecl2 = std::get<VarDeclStmt>(*stmt);

    CHECK(varDecl2.data_type.type == DTYPE_UINT);
    CHECK(varDecl2.identifier == "bob");

    source = "let josh : bool = 7;";
    
    scanner.loadSource(source);
    tokens = scanner.scan();
    parser.loadTokens(tokens);
    stmt = std::move(parser.declaration());
    auto& varDecl3 = std::get<VarDeclStmt>(*stmt);

    CHECK(varDecl3.data_type.type == DTYPE_BOOL);
    CHECK(varDecl3.identifier == "josh");

    source = "let s : string = \"\";";
    
    scanner.loadSource(source);
    tokens = scanner.scan();
    parser.loadTokens(tokens);
    stmt = std::move(parser.declaration());
    auto& varDecl4 = std::get<VarDeclStmt>(*stmt);

    CHECK(varDecl4.data_type.type == DTYPE_STRING);
    CHECK(varDecl4.identifier == "s");

    source = "let d : double = 54.2;";
    
    scanner.loadSource(source);
    tokens = scanner.scan();
    parser.loadTokens(tokens);
    stmt = std::move(parser.declaration());
    auto& varDecl5 = std::get<VarDeclStmt>(*stmt);

    CHECK(varDecl5.data_type.type == DTYPE_DOUBLE);
    CHECK(varDecl5.identifier == "d");
}

TEST_CASE("Testing Expression Statement")
{
    Scanner scanner;
    Parser parser;

    std::string source = "5 + 2;";
    
    scanner.loadSource(source);
    auto tokens = scanner.scan();

    parser.loadTokens(tokens);
    auto stmt = std::move(parser.parseExprStmt());

    auto expr_string = getExprString(*std::get<ExprStmt>(*stmt).expr);
    CHECK(expr_string == "(5+2)");
}

TEST_CASE("Testing If Statement")
{
    Scanner scanner;
    Parser parser;

    std::string source = "if (3 + 1) 7 + 2; else 6 - 7;";
    
    scanner.loadSource(source);
    auto tokens = scanner.scan();

    parser.loadTokens(tokens);
    auto stmt = std::move(parser.statement());

    auto expr_string = getExprString(*std::get<IfStmt>(*stmt).conditional);

    CHECK(expr_string == "(3+1)");

    auto if_block = std::move(std::get<IfStmt>(*stmt).if_block);
    expr_string = getExprString(*std::get<ExprStmt>(*if_block).expr);

    CHECK(expr_string == "(7+2)");

    auto else_block = std::move(std::get<IfStmt>(*stmt).else_block);
    expr_string = getExprString(*std::get<ExprStmt>(*else_block).expr);

    CHECK(expr_string == "(6-7)");

    source = "if (3 + 1) 7 + 2;";
    
    scanner.loadSource(source);
    tokens = scanner.scan();

    parser.loadTokens(tokens);
    stmt = std::move(parser.statement());
    else_block = std::move(std::get<IfStmt>(*stmt).else_block);

    CHECK(else_block == nullptr);
}

TEST_CASE("Testing For statements")
{
    Scanner scanner;
    Parser parser;

    std::string source = "for(let x : int = 5; 3 + 2; 6 - 7) 5 + 9;";
    
    scanner.loadSource(source);
    auto tokens = scanner.scan();

    parser.loadTokens(tokens);
    auto stmt = std::move(parser.statement());

    auto init = std::move(std::get<ForStmt>(*stmt).init);
    auto cond = std::move(std::get<ForStmt>(*stmt).conditional);
    auto update = std::move(std::get<ForStmt>(*stmt).update);
    auto block = std::move(std::get<ForStmt>(*stmt).stmt);


    auto data_type = std::get<VarDeclStmt>(*init).data_type;
    auto id = std::get<VarDeclStmt>(*init).identifier;
    auto init_expr = getExprString(*std::get<VarDeclStmt>(*init).expr);

    CHECK(data_type.type == DTYPE_INT);
    CHECK(id == "x");
    CHECK(init_expr == "5");

    std::string expr_string = getExprString(*std::get<ExprStmt>(*cond).expr);
    CHECK(expr_string == "(3+2)");

    expr_string = getExprString(*update);
    CHECK(expr_string == "(6-7)");

    expr_string = getExprString(*std::get<ExprStmt>(*block).expr);
    CHECK(expr_string == "(5+9)");
}

TEST_CASE("Testing While Statements")
{
    Scanner scanner;
    Parser parser;

    std::string source = "while(4 + 2) 7 + 2;";
    
    scanner.loadSource(source);
    auto tokens = scanner.scan();

    parser.loadTokens(tokens);
    auto stmt = std::move(parser.statement());
    auto while_stmt = std::move(std::get<WhileStmt>(*stmt));

    std::string expr_string = getExprString(*while_stmt.conditional);
    CHECK(expr_string == "(4+2)");

    auto block = std::move(std::get<ExprStmt>(*while_stmt.stmt).expr);
    expr_string = getExprString(*block);
    CHECK(expr_string == "(7+2)");
}
