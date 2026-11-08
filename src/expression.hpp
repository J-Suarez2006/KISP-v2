#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include "token.hpp"
#include "scope.hpp"

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
    Type inferred_type;
};

struct UnaryExpr
{
    Token op;
    std::unique_ptr<Expr> expr;
    Type inferred_type;
};

struct CallExpr
{
    std::unique_ptr<Expr> callee;
    Token l_paren;
    std::vector<std::unique_ptr<Expr>> args;
    Type inferred_type;
};

struct BinaryExpr
{
    Token op;
    std::unique_ptr<Expr> lhs;
    std::unique_ptr<Expr> rhs;
    Type inferred_type;
};

struct GroupedExpr
{
    Token l_paren;
    std::unique_ptr<Expr> expr;
    Type inferred_type;
};

Expr makeLiteral(Token lit);
Expr makeUnary(Token op, std::unique_ptr<Expr> expr);
Expr makeBinary(Token op, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs);
Expr makeGrouped(Expr expr);


#endif