#include "parser.hpp"

Parser::Parser()
{
    m_idx = 0;
    m_tokens.clear();
}
void Parser::loadTokens(std::vector<Token> tokens)
{
    m_idx = 0;
    m_tokens.clear();
    m_tokens = tokens;
}
void Parser::reset()
{
    m_idx = 0;
    m_tokens.clear();
}

void Parser::synchronize()
{
    while(!match({SEMICOLON, RIGHT_CURLY_BRACE}) && !isAtEnd())
    {
        advance();
    }

    if (isAtEnd()) return;

    advance();
}

/* Expression Parsing */
std::unique_ptr<Expr> Parser::parseExpr()
{
    return assignment();
}
std::unique_ptr<Expr> Parser::assignment()
{
    auto expr = logicalOr();

    while (match({EQUALS}))
    {
        Token op = prev();
        auto rhs = logicalOr();

        expr = std::make_unique<Expr>(BinaryExpr{
            .op = op,
            .lhs = std::move(expr),
            .rhs = std::move(rhs)
        });
    }

    return expr;
}
std::unique_ptr<Expr> Parser::logicalOr()
{
    auto expr = logicalAnd();

    while (match({PIPE_PIPE}))
    {
        Token op = prev();
        auto rhs = logicalAnd();

        expr = std::make_unique<Expr>(BinaryExpr{
            .op = op,
            .lhs = std::move(expr),
            .rhs = std::move(rhs)
        });
    }

    return expr;
}
std::unique_ptr<Expr> Parser::logicalAnd()
{
    auto expr = equality();

    while (match({AND_AND}))
    {
        Token op = prev();
        auto rhs = equality();

        expr = std::make_unique<Expr>(BinaryExpr{
            .op = op,
            .lhs = std::move(expr),
            .rhs = std::move(rhs)
        });
    }

    return expr;
}
std::unique_ptr<Expr> Parser::equality()
{
    auto expr = comparison();

    while (match({BANG_EQUALS, EQUALS_EQUALS}))
    {
        Token op = prev();
        auto rhs = comparison();

        expr = std::make_unique<Expr>(BinaryExpr{
            .op = op, 
            .lhs = std::move(expr), 
            .rhs = std::move(rhs)
        });
    }

    return expr;
}
std::unique_ptr<Expr> Parser::comparison()
{
    auto expr = term();

    while (match({
        GREATER_THAN, GREATER_OR_EQUAL,
        LESS_THAN, LESS_OR_EQUAL
    }))
    {
        Token op = prev();
        auto rhs = term();

        expr = std::make_unique<Expr>(BinaryExpr{
            .op = op, 
            .lhs = std::move(expr),
            .rhs = std::move(rhs)
        });
    }

    return expr;
}
std::unique_ptr<Expr> Parser::term()
{
    auto expr = factor();

    while (match({
        MINUS, PLUS
    }))
    {
        Token op = prev();
        auto rhs = term();

        expr = std::make_unique<Expr>(BinaryExpr{
            .op = op,
            .lhs = std::move(expr),
            .rhs = std::move(rhs)
        });
    }

    return expr;
}
std::unique_ptr<Expr> Parser::factor()
{
    auto expr = unary();

    while (match({ASTERISK, SLASH}))
    {
        Token op = prev();
        auto rhs = unary();

        expr = std::make_unique<Expr>(BinaryExpr{
            .op = op,
            .lhs = std::move(expr),
            .rhs = std::move(rhs)
        });
    }

    return expr;
}
std::unique_ptr<Expr> Parser::unary()
{
    if (match({BANG, MINUS}))
    {
        Token op = prev();
        auto expr = call();

        return std::make_unique<Expr>(UnaryExpr{
            .op = op,
            .expr = std::move(expr)
        });
    }

    return call();
}
std::unique_ptr<Expr> Parser::call()
{
    auto expr = primary();

    while (match({LEFT_PAREN}))
    {
        std::vector<std::unique_ptr<Expr>> args;

        if (!check(RIGHT_PAREN))
        {
            do 
            {
                args.push_back(parseExpr());
            } while (match({COMMA}));
        }

        consume({RIGHT_PAREN}, "Expected \')\' after call.");

        expr = std::make_unique<Expr>(CallExpr{
            .callee = std::move(expr),
            .args = std::move(args)
        });
    }

    return expr;
}
std::unique_ptr<Expr> Parser::primary()
{
    if (match({
        BLIT_FALSE, BLIT_TRUE, NUMBER, STRING_LITERAL, NIL, IDENTIFIER
    }))
    {
        return std::make_unique<Expr>(LiteralExpr{
            .literal = prev()
        });
    }

    if (match({LEFT_PAREN}))
    {
        auto expr = parseExpr();
        consume(RIGHT_PAREN, "Expected \'}\'");
        return std::make_unique<Expr>(GroupedExpr{
            .expr = std::move(expr)
        });
    }
}


/* Statement Parsing */
std::unique_ptr<Stmt> Parser::parseWhileStmt()
{
    consume(LEFT_PAREN, "Expected \'(\'");

    auto conditional = std::move(parseExpr());

    consume(RIGHT_PAREN, "Expected \')\'");

    auto stmt = std::move(statement());

    return std::make_unique<Stmt>(WhileStmt{
        .conditional = std::move(conditional),
        .stmt = std::move(stmt)
    });
}

std::unique_ptr<Stmt> Parser::parseIfStmt()
{
    consume(LEFT_PAREN, "Expected \'(\'");

    auto conditional = std::move(parseExpr());

    consume(RIGHT_PAREN, "Expected \')\'");

    auto if_block = std::move(statement());
    
    if(match({ELSE}))
    {
        auto else_block = std::move(statement());

        return std::make_unique<Stmt>(IfStmt{
            .conditional = std::move(conditional),
            .if_block = std::move(if_block),
            .else_block = std::move(else_block)
        });
    }

    return std::make_unique<Stmt>(IfStmt{
        .conditional = std::move(conditional),
        .if_block = std::move(if_block),
        .else_block = nullptr
    });
}

std::unique_ptr<Stmt> Parser::parseForStmt()
{
    consume(LEFT_PAREN, "Expected \'(\'");

    auto init = std::move(declaration());

    
    auto cond = std::move(parseExprStmt());
    auto update = std::move(parseExpr());


    std::cout << m_tokens[m_idx].lexeme;

    consume(RIGHT_PAREN, "Expected \')\'");

    auto stmt = std::move(statement());

    return std::make_unique<Stmt>(ForStmt{
        .init = std::move(init),
        .conditional = std::move(cond),
        .update = std::move(update),
        .stmt = std::move(stmt)
    });
}

std::unique_ptr<Stmt> Parser::parseReturnStmt()
{
    auto expr = std::move(parseExpr());

    consume({SEMICOLON}, "expected \';\''");

    return std::make_unique<Stmt>(ReturnStmt{
        .expr = std::move(expr)
    });
}

std::unique_ptr<Stmt> Parser::parsePrintStmt()
{
    consume({LEFT_PAREN}, "Expected \'(\'");
    auto expr = std::move(parseExpr());
    consume({RIGHT_PAREN}, "Expected \')\'");

    consume({SEMICOLON}, "Expected \';\''");

    return std::make_unique<Stmt>(ReturnStmt{
        .expr = std::move(expr)
    });
}

std::unique_ptr<Stmt> Parser::parseBlockStmt()
{
    std::vector<std::unique_ptr<Stmt>> block;

    while(!match({RIGHT_CURLY_BRACE}))
    {
        block.push_back(declaration());
    }

    return std::make_unique<Stmt>(BlockStmt{
        .block = std::move(block)
    });
}  

std::unique_ptr<Stmt> Parser::parseExprStmt()
{
    auto expr = std::move(parseExpr());
    consume(SEMICOLON, "Expected \';\'.");

    return std::make_unique<Stmt>(ExprStmt{
        std::move(expr)
    });
}

std::unique_ptr<Stmt> Parser::statement()
{
    if (match({IF})) return parseIfStmt();
    if (match({FOR})) return parseForStmt();
    if (match({WHILE})) return parseWhileStmt();
    if (match({RETURN})) return parseReturnStmt();
    if (match({PRINT})) return parsePrintStmt();
    return parseExprStmt();
}

/* Declarations */
std::unique_ptr<Stmt> Parser::parseVarDeclStmt()
{
    consume(IDENTIFIER, "Expected identifier.");

    Token id = prev();

    consume(COLON, "expected \':\'");

    consume({
        DTYPE_BOOL, DTYPE_CHAR, DTYPE_DOUBLE, 
        DTYPE_FLOAT, DTYPE_INT, DTYPE_UINT, DTYPE_STRING, IDENTIFIER
    }, "Expected data type.");

    auto data_type = prev();

    if(match({EQUALS}))
    {
        auto expr = parseExpr();
        consume({SEMICOLON}, "Expected \';\'");
        return std::make_unique<Stmt>(VarDeclStmt{
            .identifier = id.lexeme,
            .data_type = data_type,
            .expr = std::move(expr)
        });
    }

    consume({SEMICOLON}, "Expected \';\'");
    return std::make_unique<Stmt>(VarDeclStmt{
        .identifier = id.lexeme,
        .data_type = data_type,
        .expr = nullptr
    });
}

std::unique_ptr<Stmt> Parser::parseFuncDefStmt()
{
    consume({IDENTIFIER}, "Expected identifier.");
    std::string id = prev().lexeme;
    consume({LEFT_PAREN}, "Expected \'(\'.");

    std::vector<std::pair<std::string, Token>> param;

    while(match({IDENTIFIER}))
    {
        std::string param_id = prev().lexeme;

        consume({COLON}, "Expected \':\'.");

        Token data_type = consume({
            DTYPE_INT, DTYPE_UINT, DTYPE_FLOAT, DTYPE_DOUBLE, 
            DTYPE_BOOL, DTYPE_STRING, DTYPE_CHAR, IDENTIFIER
        }, "Expected parameter datatype.");

        param.push_back({param_id, data_type});

        if (check(RIGHT_PAREN)) break;
        
        consume({COMMA}, "Expected \',\'/");
    }

    std::cout << m_tokens[m_idx].lexeme;
    consume({RIGHT_PAREN}, "Expected \')\'.");
    consume({ARROW}, "Expected \')\'.");

    Token return_type = consume({
            DTYPE_INT, DTYPE_UINT, DTYPE_FLOAT, DTYPE_DOUBLE, 
            DTYPE_BOOL, DTYPE_STRING, DTYPE_CHAR, DTYPE_VOID, IDENTIFIER}, 
            "Expected return type.");

    consume({LEFT_CURLY_BRACE}, "Expected \'{\'.");

    auto block = parseBlockStmt();

    return std::make_unique<Stmt>(FuncDefStmt{
        .identifier = id,
        .param = param,
        .arity = param.size(),
        .return_type = return_type,
        .function = std::move(block)
    });
}

std::unique_ptr<Stmt> Parser::parseStructDefStmt()
{
    consume({IDENTIFIER}, "Expected identifier.");
    std::string id = prev().lexeme;
    consume({LEFT_CURLY_BRACE}, "Expected \'{\'.");

    std::vector<std::unique_ptr<Stmt>> definitions;

    while(!match({RIGHT_CURLY_BRACE}))
    {
        definitions.push_back(declaration());
    }

    consume({SEMICOLON}, "Expected \';\'.");

    return std::make_unique<Stmt>(StructDefStmt{
        .identifier = id, .definitions = std::move(definitions)
    });
}

std::unique_ptr<Stmt> Parser::declaration()
{
    if(match({LET})) return parseVarDeclStmt();
    if(match({FUNC})) return parseFuncDefStmt();
    if(match({STRUCT})) return parseStructDefStmt();

    return statement();
}

// Parse
std::vector<std::unique_ptr<Stmt>> Parser::parse()
{
    std::vector<std::unique_ptr<Stmt>> result;

    while (!isAtEnd())
    {
        try 
        {
            result.push_back(declaration());
        }
        catch (std::runtime_error& e)
        {
            synchronize();
        }
    }

    return result;
}