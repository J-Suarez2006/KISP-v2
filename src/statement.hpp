#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include "expression.hpp"

#include <variant>
#include <string>
#include <vector>
#include <utility>

struct VarDeclStmt;
struct FuncDefStmt;
struct StructDefStmt;

struct BlockStmt;
struct IfStmt;
struct WhileStmt;
struct ForStmt;
struct ReturnStmt;
struct PrintStmt;
struct ExprStmt;

using Stmt = std::variant<
                        VarDeclStmt, FuncDefStmt, StructDefStmt,
                        IfStmt, ForStmt, WhileStmt, ReturnStmt, PrintStmt, BlockStmt, ExprStmt
                                >;

struct VarDeclStmt
{
    std::string identifier;
    Token data_type;
    std::unique_ptr<Expr> expr;
};

struct FuncDefStmt
{
    std::string identifier;
    std::vector<std::pair<std::string, Token>> param;
    long long unsigned int arity;
    Token return_type;
    std::unique_ptr<Stmt> function;
};

struct StructDefStmt
{
    std::string identifier;
    std::vector<std::unique_ptr<Stmt>> definitions;
};

struct ForStmt
{
    std::unique_ptr<Stmt> init;
    std::unique_ptr<Stmt> conditional;
    std::unique_ptr<Expr> update;
    std::unique_ptr<Stmt> stmt; 
};

struct WhileStmt
{
    std::unique_ptr<Expr> conditional;
    std::unique_ptr<Stmt> stmt;
};

struct IfStmt
{
    std::unique_ptr<Expr> conditional;
    std::unique_ptr<Stmt> if_block;
    std::unique_ptr<Stmt> else_block;
};

struct PrintStmt
{
    std::unique_ptr<Expr> expr;
};

struct ReturnStmt
{
    std::unique_ptr<Expr> expr;
};

struct BlockStmt
{
    std::vector<std::unique_ptr<Stmt>> block;
};

struct ExprStmt
{
    std::unique_ptr<Expr> expr;
};

#endif