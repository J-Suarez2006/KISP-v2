#include <iostream>

#include "scanner.hpp"
#include "parser.hpp"

#include <string>
#include <vector>


std::string getExprString(const Expr& expr)
{
    std::string result;

    if (std::holds_alternative<LiteralExpr>(expr))
    {
        auto& lit = std::get<LiteralExpr>(expr);
        result += lit.literal.lexeme;
    }
    else if (std::holds_alternative<BinaryExpr>(expr))
    {   
        auto& bin = std::get<BinaryExpr>(expr);
        result += "( ";
        result += getExprString(*bin.lhs);
        result += bin.op.lexeme;
        result += getExprString(*bin.rhs);
        result += " )";
    }
    else if (std::holds_alternative<UnaryExpr>(expr))
    {
        auto& un = std::get<UnaryExpr>(expr);
        result +=  "( ";
        result +=  un.op.lexeme;
        result += getExprString(*un.expr);
        result +=  ")";
    }
    else if (std::holds_alternative<GroupedExpr>(expr))
    {
        auto& grouped = std::get<GroupedExpr>(expr);
        result +=  "( ";
        result += getExprString(*grouped.expr);
        result +=  ")";
    }
    
    return result;
}

int main(int, char**)
{
    std::string source = "23 == 2 + 1.463 * 2 / 3 + 4 + -2 * (3 * 4)";    
    Scanner scanner;
    scanner.loadSource(source);
    auto tokens = scanner.scan();

    Parser parser;
    parser.loadTokens(tokens);
    auto AST = parser.parseExpr();

    std::cout << getExprString(*AST);
    
}
