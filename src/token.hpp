#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

enum TypeToken
{
    // One character
    COMMA, DOT, SEMICOLON, COLON,
    LEFT_PAREN, RIGHT_PAREN, 
    LEFT_CURLY_BRACE, RIGHT_CURLY_BRACE,
    LEFT_SQUARE_BRACE, RIGHT_SQUARE_BRACE,

    // One or two characters
    EQUALS, EQUALS_EQUALS,
    MINUS, MINUS_EQUALS, DECREMENT,
    PLUS, PLUS_EQUALS, INCREMENT,
    ASTERISK, ASTERISK_EQUALS, SLASH, SLASH_EQUALS, 
    MODULUS, MODULUS_EQUALS, 
    BANG, BANG_EQUALS, AND_AND, PIPE_PIPE,
    GREATER_THAN, GREATER_OR_EQUAL, LESS_THAN, LESS_OR_EQUAL,
    ARROW,

    // Literals & values
    STRING_LITERAL, NUMBER, IDENTIFIER, NIL,
    BLIT_TRUE, BLIT_FALSE,

    // Control flow
    IF, ELSE, WHILE, FOR, RETURN, 
    TRY, CATCH, CONTINUE, BREAK,
    
    // Declarations & user-defined types
    LET, SELF, FUNC, STRUCT, ENUM, 

    // Datatype labels
    DTYPE_INT, DTYPE_UINT, DTYPE_FLOAT, DTYPE_DOUBLE, DTYPE_BOOL, DTYPE_STRING, DTYPE_CHAR,

    // Misc
    PRINT,
    
    // End of file
    END_OF_FILE
};

struct Token
{
    TypeToken type;
    std::string lexeme;
    int line;
};


#endif