#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include "token.hpp"

#include <memory>
#include <variant>
#include <vector>

struct LiteralExpr;
struct UnaryExpr;
struct BinaryExpr;
struct GroupedExpr;
struct CallExpr;

using Expr = std::variant<LiteralExpr, UnaryExpr, CallExpr, BinaryExpr, GroupedExpr>;

struct LiteralExpr
{
    Token literal;
};

struct UnaryExpr
{
    Token op;
    std::unique_ptr<Expr> expr;
};

struct CallExpr
{
    std::unique_ptr<Expr> callee;
    std::vector<std::unique_ptr<Expr>> args;
};

struct BinaryExpr
{
    Token op;
    std::unique_ptr<Expr> lhs;
    std::unique_ptr<Expr> rhs;
};

struct GroupedExpr
{
    std::unique_ptr<Expr> expr;
};

Expr makeLiteral(Token lit);
Expr makeUnary(Token op, std::unique_ptr<Expr> expr);
Expr makeBinary(Token op, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs);
Expr makeGrouped(Expr expr);


#endif