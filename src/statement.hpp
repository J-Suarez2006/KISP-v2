#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include "expression.hpp"
#include "scope.hpp"
#include "token.hpp"

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
    Scope* scope;
};

struct FuncDefStmt
{
    std::string identifier;
    std::vector<std::pair<std::string, Token>> param;
    long long unsigned int arity;
    Token return_type;
    std::unique_ptr<Stmt> function;
    Token left_curly_brace;
    Scope* scope;
};

struct StructDefStmt
{
    std::string identifier;
    std::vector<std::unique_ptr<Stmt>> definitions;
    Token left_curly_brace;
    Scope* scope;
};

struct ForStmt
{
    std::unique_ptr<Stmt> init;
    std::unique_ptr<Stmt> conditional;
    std::unique_ptr<Expr> update;
    std::unique_ptr<Stmt> stmt; 
    Scope* scope;
};

struct WhileStmt
{
    std::unique_ptr<Expr> conditional;
    std::unique_ptr<Stmt> stmt;
    Scope* scope;
};

struct IfStmt
{
    std::unique_ptr<Expr> conditional;
    std::unique_ptr<Stmt> if_block;
    std::unique_ptr<Stmt> else_block;
    Scope* scope;
};

struct PrintStmt
{
    std::unique_ptr<Expr> expr;
    Scope* scope;
};

struct ReturnStmt
{
    std::unique_ptr<Expr> expr;
    Scope* scope;
};

struct BlockStmt
{
    std::vector<std::unique_ptr<Stmt>> block;
    Scope* scope;
};

struct ExprStmt
{
    std::unique_ptr<Expr> expr;
    Scope* scope;
};

#endif