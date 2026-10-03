#ifndef PARSER_HPP
#define PARSER_HPP

#include "token.hpp"
#include "expression.hpp"
#include "error.hpp"
#include "statement.hpp"

#include <stdexcept>
#include <vector>

class Parser
{
    private:
    std::vector<Token> m_tokens;
    int m_idx;

    void synchronize();

    const inline int line()
    {
        return m_tokens[m_idx].line;
    }

    inline bool isAtEnd()
    {
        return m_tokens[m_idx].type == END_OF_FILE;
    }

    inline Token peek()
    {
        return m_tokens[m_idx];
    }

    inline Token prev()
    {
        return m_tokens[m_idx - 1];
    }

    inline Token advance()
    {
        if (!isAtEnd()) m_idx++;
        return prev();
    }

    inline bool check(TypeToken token_type)
    {
        if (isAtEnd()) return false;
        return peek().type == token_type;
    }

    inline bool match(std::vector<TypeToken> token_types)
    {
        for (auto type : token_types)
        {
            if (check(type)) 
            {
                advance();
                return true;
            }
        }
        
        return false;
    }

    inline Token consume(TypeToken token_type, std::string message)
    {
        if(check(token_type)) return advance();

        syntaxError(line(), message);
        synchronize();
        throw std::runtime_error(message);
    }

    inline Token consume(std::vector<TypeToken> token_types, std::string message)
    {
        for(auto& type : token_types)
        {
            if (check(type)) return advance();
        }

        syntaxError(line(), message);
        synchronize();
        throw std::runtime_error(message);
    }

    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();

    public:
    Parser();
    void loadTokens(std::vector<Token> tokens);
    
    void reset();
    std::unique_ptr<Expr> parseExpr();

    std::unique_ptr<Stmt> parseIfStmt();
    std::unique_ptr<Stmt> parseForStmt();
    std::unique_ptr<Stmt> parseWhileStmt();
    std::unique_ptr<Stmt> parseReturnStmt();
    std::unique_ptr<Stmt> parsePrintStmt();
    std::unique_ptr<Stmt> parseBlockStmt();
    std::unique_ptr<Stmt> parseExprStmt();

    std::unique_ptr<Stmt> statement();    

    std::unique_ptr<Stmt> parseVarDeclStmt();    

    std::unique_ptr<Stmt> declaration();

    

    std::vector<std::unique_ptr<Stmt>> parse();
};

#endif