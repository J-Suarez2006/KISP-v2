#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "scanner.hpp"

#include <iostream>

TEST_CASE("Testing keywords") 
{
    std::string source = "if else nil while for try "
    "catch continue break return print true false "
    "let self struct enum int double char bool string void";

    std::vector<TypeToken> token_type_test = {
        IF, ELSE, NIL, WHILE, FOR, TRY, CATCH, CONTINUE,
        BREAK, RETURN, PRINT, BLIT_TRUE, BLIT_FALSE,
        LET, SELF, STRUCT, ENUM, DTYPE_INT, DTYPE_DOUBLE,
        DTYPE_CHAR, DTYPE_BOOL, DTYPE_STRING, DTYPE_VOID
    };

    Scanner scanner;
    scanner.loadSource(source);

    std::vector<Token> tokens = scanner.scan();


    for (int i = 0; i < token_type_test.size(); ++i)
    {
        CHECK(tokens[i].type == token_type_test[i]);
        
        std::cout << "Token #" << i << " info:\n"
                  << "Lexeme: " << tokens[i].lexeme << '\n'
                  << "Enum: " << tokens[i].type << '\n'
                  << "Expected: " << token_type_test[i] << '\n';

    }
}

TEST_CASE("Testing literals") 
{
    std::string source = "35 32 76854 23.54 12345";
    
    Scanner scanner;
    scanner.loadSource(source);
    std::vector<Token> tokens = scanner.scan();

    for (int i = 0; i < tokens.size() - 1; ++i)
    {
        CHECK(tokens[i].type == NUMBER);
        std::cout << "Token #" << i << " info:\n"
                  << "Lexeme: " << tokens[i].lexeme << '\n'
                  << "Enum: " << tokens[i].type << '\n';
    }
    
    tokens.clear();
    source.clear();
    source = "\"rwjidofj\" \"dsfewfa\"";
    std::cout << source;
    scanner.loadSource(source);
    tokens = scanner.scan();

    for (int i = 0; i < tokens.size() - 1; ++i)
    {
        CHECK(tokens[i].type == STRING_LITERAL);
        std::cout << "Token #" << i << " info:\n"
                  << "Lexeme: " << tokens[i].lexeme << '\n'
                  << "Enum: " << tokens[i].type << '\n';
    }

    tokens.clear();
    source.clear();
    source = "nil";

    scanner.loadSource(source);
    tokens = scanner.scan();
    CHECK(tokens[0].type == NIL);

    tokens.clear();
    source.clear();
    source = "true false";

    scanner.loadSource(source);
    tokens = scanner.scan();
    CHECK(tokens[0].type == BLIT_TRUE);
    CHECK(tokens[1].type == BLIT_FALSE);

}

TEST_CASE("Testing symbols")
{
    std::string source = ",.;:(){}[] "
    "= == - -= -- + += ++ * *= / /= \% \%= ! != "
    "&& || > >= < <= ->";
    std::vector<TypeToken> token_type_test = {
        COMMA, DOT, SEMICOLON, COLON, LEFT_PAREN, RIGHT_PAREN,
        LEFT_CURLY_BRACE, RIGHT_CURLY_BRACE, LEFT_SQUARE_BRACE,
        RIGHT_SQUARE_BRACE, EQUALS, EQUALS_EQUALS, MINUS, MINUS_EQUALS, 
        DECREMENT, PLUS, PLUS_EQUALS, INCREMENT, ASTERISK, ASTERISK_EQUALS, 
        SLASH, SLASH_EQUALS, MODULUS, MODULUS_EQUALS, BANG, BANG_EQUALS, 
        AND_AND, PIPE_PIPE, GREATER_THAN, GREATER_OR_EQUAL, LESS_THAN, 
        LESS_OR_EQUAL, ARROW
    };

    Scanner scanner;
    scanner.loadSource(source);
    std::vector<Token> tokens = scanner.scan();

    for (int i = 0; i < token_type_test.size(); ++i)
    {
        CHECK(tokens[i].type == token_type_test[i]);
        
        std::cout << "Token #" << i << " info:\n"
                  << "Lexeme: " << tokens[i].lexeme << '\n'
                  << "Enum: " << tokens[i].type << '\n'
                  << "Expected: " << token_type_test[i] << '\n';

    }
}

